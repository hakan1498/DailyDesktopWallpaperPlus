#include "photobrowser.h"
#include "ui_photobrowser.h"
#include "itemdelegate.h"
#include "manage_database.h"

#include <QSize>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QPixmap>
#include <QImage>
#include <QIcon>
#include <QStandardItem>
#include <QObject>
#include <QtConcurrent/QtConcurrent>
#include <QThreadPool>
#include <QCloseEvent>
#include <QFileInfoList>
#include <QSettings>
#include <QLabel>
#include <QDesktopServices>

PhotoBrowser::PhotoBrowser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PhotoBrowser)
{
    ui->setupUi(this);

    _get_specific_values=false;
    _get_values();
    _read_settings();
    _setPictureRes();

    //Init image list
    ui->listView->setResizeMode(QListView::Adjust);
    ui->listView->setFlow(QListView::LeftToRight);
    ui->listView->setSpacing(15);
    ui->listView->setWrapping(true);
    ui->listView->setUniformItemSizes(true);
    ui->listView->setIconSize(QSize (_scaled_picture_width, _scaled_picture_height));
    ui->listView->setViewMode(QListView::IconMode);
    ui->listView->setMovement(QListView::Static);
    ui->listView->setItemDelegate(new ItemDelegate(this));

    int _real_size = _thumbfilelist.size()-1;
    QStandardItemModel *model = new QStandardItemModel(_real_size, 1, this);
    manage_database ManageDatabase;
    ManageDatabase.init_database();
    for(int i=0;i<_real_size; i++){
        _thumbfilename = _thumbfilelist.at(i).toUtf8();
        QPixmap thumb;
        thumb.load(_thumbfiledir+"/"+_thumbfilename);
        thumb.scaled(_scaled_picture_width, _scaled_picture_height, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        QIcon icon = thumb;
        QStandardItem *item = new QStandardItem(icon, _thumbfilelist.at(i).toUtf8());
        model->setItem(i, item);
    }
    ui->listView->setModel(model);
    // ------ End of init

    wallchanged = false;

    _label_Details = new QLabel(this);
    _label_headline = new QLabel(this);
    _label_headline1 = new QLabel(this);
    _label_description_and_copyright1 = new QLabel(this);
    _label_description_and_copyright = new QLabel(this);
    _preview_text = new QLabel(this);
    _previewImageLabel = new QLabel(this);
    _getmoreInformations = new QPushButton(this);
    _getmoreInformations->setText("Get more Informations");
    _getmoreInformations->setVisible(false);
    connect(_getmoreInformations, &QPushButton::released, this, &PhotoBrowser::_getmoreInformationsButton_clicked);
    _label_image_size1 = new QLabel(this);
    _label_image_size = new QLabel(this);

    _layout_details = new QVBoxLayout();
    ui->frame_2->setLayout(_layout_details);

    _layout_details->addWidget(_label_Details);
    _layout_details->addSpacerItem(new QSpacerItem(0,20, QSizePolicy::Minimum,QSizePolicy::Minimum));

    _layout_details->addWidget(_label_headline);
    _layout_details->addWidget(_label_headline1);
    _layout_details->addSpacerItem(new QSpacerItem(0,20, QSizePolicy::Minimum,QSizePolicy::Minimum));

    _layout_details->addWidget(_label_description_and_copyright1);
    _layout_details->addWidget(_label_description_and_copyright);
    _layout_details->addSpacerItem(new QSpacerItem(0,20, QSizePolicy::Minimum,QSizePolicy::Minimum));

    _layout_details->addWidget(_label_image_size1);
    _layout_details->addWidget(_label_image_size);
    _layout_details->addSpacerItem(new QSpacerItem(0,20, QSizePolicy::Minimum,QSizePolicy::Minimum));

    _layout_details->addWidget(_preview_text);
    _layout_details->addWidget(_previewImageLabel);
    _layout_details->setAlignment(_previewImageLabel, Qt::AlignHCenter);
    _layout_details->addSpacerItem(new QSpacerItem(0,20, QSizePolicy::Minimum,QSizePolicy::Minimum));
    _layout_details->addWidget(_getmoreInformations);
    _layout_details->setAlignment(_getmoreInformations, Qt::AlignHCenter);
    _layout_details->addStretch();

    _first_run=true;
    _display_details();
}

void PhotoBrowser::closeEvent(QCloseEvent * event)
{
    // stop close event and hide window;
    event->ignore();
    this->hide();
}

void PhotoBrowser::reject() {
    this->hide();
}

PhotoBrowser::~PhotoBrowser()
{
    delete ui;
}

void PhotoBrowser::_setPictureRes() {
    if(_resolution=="1024x768") {
        _scaled_picture_width = (1024/100)*15;
        _scaled_picture_height = (768/100)*15;
    } else if(_resolution=="1280x720") {
        _scaled_picture_width = (1280/100)*15;
        _scaled_picture_height = (720/100)*15;
    } else if(_resolution=="1366x768") {
        _scaled_picture_width = (1366/100)*15;
        _scaled_picture_height = (768/100)*15;
    } else if(_resolution=="1920x1080") {
        _scaled_picture_width = (1920/100)*9;
        _scaled_picture_height = (1080/100)*9;
    } else if(_resolution=="1920x1200") {
        _scaled_picture_width = (1920/100)*9;
        _scaled_picture_height = (1200/100)*9;
    } else if(_resolution=="UHD") {
        _scaled_picture_width = 172;
        _scaled_picture_height = 97;
    };
}

void PhotoBrowser::_read_settings()
{
    QString _iniFilePath = QDir::homePath()+"/.DailyDesktopWallpaperPlus/settings.ini";

    // read settings from INI file
    QSettings settings(_iniFilePath, QSettings::IniFormat);

    settings.beginGroup("BING-SETTINGS");
    _resolution = settings.value("resolution","").toString();
    settings.endGroup();

    settings.beginGroup("SETTINGS");
    _WallpaperDir = settings.value("WallpaperDir","").toString();
    _OldWallpaperDir = settings.value("OldWallpaperDir","").toString();
    _thumbfiledir = settings.value("ThumbFileDir","").toString();
    settings.endGroup();
}

void PhotoBrowser::on_pushButton_clicked()
{
    _get_specific_values=true;
    wallchanged = true;
    setWallpaperFile();
    _get_values();
}

void PhotoBrowser::removeWallpaperFile()
{
    QDir wallDir(_WallpaperDir);
    QFileInfoList WallpaperList = wallDir.entryInfoList(QStringList() << "*.jpg", QDir::Files);
    int totalfiles = WallpaperList.size();
    int minFile = 1;
    if (!(minFile > totalfiles))
    {
        /* if in the selected wallpaper directory are different photo files,
         * that are not wallpaperfiles, then filter it
         * the filename of the wallpaperfiles of DailyDesktopWallpaperPlus
         * contains in the filename "background". */

        for (int i = 0; i < totalfiles; i++) {
            QString _picturefile = WallpaperList[0].baseName()+".jpg";
            if(_picturefile.contains("-background.jpg"))
            {
                _wallpaperfile = WallpaperList[0].baseName()+".jpg";
                QFile wallfile(_WallpaperDir+"/"+_wallpaperfile);
                wallfile.remove();
            }
        }
    }
    if (minFile > totalfiles)
    {
        // Set a content in the qstring to avoid a crash
        _wallpaperfile = "NULL";
    }
}

void PhotoBrowser::setWallpaperFile()
{
    QModelIndex index = ui->listView->currentIndex();
    manage_database ManageDatabase;
    ManageDatabase._thumb_filename = index.data(Qt::DisplayRole).toString();
    ManageDatabase.get_wallpaperfilename();
    _setwall._wallpaperfilename = _OldWallpaperDir+"/"+ManageDatabase._wallpaperfilename;
    _setwall._set_wallpaper();
    _photobrowser_specific_wallpaperfilename = ManageDatabase._wallpaperfilename;
}

void PhotoBrowser::_get_values()
{
    manage_database ManageDatabase;
    ManageDatabase.init_database();
    if(ManageDatabase._initDB_failed==false)
    {
        if(_get_specific_values==false)
        {
            ManageDatabase.create_full_thumbfilelist();
            _thumbfilelist = ManageDatabase._full_thumbfilelist;
        } else
        {
            QModelIndex index = ui->listView->currentIndex();
            ManageDatabase._thumb_filename = index.data(Qt::DisplayRole).toString();
            ManageDatabase.get_specific_values();
            _pb_copyright_description_photo = ManageDatabase._photobrowser_specific_desc;
            _pb_headline = ManageDatabase._photobrowser_specific_headline;
            _pb_copyright_link = ManageDatabase._photobrowser_specific_browser_url;
            _size_width = ManageDatabase._out_width;
            _size_height = ManageDatabase._out_height;
            _wallpaperfile = ManageDatabase._wallpaperfilename;
            _get_specific_values=false;
        }
    }
    QThread::msleep(100);
    if (_wallpaperfile.isEmpty())
    {
        // Set a content in the qstring to avoid a crash
        _wallpaperfile = "NULL";
    }
}

void PhotoBrowser::on_listView_clicked(const QModelIndex &index)
{
    _display_details();
}

void PhotoBrowser::_display_details()
{
    _label_Details->setText("Details");
    _label_Details->setStyleSheet("font: 14pt; font-weight: bold;");

    _label_headline->setStyleSheet("font-style: italic;");
    _label_headline->setText("Please select a picture.");

    if(_first_run==false)
    {
        _get_specific_values=true;
        _get_values();

        _label_headline->setStyleSheet("font-weight: bold;");
        _label_headline->setText("Title");

        _label_headline1->setText(_pb_headline);
        _label_headline1->setWordWrap(true);
        _label_headline1->setMaximumWidth(275);

        _label_description_and_copyright1->setStyleSheet("font-weight: bold;");
        _label_description_and_copyright1->setText("Description and Copyright: ");

        _label_description_and_copyright->setText(_pb_copyright_description_photo);
        _label_description_and_copyright->setWordWrap(true);
        _label_description_and_copyright->setMaximumWidth(275);

        _label_image_size1->setText("Picture size:");
        _label_image_size1->setStyleSheet("font-weight: bold;");
        _label_image_size->setText(QString::number(_size_width)+" x "+QString::number(_size_height));

        _preview_text->setText("Preview:");
        _preview_text->setStyleSheet("font-weight: bold;");

        QImage _loadPrevImage;
        QImage _displayImage;
        _loadPrevImage.load(_OldWallpaperDir+"/"+_wallpaperfile);
        _displayImage = _loadPrevImage.scaled(240,150, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
        _previewImageLabel->setPixmap(QPixmap::fromImage(_displayImage));

        QPixmap _gotoBing(":icons/Info.png");
        _getmoreInformations->setIcon(_gotoBing);
        _getmoreInformations->setVisible(true);
        _getmoreInformations->setMinimumWidth(240);
    }
    _first_run=false;
}

void PhotoBrowser::_getmoreInformationsButton_clicked()
{
    QDesktopServices::openUrl(_pb_copyright_link);
}
