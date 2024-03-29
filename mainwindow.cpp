#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "photobrowser.h"
#include "manage_database.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QFileInfoList>
#include <QSettings>
#include <QTextStream>
#include <QProcess>
#include <QPixmap>
#include <QActionGroup>
#include <QApplication>
#include <QCursor>
#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include <QWidgetAction>
#include <QUrl>
#include <QDesktopServices>
#include <QtGlobal>
#include <QDate>
#include <QThread>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    qApp->setAttribute(Qt::AA_DontShowIconsInMenus, false);

    _photobrowser_specific_values=false;
    _set_reset=false;

    QString _usedDesktop = qgetenv("XDG_SESSION_DESKTOP");
    _IsUnity=false;
    if(_usedDesktop=="ubuntu") {
        _IsUnity=true;
    }

    _iniFilePath = QDir::homePath()+"/.DailyDesktopWallpaperPlus/settings.ini";
    mSystemTrayIcon = new QSystemTrayIcon(this);

    set_values();
    QThread::msleep(100);
    check_dir();
    load_wallpaper();
    init_MainContextMenu();
    init_SystemTrayIcon();
    checkFiles();

    if(_SaveOldWallpaper==true) {
        if(_delete_automatically==true) {
            manage_wallpapers();
        }
    }
    connect(mSystemTrayIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(slotActive(QSystemTrayIcon::ActivationReason)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotActive(QSystemTrayIcon::ActivationReason r)
{
    if (r == QSystemTrayIcon::Trigger) {
            menu->exec(QCursor::pos());
    }
}

void MainWindow::set_values()
{
    QString _iniFileDir = QDir::homePath()+"/.DailyDesktopWallpaperPlus";

    QDir settings_dir(_iniFileDir);
    if (!settings_dir.exists())
    {
        settings_dir.mkpath(_iniFileDir);
    }

    // If settings file does not exist then create it with default settings
    QFile iniFile(_iniFilePath);
    if(!iniFile.exists(_iniFilePath))
    {
        QString iniDefaultData = "[BING-SETTINGS]\n"
                "binglocation=en-US\n"
                "country=USA\n"
                "resolution=1920x1080\n"
                "\n"
                "[SETTINGS]\n"
                "WallpaperDir="+QDir::homePath()+"/.DailyDesktopWallpaperPlus/background_wallpaper\n"
                "OldWallpaperDir="+QDir::homePath()+"/.DailyDesktopWallpaperPlus/old_Wallpapers\n"
                "ThumbFileDir="+QDir::homePath()+"/.DailyDesktopWallpaperPlus/thumb_pictures\n"
                "Autostart=true\n"
                "SaveOldWallpaper=true\n"
                "Provider=Bing\n"
                "current_photo_dl_url=\n"
                "create_menu_item=true\n"
                "delete_automatically=true\n"
                "delete_older_than=15\n"
                "\n"
                "[SETWALLPAPER]\n"
                "AutoChange=true\n"
                "Parameter=0\n"
                "Auto_timebased_change=true\n"
                "time_hours=18\n"
                "time_minutes=0\n"
                "\n"
                "[VERSION]\n"
                "Version="+_appVersion+"\n";

        if (iniFile.open(QIODevice::Append))
        {
            QTextStream stream(&iniFile);
            stream << iniDefaultData << Qt::endl;
        }
    }

    // read settings from INI file
    QSettings settings(_iniFilePath, QSettings::IniFormat);

    settings.beginGroup("BING-SETTINGS");
    _resolution = settings.value("resolution","").toString();
    _binglocation = settings.value("binglocation","").toString();
    _country = settings.value("country","").toString();
    settings.endGroup();

    settings.beginGroup("SETTINGS");
    _Autostart = settings.value("Autostart","").toBool();
    _WallpaperDir = settings.value("WallpaperDir","").toString();
    _OldWallpaperDir = settings.value("OldWallpaperDir","").toString();
    _thumbfiledir = settings.value("ThumbFileDir","").toString();
    _SaveOldWallpaper = settings.value("SaveOldWallpaper","").toBool();
    _create_menu_item = settings.value("create_menu_item","").toBool();
    _delete_automatically = settings.value("delete_automatically","").toBool();
    _delete_older_than = settings.value("delete_older_than","").toInt();
    settings.endGroup();

    settings.beginGroup("SETWALLPAPER");
    _AutoChange = settings.value("AutoChange","").toBool();
    _Parameter = settings.value("Parameter","").toInt();
    _time_hours = settings.value("time_hours","").toInt();
    _time_minutes = settings.value("time_minutes","").toInt();
    settings.endGroup();

    settings.beginGroup("SETTINGS");
    _Provider = settings.value("Provider","").toString();
    settings.endGroup();

    _appVersion = "3.5";
    _write_AppVersion();

    if (_Autostart == true)
    {
        set_autostart();
    }
    else
    {
        no_autostart();
    }

    if (_create_menu_item == true)
    {
        set_menu_item();
    }
    else
    {
        no_menu_item();
    }

    if (_AutoChange == true)
    {
        set_autoChange();
    }
    else
    {
        no_autoChange();
    }
}

void MainWindow::check_dir()
{
    QDir _wallDir(_WallpaperDir);
    QDir _oldwallDir(_OldWallpaperDir);
    if(!_wallDir.exists()) {
        _wallDir.mkpath(_WallpaperDir);
    }
    if(_SaveOldWallpaper == true) {
        if(!_oldwallDir.exists()) {
            _oldwallDir.mkpath(_OldWallpaperDir);
        }
    }
}

void MainWindow::load_wallpaper()
{
    // If we got here from a timer, make sure to reset the timer for the next scheduled time...
    if (_Autostart && _autoChangeTimer->isActive())
    {
        no_autoChange();
        set_autoChange();
    }
    if(_Provider =="Bing") {
        _setBingWallpaper();
    }
    if(_Provider =="WindowsSpotlight") {
        _setWinSpotWallpaper();
    }
}

void MainWindow::set_autoChange()
{
    if (_AutoChange && _autoChangeTimer == NULL)
    {
        QTime scheduledTime(_time_hours, _time_minutes);

        // Default of midnight if time is invalid for some reason...
        if (!scheduledTime.isValid())
        {
            scheduledTime.setHMS(23,59,59);
        }

        int milliSecondsToGo = QTime::currentTime().msecsTo(scheduledTime);

        // If time is in the past, schedule it for tomorrow...
        if (milliSecondsToGo < 0)
        {
            // 0,0 is the start of the day whereas 23,59 + 1 min is tonight at midnight...
            int msUntilMidnight = QTime::currentTime().msecsTo(QTime(23,59)) + 60000;
            milliSecondsToGo = msUntilMidnight + (QTime(0,0).msecsTo(scheduledTime));
        }

        // Some padding for good measure to avoid firing more than once at the scheduled time
        milliSecondsToGo += 3000;

        _autoChangeTimer = new QTimer(this);
        connect(_autoChangeTimer, &QTimer::timeout, this, &MainWindow::load_wallpaper);
        _autoChangeTimer->start(milliSecondsToGo);
    }
}

void MainWindow::no_autoChange()
{
    if (_autoChangeTimer != NULL)
    {
        _autoChangeTimer->stop();
        disconnect(_autoChangeTimer, &QTimer::timeout, this, &MainWindow::load_wallpaper);
        delete _autoChangeTimer;
        _autoChangeTimer = NULL;
    }
}

void MainWindow::no_autostart()
{
    autostart_and_menuitem.no_autostart();
}

void MainWindow::set_autostart()
{
    autostart_and_menuitem.set_autostart();
}

void MainWindow::no_menu_item()
{
    autostart_and_menuitem.no_menuitem();
}

void MainWindow::set_menu_item()
{
    autostart_and_menuitem.set_menuitem();
}

void MainWindow::load_bgp_specific_settings()
{
    // read background-photo specific settings from Database

    manage_database ManageDatabase;
    ManageDatabase.init_database();
    if(ManageDatabase._initDB_failed==false)
    {
        if(_photobrowser_specific_values==false)
        {
            ManageDatabase.get_last_record();
            _copyright_description_photo = ManageDatabase._last_rec_description_and_copyright;
            _headline = ManageDatabase._last_rec_headline;
            _setwall._wallpaperfilename = ManageDatabase._last_rec_filename;
            _wallpaperfile = ManageDatabase._last_rec_filename;
            _copyright_link = ManageDatabase._last_browser_url;
        }
    }
    QThread::msleep(100);
    if (_wallpaperfile.isEmpty())
    {
        // Set a content in the qstring to avoid a crash
        _wallpaperfile = "NULL";
    }
}

void MainWindow::init_MainContextMenu()
{
    load_bgp_specific_settings();

    menu = new QMenu(this);

    /* If you NOT use Unity(Ubuntu); See QTBUG-26840: https://bugreports.qt.io/browse/QTBUG-26840
     * Init Widgets to show title, thumbnail of the Wallpaper and
     * description in the context menu */

    if(_IsUnity==false) { 
        if(!(_wallpaperfile=="NULL" or _wallpaperfile==""))
        {
            QWidget* _descWidget = new QWidget();
            QVBoxLayout* dL = new QVBoxLayout();
            QWidgetAction * _widgetaction = new QWidgetAction(menu);
            QLabel * _imageLabel = new QLabel();
            QLabel * _labelTitle = new QLabel();
            QLabel * _labelBingLocation = new QLabel("Bing Location: "+_country);
            QLabel * _labelDescription = new QLabel(_copyright_description_photo);
            _imageLabel->setAlignment(Qt::AlignCenter);
            _labelTitle->setAlignment(Qt::AlignCenter);
            _labelBingLocation->setAlignment(Qt::AlignCenter);
            _labelDescription->setAlignment(Qt::AlignCenter);

            if(_Provider =="Bing" && _photobrowser_specific_values==false) {
                if(!_headline.isEmpty()) {
                    _labelTitle->setText(_headline);
                } else {
                    _labelTitle->setText("Bing's photo of the day");
                }
            }

            if(_Provider =="WindowsSpotlight" && _photobrowser_specific_values==false) {
                if(!_headline.isEmpty()) {
                    _labelTitle->setText(_headline);
                } else {
                    _labelTitle->setText("Photo of Windows Spotlight");
                }
                _labelBingLocation->hide();
            }

            if(_photobrowser_specific_values==true) {
                _labelTitle->setText(_headline);
                _labelBingLocation->hide();
                _loadImage.load(_OldWallpaperDir+"/"+_wallpaperfile);
            } else {
                _loadImage.load(_WallpaperDir+"/"+_wallpaperfile);
            }

            QThread::msleep(300);

            _descImage = _loadImage.scaled(280,150, Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
            _imageLabel->setPixmap(QPixmap::fromImage(_descImage));

            _labelBingLocation->setWordWrap(true);
            _labelBingLocation->setStyleSheet("font: 8pt");

            _labelDescription->setWordWrap(true);
            _labelDescription->setStyleSheet("font: 8pt; font-style: italic; text-align:center;");

            _labelTitle->setStyleSheet("font: 8pt; font-weight: bold; text-align:center;");

            dL->addWidget(_labelTitle);
            dL->addWidget(_imageLabel);
            dL->addWidget(_labelBingLocation);
            dL->addWidget(_labelDescription);
            _descWidget->setLayout(dL);
            _descWidget->show();
            _widgetaction->setDefaultWidget(_descWidget);

            menu->addAction(_widgetaction);
            menu->addSeparator();
        }
    }

    //Init provider specific context menu items
    if (_Provider == "Bing")
    {
        QPixmap refresh(":icons/Download.png");
        QPixmap _loc(":/icons/Computer.png");
        QPixmap _display(":/icons/monitor.png");

        QPixmap _gotoBing(":icons/Info.png");
        QAction * gotoBing_bing = menu->addAction(_gotoBing, "Learn more about this Picture");
        QAction * bingRefresh = menu->addAction(refresh, "Refresh Wallpaper");
        menu->addSeparator();
        QMenu * bingLoc = menu->addMenu(_loc, "Bing Location");
        QMenu * bingRes = menu->addMenu(_display, "Resolution of the Wallpaper");
        menu->addSeparator();

        connect(gotoBing_bing, SIGNAL(triggered()), this, SLOT(_gotoBing_bing_click()));
        connect(bingRefresh, SIGNAL(triggered()), this, SLOT(_menu_bingRefresh_click()));

        //create submenus to select bing locations
        QPixmap usa(":countries/us.png");
        QPixmap japan(":countries/japan.png");
        QPixmap china(":countries/china.png");
        QPixmap australia(":countries/australia.png");
        QPixmap gb(":countries/gb.png");
        QPixmap germany(":countries/germany.png");
        QPixmap canada(":countries/canada.png");
        QPixmap nz(":countries/nz.png");
        QPixmap brazil(":countries/brazil.png");
        QPixmap france(":countries/france.png");

        QAction * _usa_ = bingLoc->addAction(usa, "USA");
        QAction * _japan_ = bingLoc->addAction(japan, "Japan");
        QAction * _china_ = bingLoc->addAction(china, "China");
        QAction * _australia_ = bingLoc->addAction(australia, "Australia");
        QAction * _gb_ = bingLoc->addAction(gb, "Great Britain");
        QAction * _germany_ = bingLoc->addAction(germany, "Germany");
        QAction * _canada_ = bingLoc->addAction(canada, "Canada");
        QAction * _nz_ = bingLoc->addAction(nz, "New Zealand");
        QAction * _brazil_ = bingLoc->addAction(brazil, "Brazil");
        QAction * _france_ = bingLoc->addAction(france, "France");

        connect(_usa_, SIGNAL(triggered()), this, SLOT(_menu_usa_click()));
        connect(_japan_, SIGNAL(triggered()), this, SLOT(_menu_japan_click()));
        connect(_china_, SIGNAL(triggered()), this, SLOT(_menu_china_click()));
        connect(_australia_, SIGNAL(triggered()), this, SLOT(_menu_australia_click()));
        connect(_gb_, SIGNAL(triggered()), this, SLOT(_menu_gb_click()));
        connect(_germany_, SIGNAL(triggered()), this, SLOT(_menu_germany_click()));
        connect(_canada_, SIGNAL(triggered()), this, SLOT(_menu_canada_click()));
        connect(_nz_, SIGNAL(triggered()), this, SLOT(_menu_nz_click()));
        connect(_brazil_, SIGNAL(triggered()), this, SLOT(_menu_brazil_click()));
        connect(_france_, SIGNAL(triggered()), this, SLOT(_menu_france_click()));

        // create submenus to select the resolution of the photo
        QAction * _UHD_ = bingRes->addAction("higher than 1920x1200");
        QAction * _1920x1200_ = bingRes->addAction("1920x1200");
        QAction * _1920x1080_ = bingRes->addAction("1920x1080");
        QAction * _1366x768_ = bingRes->addAction("1366x768");
        QAction * _1280x720_ = bingRes->addAction("1280x720");
        QAction * _1024x768_ = bingRes->addAction("1024x768");

        QActionGroup* _bingphoto_resolution_group = new QActionGroup(this);

        _UHD_->setCheckable(true);
        _1920x1200_->setCheckable(true);
        _1920x1080_->setCheckable(true);
        _1366x768_->setCheckable(true);
        _1280x720_->setCheckable(true);
        _1024x768_->setCheckable(true);

        _UHD_->setActionGroup(_bingphoto_resolution_group);
        _1920x1200_->setActionGroup(_bingphoto_resolution_group);
        _1920x1080_->setActionGroup(_bingphoto_resolution_group);
        _1366x768_->setActionGroup(_bingphoto_resolution_group);
        _1280x720_->setActionGroup(_bingphoto_resolution_group);
        _1024x768_->setActionGroup(_bingphoto_resolution_group);

        connect(_UHD_, SIGNAL(triggered()), this, SLOT(_menu_bingRes_UHD_click()));
        connect(_1920x1200_, SIGNAL(triggered()), this, SLOT(_menu_bingRes_1920x1200_click()));
        connect(_1920x1080_, SIGNAL(triggered()), this, SLOT(_menu_bingRes_1920x1080_click()));
        connect(_1366x768_, SIGNAL(triggered()), this, SLOT(_menu_bingRes_1366x768_click()));
        connect(_1280x720_, SIGNAL(triggered()), this, SLOT(_menu_bingRes_1280x720_click()));
        connect(_1024x768_, SIGNAL(triggered()), this, SLOT(_menu_bingRes_1024x768_click()));

        // set resolution-item
        if(_resolution=="UHD") {
            _UHD_->setChecked(true);
        } else if(_resolution=="1920x1200") {
            _1920x1200_->setChecked(true);
        } else if(_resolution=="1920x1080") {
            _1920x1080_->setChecked(true);
        } else if(_resolution=="1366x768") {
            _1366x768_->setChecked(true);
        } else if(_resolution=="1280x720") {
            _1280x720_->setChecked(true);
        } else if(_resolution=="1024x768") {
            _1024x768_->setChecked(true);
        }
    }

    if (_Provider == "WindowsSpotlight")
    {
        QPixmap _gotoBing(":icons/Info.png");
        QAction * gotoBing = menu->addAction(_gotoBing, "Learn more about this Picture");

        QPixmap refresh(":icons/Download.png");
        QAction * wspotRef = menu->addAction(refresh, "Refresh Wallpaper");

        menu->addSeparator();

        connect(gotoBing, SIGNAL(triggered()), this, SLOT(_gotoBing_click()));
        connect(wspotRef, SIGNAL(triggered()), this, SLOT(_wspotRef_click()));
    }

    QPixmap _providers(":/icons/Earth.png");
    QPixmap _loadexistwallpaper(":/icons/Upload.png");
    QPixmap _wrench(":/icons/Wrench.png");
    QPixmap _about(":/icons/About.png");
    QPixmap _exit(":/icons/Exit.png");

    QMenu * providers = menu->addMenu(_providers, "Providers");
    QAction * ShowPhotoBrowser = menu->addAction(_loadexistwallpaper, "Load existing Wallpaper");
    menu->addSeparator();
    QAction * settings = menu->addAction(_wrench, "Settings");
    QAction * about = menu->addAction(_about, "About");
    menu->addSeparator();
    QAction * exitApp = menu->addAction(_exit, "Exit");

    connect(ShowPhotoBrowser, SIGNAL(triggered()), this, SLOT(_show_photobrowser_click()));
    connect(settings, SIGNAL(triggered()), this, SLOT(_menu_settings_click()));
    connect(about, SIGNAL(triggered()), this, SLOT(_menu_about_click()));
    connect(exitApp, SIGNAL(triggered()), this, SLOT(close()));

    // Create submenus of providers
    // Is private
    _winspot_wall_option = providers->addAction("Photo of Windows Spotlight");
    _bing_wall_option = providers->addAction("Bing's picture of the day");

    QActionGroup* _providers_group = new QActionGroup(this);

    _winspot_wall_option->setCheckable(true);
    _bing_wall_option->setCheckable(true);

    _winspot_wall_option->setActionGroup(_providers_group);
    _bing_wall_option->setActionGroup(_providers_group);

    connect(_winspot_wall_option, SIGNAL(triggered()), this, SLOT(_menu_winspot_wall_option_click()));
    connect(_bing_wall_option, SIGNAL(triggered()), this, SLOT(_menu_bing_wall_option_click()));

    if(_SaveOldWallpaper == false) {
        ShowPhotoBrowser->setEnabled(false);
    } else {
        ShowPhotoBrowser->setEnabled(true);
    }

    // set item
    if(_Provider=="Bing") {
        _bing_wall_option->setChecked(true);
    } else {
        _winspot_wall_option->setChecked(true);
    } 
}

void MainWindow::init_SystemTrayIcon()
{
    mSystemTrayIcon->setIcon(QIcon(":/128.png"));
    if(_IsUnity==true){
        mSystemTrayIcon->setContextMenu(menu);
    }
    mSystemTrayIcon->show();
    mSystemTrayIcon->setVisible(true);
}

void MainWindow::_show_photobrowser_click()
{
    manage_wallpapers();
    QThread::msleep(100);
    PhotoBrowser _photobrowser;
    _photobrowser.setModal(true);
    _photobrowser.exec();
    mSystemTrayIcon->setToolTip("");
    if (_photobrowser.wallchanged==true) {
       _photobrowser_specific_values=true;
       _copyright_description_photo = _photobrowser._pb_copyright_description_photo;
       _headline = _photobrowser._pb_headline;
       _copyright_link = _photobrowser._pb_copyright_link;
       _wallpaperfile = _photobrowser._photobrowser_specific_wallpaperfilename;

       _photobrowser.wallchanged=false;
       updateContextMenu();
    }
}

void MainWindow::_menu_settings_click()
{
    SettingsWindow _sett_win;
    _sett_win.setFixedSize(_sett_win.size());
    _sett_win.setModal(true);
    _sett_win.exec();
    set_values();
    updateContextMenu();
    if(_sett_win._set_reset==true)
    {
        _reset();
    }
}

void MainWindow::_menu_about_click()
{
    About _about_win;
    _about_win.setFixedSize(_about_win.size());
    _about_win.setModal(true);
    _about_win.exec();
}

void MainWindow::_menu_winspot_wall_option_click()
{
    _Provider = "WindowsSpotlight";
    _write_provider_settings();
    menu->clear();
    updateContextMenu();
    _setWinSpotWallpaper();
    _bing_wall_option->setChecked(false);
}

void MainWindow::_menu_bing_wall_option_click()
{
    _Provider = "Bing";
    _write_provider_settings();
    menu->clear();
    updateContextMenu();
    _setBingWallpaper();
    _winspot_wall_option->setChecked(false);
}

void MainWindow::_gotoBing_bing_click()
{
    QDesktopServices::openUrl(_copyright_link);
}

void MainWindow::_menu_bingRefresh_click()
{
    _setBingWallpaper();
}

void MainWindow::_wspotRef_click()
{
    _setWinSpotWallpaper();
}

void MainWindow::_gotoBing_click()
{
    QDesktopServices::openUrl(_copyright_link);
}

void MainWindow::_menu_usa_click()
{
    _selected_binglocation = "en-US";
    _country="USA";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_japan_click()
{
    _selected_binglocation = "ja-JP";
    _country="Japan";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_china_click()
{
    _selected_binglocation = "zh-CN";
    _country="China";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_australia_click()
{
    _selected_binglocation = "en-AU";
    _country="Australia";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_gb_click()
{
    _selected_binglocation = "en-GB";
    _country="Great Britain";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_germany_click()
{
    _selected_binglocation = "de-DE";
    _country="Germany";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_canada_click()
{
    _selected_binglocation = "en-CA";
    _country="Canada";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_nz_click()
{
    _selected_binglocation = "en-NZ";
    _country="New Zealand";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_brazil_click()
{
    _selected_binglocation = "pt-BR";
    _country="Brazil";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_france_click()
{
    _selected_binglocation = "fr-FR";
    _country="France";
    _write_binglocation_settings();
    _setBingWallpaper();
}

void MainWindow::_menu_bingRes_UHD_click()
{
    _selected_bing_photo_resolution = "UHD";
    _resolution="UHD";
    _write_bing_photo_resolution_settings();
    _setBingWallpaper();
    updateContextMenu();
}

void MainWindow::_menu_bingRes_1920x1200_click()
{
    _selected_bing_photo_resolution = "1920x1200";
    _resolution="1920x1200";
    _write_bing_photo_resolution_settings();
    _setBingWallpaper();
    updateContextMenu();
}

void MainWindow::_menu_bingRes_1920x1080_click()
{
    _selected_bing_photo_resolution = "1920x1080";
    _resolution="1920x1080";
    _write_bing_photo_resolution_settings();
    _setBingWallpaper();
    updateContextMenu();
}

void MainWindow::_menu_bingRes_1366x768_click()
{
    _selected_bing_photo_resolution = "1366x768";
    _resolution="1366x768";
    _write_bing_photo_resolution_settings();
    _setBingWallpaper();
    updateContextMenu();
}

void MainWindow::_menu_bingRes_1280x720_click()
{
    _selected_bing_photo_resolution = "1280x720";
    _resolution="1280x720";
    _write_bing_photo_resolution_settings();
    _setBingWallpaper();
    updateContextMenu();
}

void MainWindow::_menu_bingRes_1024x768_click()
{
    _selected_bing_photo_resolution = "1024x768";
    _resolution="1024x768";
    _write_bing_photo_resolution_settings();
    _setBingWallpaper();
    updateContextMenu();
}

void MainWindow::_write_provider_settings()
{
    QSettings _select_provider_settings(_iniFilePath, QSettings::IniFormat);

    _select_provider_settings.beginGroup("SETTINGS");
    _select_provider_settings.setValue("Provider", _Provider);
    _select_provider_settings.endGroup();
    _select_provider_settings.sync();
}

void MainWindow::_write_binglocation_settings()
{
    QSettings _bingLocation_settings(_iniFilePath, QSettings::IniFormat);

    _bingLocation_settings.beginGroup("BING-SETTINGS");
    _bingLocation_settings.setValue("binglocation", _selected_binglocation);
    _bingLocation_settings.setValue("country", _country);
    _bingLocation_settings.endGroup();
    _bingLocation_settings.sync();
}

void MainWindow::_write_bing_photo_resolution_settings()
{
    QSettings _bing_photo_resolution_settings(_iniFilePath, QSettings::IniFormat);

    _bing_photo_resolution_settings.beginGroup("BING-SETTINGS");
    _bing_photo_resolution_settings.setValue("resolution", _selected_bing_photo_resolution);
    _bing_photo_resolution_settings.endGroup();
    _bing_photo_resolution_settings.sync();
}

void MainWindow::_write_AppVersion()
{
    QSettings _version_settings(_iniFilePath, QSettings::IniFormat);

    _version_settings.beginGroup("VERSION");
    _version_settings.setValue("Version", _appVersion);
    _version_settings.endGroup();
    _version_settings.sync();
}

void MainWindow::_check_internet_connection()
{
    //check internet connection at first
    QString _check_url = "http://www.bing.com";
    QEventLoop loop;
    QObject::connect(&dl_manager,&QNetworkAccessManager::finished,&loop,&QEventLoop::quit);
    reply = dl_manager.get(QNetworkRequest(_check_url));
    loop.exec();
    if (reply->bytesAvailable())
    {
        _keeporremove_old_wallpaper();
        if(_Provider =="Bing") {
            getbingwallpaper.get_bing_wallpaper();
        }
        if(_Provider =="WindowsSpotlight") {
            getwinspotwallpaper.get_wallpaper();
        }
    }
    else
    {
        qDebug() << "ERROR: No connection to the internet.";
        _show_tooltip();
    }
    reply->close();
    loop.exit();
    load_bgp_specific_settings();
    set_tooltip_string();
}

void MainWindow::_keeporremove_old_wallpaper()
{
    load_bgp_specific_settings();
    if (_SaveOldWallpaper == true)
    {
        if(!(_wallpaperfile == "NULL" or _wallpaperfile == "")) {
            QFile::copy(_WallpaperDir+"/"+_wallpaperfile, _OldWallpaperDir+"/"+_wallpaperfile);
            QFile old_bgfile(_WallpaperDir+"/"+_wallpaperfile);
            old_bgfile.remove(_WallpaperDir+"/"+_wallpaperfile);
        }
    }
    else
    {
        QFile background_file(_WallpaperDir+"/"+_wallpaperfile);
        background_file.remove();
    }
}

void MainWindow::set_tooltip_string()
{
    if(_Provider == "Bing")
    {
        _tooltip_title = _headline;
        _tooltip_message = _copyright_description_photo;
    } else
    {
        _tooltip_title = _headline;
        _tooltip_message = _copyright_description_photo;
    }
}

void MainWindow::_show_tooltip()
{
    QSystemTrayIcon::MessageIcon icon = QSystemTrayIcon::MessageIcon(QSystemTrayIcon::Information);
    mSystemTrayIcon->showMessage(_tooltip_title, _tooltip_message, icon, 5000);
    mSystemTrayIcon->setToolTip("Description: \n"+_tooltip_message);
}

void MainWindow::_setBingWallpaper()
{
    _photobrowser_specific_values=false;
    _check_internet_connection();
    _show_tooltip();
    _setwall._wallpaperfilename = _WallpaperDir+"/"+_wallpaperfile;
    _setwall._set_wallpaper();
    updateContextMenu();
}

void MainWindow::_setWinSpotWallpaper()
{
    _photobrowser_specific_values=false;
    _check_internet_connection();
    _show_tooltip();
    _setwall._wallpaperfilename = _WallpaperDir+"/"+_wallpaperfile;
    _setwall._set_wallpaper();
    updateContextMenu();
}

void MainWindow::checkFiles()
{
    // check if more than one picturefile in the directory
    // If more than one file in the directory, then remove it (e.g. after a crash to remove corrupt files)
    QDir wallDir(_WallpaperDir);
    QStringList _wallList = QDir(_WallpaperDir).entryList();
    QString _actual_picturefile = _wallpaperfile;
    for(int i = 0; i < _wallList.size(); i++)
    {
        if(!(_wallList.at(i).contains(_actual_picturefile)))
        {
            QFile _file(_WallpaperDir+"/"+_wallList.at(i));
            _file.remove();
        }
    }
}

void MainWindow::updateContextMenu()
{
    //init MainContextMenu and set new to refresh the description and the Wallpaper-thumbnail
    init_MainContextMenu();
    if(_IsUnity==true){
        mSystemTrayIcon->setContextMenu(menu);
    }
}

void MainWindow::manage_wallpapers()
{
    manage_database ManageDatabase;
    ManageDatabase.init_database();
    if(ManageDatabase._initDB_failed==false)
    {
        ManageDatabase.read_date();
        for(int i = 0; i < ManageDatabase.datelist.size(); i++)
        {
            QDate current_date = QDate::currentDate();
            QDate selected_date = QDate::fromString(ManageDatabase.datelist.at(i), "yyyyMMdd");
            if(selected_date.daysTo(current_date)>_delete_older_than) {
                ManageDatabase.selected_datelist.append(ManageDatabase.datelist.at(i));
            }
        }
        ManageDatabase.create_filenamelist();
        for(int j = 0; j < ManageDatabase.filenamelist.size(); j++)
        {
            QString oldwallfile = _OldWallpaperDir+"/"+ManageDatabase.filenamelist.at(j);
            QFile old_wallp(oldwallfile);
            old_wallp.remove();
            qDebug() << "Picture " << oldwallfile << " deleted.";
            oldwallfile.clear();

            QString oldthumbfile = _thumbfiledir+"/"+ManageDatabase.filenamelist.at(j);
            QFile old_thumbf(oldthumbfile);
            old_wallp.remove();
            qDebug() << "Thumbnail-Picture " << oldthumbfile << " deleted.";
            oldthumbfile.clear();
        }
        ManageDatabase.delete_old_records();
    }

    /* now, delete old Wallpaperfiles and Thumbnail-Pictures from
     * the directory _OldWallpaperDir, if the filename of it is not
     * in the database (e.g. after a crash) */

    QStringList DirectoryContent_allWallpapers;


    if(_SaveOldWallpaper==true)
    {
         DirectoryContent_allWallpapers.append(QDir(_OldWallpaperDir).entryList() + QDir(_WallpaperDir).entryList());
    } else
    {
         DirectoryContent_allWallpapers.append(QDir(_WallpaperDir).entryList());
    }

    ManageDatabase.create_full_filenamelist();

    for(int k = 0; k < DirectoryContent_allWallpapers.size(); k++)
    {
        if(!(ManageDatabase.full_filenamelist.contains(DirectoryContent_allWallpapers.at(k))))
        {
            if(!(DirectoryContent_allWallpapers.at(k).toUtf8()==".") && !(DirectoryContent_allWallpapers.at(k).toUtf8()==".."))
            {
                QString file_to_delete = _OldWallpaperDir+"/"+DirectoryContent_allWallpapers.at(k);
                QFile file(file_to_delete);
                file.remove();
                qDebug() << "Picture " << DirectoryContent_allWallpapers.at(k).toUtf8() << " does not exist in the Database --> File removed.";
            }
        }
    }

    // Check if unused thumbnail-pictures are there and delete it.

    QStringList DirectoryContent_ThumbnailPictures;
    DirectoryContent_ThumbnailPictures.append(QDir(_thumbfiledir).entryList());
    ManageDatabase.create_full_thumbfilelist();
    for(int l=0; l < DirectoryContent_ThumbnailPictures.size(); l++)
    {
        if(!(ManageDatabase._full_thumbfilelist.contains(DirectoryContent_ThumbnailPictures.at(l))))
        {
            if(!(DirectoryContent_ThumbnailPictures.at(l).toUtf8()==".") && !(DirectoryContent_ThumbnailPictures.at(l).toUtf8()==".."))
            {
                QString Thumbfile_to_delete = _thumbfiledir+"/"+DirectoryContent_ThumbnailPictures.at(l);
                QFile thumbfile(Thumbfile_to_delete);
                thumbfile.remove();
                qDebug() << "Thumbnail-Picture " << DirectoryContent_ThumbnailPictures.at(l).toUtf8() << " does not exist in the Database --> File removed.";

            }
        }
    }

    /* delete "death" records in the database, if the filename is not
     * foundable in _OldWallpaperDir */

    ManageDatabase.existing_files = DirectoryContent_allWallpapers;
    ManageDatabase.delete_unused_records();

    QThread::msleep(100);
}

void MainWindow::_reset()
{
    QString _scriptfile = QDir::homePath()+"/reset_ddwp.sh";

    QFile reset_script(_scriptfile);
    if(!reset_script.exists(_scriptfile))
    {
        QString _script = "#!/bin/bash\n"
                "homeDir=\""+QDir::homePath()+"/.DailyDesktopWallpaperPlus\"\n"
                "\n"
                "rm $homeDir\'ddwp_database.sqlite\'\n"
                "rm $homeDir\'/mainicon.png\'\n"
                "rm $homeDir\'/settings.ini\'\n"
                "rm -r $homeDir";
        if (reset_script.open(QIODevice::Append))
        {
            QTextStream stream(&reset_script);
            stream << _script << Qt::endl;
        }
    }
    QProcess::execute("/bin/bash "+_scriptfile);
    reset_script.remove();
    QThread::msleep(300);
    QApplication::quit();
}

