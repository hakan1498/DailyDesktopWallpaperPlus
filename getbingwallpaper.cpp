#include "getbingwallpaper.h"
#include "manage_database.h"

#include <QUrl>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>
#include <QtXml/QDomElement>
#include <QtXml/QDomDocument>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QApplication>
#include <QPixmap>
#include <QSettings>
#include <QDebug>
#include <QThread>

void GetBingWallpaper::get_bing_wallpaper()
{
    bing_basefile_parse();
    download_photo_option();
    add_record();
}

void GetBingWallpaper::bing_basefile_parse()
{
    get_bing_basefile();

    QString baseUrl = "https://www.bing.com";

    QString _basefile_content;

    QFile xmlfile;
    xmlfile.setFileName(QDir::homePath()+"/.DailyDesktopWallpaperPlus/basefile.xml");
    xmlfile.open(QIODevice::ReadOnly | QIODevice::Text);
    _basefile_content = xmlfile.readAll();
    xmlfile.close();

    QDomDocument basefile;

    basefile.setContent(_basefile_content);

    QDomElement element= basefile.documentElement().firstChild().firstChild().toElement();

    while(!element.isNull()) {
          if(element.tagName()=="urlBase") {
              urlBase = element.firstChild().toText().data();
          }
          if(element.tagName()=="copyright") {
              _copyright_bing_photo = element.firstChild().toText().data();
          }
          if(element.tagName()=="copyrightlink") {
              _copyright_link = element.firstChild().toText().data();
          }
          if(element.tagName()=="headline") {
              _headline_bing_desc = element.firstChild().toText().data();
          }      
          // go to next element
          element = element.nextSibling().toElement();
    }

    _bing_photo_url = baseUrl+urlBase+"_"+_bingresolution+".jpg";

    write_settings();
    remove_baseFile();
}

void GetBingWallpaper::get_bing_basefile()
{
    read_settings();

    QString url = "https://www.bing.com/HPImageArchive.aspx?format=xml&idx=0&n=1&mkt="+_binglocation;

    download_photo = false;
    download_file();

    QString filename = QDir::homePath()+"/.DailyDesktopWallpaperPlus/basefile.xml";

    QFile xmlFile(filename);
    if(QFileInfo::exists(filename))
    {
        xmlFile.remove();
    }

    if (xmlFile.open(QIODevice::ReadWrite)) {
        QTextStream stream(&xmlFile);
        stream << _bing_reply << endl;
    }
}

void GetBingWallpaper::read_settings()
{
    _iniFilePath = QDir::homePath()+"/.DailyDesktopWallpaperPlus/settings.ini";

    QSettings _settings(_iniFilePath, QSettings::IniFormat);

    _settings.beginGroup("BING-SETTINGS");
    _binglocation = _settings.value("binglocation","").toString();
    _bingresolution = _settings.value("resolution","").toString();
    _settings.endGroup();

    _settings.beginGroup("SETTINGS");
    _WallpaperDir = _settings.value("WallpaperDir","").toString();
    _thumbfiledir = _settings.value("ThumbFileDir","").toString();
    _settings.endGroup();

    _iniFilePath.clear();
}

void GetBingWallpaper::remove_baseFile()
{
    QFile baseFile(QDir::homePath()+"/.DailyDesktopWallpaperPlus/basefile.xml");
    baseFile.remove();
}

void GetBingWallpaper::write_settings()
{
    _iniFilePath = QDir::homePath()+"/.DailyDesktopWallpaperPlus/settings.ini";

    QSettings _settings(_iniFilePath, QSettings::IniFormat);

    _settings.beginGroup("SETTINGS");
    _settings.setValue("current_photo_dl_url", _bing_photo_url);
    _settings.endGroup();
    _settings.sync();
}

void GetBingWallpaper::download_photo_option()
{
    download_photo = true;
    download_file();
}

void GetBingWallpaper::download_file()
{
    QString _download_url;

    // if download photo = true, then set bing_photo_url as download-url;
    // else set bing_basefile_url as download_url;

    if(download_photo == true) {
       _download_url = _bing_photo_url;
    } else
    {
       _download_url = "https://www.bing.com/HPImageArchive.aspx?format=xml&idx=0&n=1&mkt="+_binglocation;
    }

    QEventLoop loop;
    QObject::connect(&dl_manager,&QNetworkAccessManager::finished,&loop,&QEventLoop::quit);
    reply = dl_manager.get(QNetworkRequest(_download_url));
    loop.exec();

    //Check connection to the internet
    if (reply->bytesAvailable())
    {
        fileDownloaded();
    }
}

void GetBingWallpaper::fileDownloaded() {

    // download content of the photo to bytearray;
    // If false then save content to QString _bing_reply

    if(download_photo == true) {
       downloaded_photo_data = reply->readAll();
       saveImage();
    } else
    {
       _bing_reply = reply->readAll();
    }
}

QByteArray GetBingWallpaper::downloadedPhotoData() const {
    return downloaded_photo_data;
}

void GetBingWallpaper::saveImage()
{
    read_settings();
    filename = QDateTime::currentDateTime().toString("yyyyMMddHHmmss")+"-background.jpg";

    QPixmap photo_wallpaper;
    photo_wallpaper.loadFromData(downloadedPhotoData());
    photo_wallpaper.save(_WallpaperDir+"/"+filename);
    _picture_size_height = photo_wallpaper.size().height();
    _picture_size_width = photo_wallpaper.size().width();

    // Create a thumbnail picture of the wallpaper for photobrowser;
    int _thumb_size_height;
    int _thumb_size_width;

    if(_bingresolution=="1024x768") {
        _thumb_size_height = (_picture_size_height/100)*15;
        _thumb_size_width =  (_picture_size_width/100)*15;
    } else if(_bingresolution=="1280x720") {
        _thumb_size_height = (_picture_size_height/100)*15;
        _thumb_size_width =  (_picture_size_width/100)*15;
    } else if(_bingresolution=="1366x768") {
        _thumb_size_height = (_picture_size_height/100)*15;
        _thumb_size_width =  (_picture_size_width/100)*15;
    } else if(_bingresolution=="1920x1080") {
        _thumb_size_height = (_picture_size_height/100)*9;
        _thumb_size_width =  (_picture_size_width/100)*9;
    } else if(_bingresolution=="1920x1200") {
        _thumb_size_height = (_picture_size_height/100)*9;
        _thumb_size_width =  (_picture_size_width/100)*9;
    } else if(_bingresolution=="UHD") {
        _thumb_size_height = 172;
        _thumb_size_width =  97;
    };

    _thumb_filename = QDateTime::currentDateTime().toString("yyyyMMddHHmmss")+"-thumb.jpg";

    QPixmap _thumb = photo_wallpaper.scaled(QSize(_thumb_size_width, _thumb_size_height),  Qt::KeepAspectRatio, Qt::SmoothTransformation);

    //Check if Thumbfile-Directory exist; Create it, if not exist.
    QDir _thumbfile_dir(_thumbfiledir);
    if(!_thumbfile_dir.exists()) {
        _thumbfile_dir.mkpath(_thumbfiledir);
    }

    _thumb.save(_thumbfiledir+"/"+_thumb_filename);
}

void GetBingWallpaper::add_record()
{
    manage_database ManageDatabase;
    ManageDatabase.init_database();
    if(ManageDatabase._initDB_failed==false)
    {
        ManageDatabase._add_record_urlBase = urlBase;
        ManageDatabase._add_record_copyright_description = _copyright_bing_photo;
        ManageDatabase._add_record_copyright_link = _copyright_link;
        ManageDatabase._add_record_headline = _headline_bing_desc;
        ManageDatabase._add_record_filename = filename;
        ManageDatabase._size_height = _picture_size_height;
        ManageDatabase._size_width = _picture_size_width;
        ManageDatabase._thumb_filename = _thumb_filename;
        ManageDatabase.add_record();
    } else {
        qDebug() << "Error while initializing Database.";
    }
    QThread::msleep(100);
}
