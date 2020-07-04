#include "getwinspotwallpaper.h"
#include "createfilename.h"

#include <QUrl>
#include <QObject>
#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QTextStream>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QApplication>
#include <QJsonValue>
#include <QPixmap>
#include <QSettings>

void GetWinSpotWallpaper::get_wallpaper()
{
    parse_json_wspot();
    download_photo_option();
}

void GetWinSpotWallpaper::parse_json_wspot()
{
    get_json_file();

    QString _json_content;
    QFile file;
    file.setFileName(QDir::homePath()+"/.DailyDesktopWallpaperPlus/cache.json");
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    _json_content = file.readAll();
    file.close();

    QJsonDocument jsondoc = QJsonDocument::fromJson(_json_content.toUtf8());
    QJsonObject root = jsondoc.object();
    QJsonValue _items_value = jsondoc.object().value("batchrsp").toObject().value("items");

    // get content of object "item"
    QString _item_content;
    QJsonObject _item;

    if(_items_value.isArray())
    {
        QJsonArray _array0 = _items_value.toArray();

        for(int i = 0; i < _array0.count(); i++)
        {
            _item = _array0.at(i).toObject();
            _item_content = _item.value("item").toString();
        }
    }

    // parse content of object "item" in a second part
    QJsonDocument _item_doc = QJsonDocument::fromJson(_item_content.toUtf8());
    QJsonObject _item_root = _item_doc.object();

    QJsonValue _u = _item_root.value("ad").toObject().value("image_fullscreen_001_landscape").toObject().value("u");
    _wspot_photo_url = _u.toString();

    QJsonValue _hs1_title_text = _item_root.value("ad").toObject().value("hs1_title_text").toObject().value("tx");
    _wspot_photo_description = _hs1_title_text.toString();

    QJsonValue _hs1_cta_text = _item_root.value("ad").toObject().value("hs1_cta_text").toObject().value("tx");
    _wspot_title_text = _hs1_cta_text.toString();

    QJsonValue _bing_searchlink_json = _item_root.value("ad").toObject().value("hs1_destination_url").toObject().value("u");
    _bing_searchlink = _bing_searchlink_json.toString();
    _bing_searchlink.remove("microsoft-edge:");

    write_settings();
    remove_jsonFile();
}

void GetWinSpotWallpaper::get_json_file()
{
    download_photo = false;
    download_file();

    QString filename = QDir::homePath()+"/.DailyDesktopWallpaperPlus/cache.json";

    QFile jsonFile(filename);
    if(QFileInfo::exists(filename))
    {
        jsonFile.remove();
    }

    if (jsonFile.open(QIODevice::ReadWrite)) {
        QTextStream stream(&jsonFile);
        stream << _reply << endl;
    }
}

void GetWinSpotWallpaper::read_settings()
{
    _iniFilePath = QDir::homePath()+"/.DailyDesktopWallpaperPlus/settings.ini";

    QSettings _settings(_iniFilePath, QSettings::IniFormat);

    _settings.beginGroup("SETTINGS");
    _WallpaperDir = _settings.value("WallpaperDir","").toString();
    _settings.endGroup();

    _iniFilePath.clear();
}

void GetWinSpotWallpaper::remove_jsonFile()
{
    QFile jsonFile(QDir::homePath()+"/.DailyDesktopWallpaperPlus/cache.json");
    jsonFile.remove();
}

void GetWinSpotWallpaper::write_settings()
{
    _iniFilePath = QDir::homePath()+"/.DailyDesktopWallpaperPlus/settings.ini";

    QSettings _settings(_iniFilePath, QSettings::IniFormat);

    _settings.beginGroup("SETTINGS");
    _settings.setValue("current_photo_dl_url", _wspot_photo_url);
    _settings.setValue("current_description", _wspot_photo_description);
    _settings.setValue("current_title", _wspot_title_text);
    _settings.endGroup();
    _settings.sync();
}

void GetWinSpotWallpaper::download_photo_option()
{
    download_photo = true;
    download_file();
}

void GetWinSpotWallpaper::download_file()
{
    QString _download_url;

    // if download photo = true, then set bing_photo_url as download-url;
    // else set json_url of windows spotlight as download_url;

    if(download_photo == true) {
       _download_url = _wspot_photo_url;
    } else
    {
       _download_url = "https://arc.msn.com/v3/Delivery/Cache?pid=279978&fmt=json&ua=WindowsShellClient&lc=en,en-US&ctry=US";
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

void GetWinSpotWallpaper::fileDownloaded() {

    // download content of the photo to bytearray;
    // If false then save content to QString _reply

    if(download_photo == true) {
       downloaded_photo_data = reply->readAll();
       saveImage();
    } else
    {
       _reply = reply->readAll();
    }
}

QByteArray GetWinSpotWallpaper::downloadedPhotoData() const {
    return downloaded_photo_data;
}

void GetWinSpotWallpaper::saveImage()
{
    read_settings();

    CreateFilename _createfilename;
    _createfilename.createFilename();

    QString filename = _WallpaperDir+"/"+_createfilename.filename_new2;

    QPixmap photo_wallpaper;
    photo_wallpaper.loadFromData(downloadedPhotoData());
    photo_wallpaper.save(filename);
}
