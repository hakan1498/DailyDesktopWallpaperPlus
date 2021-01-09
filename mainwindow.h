#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settingswindow.h"
#include "about.h"
#include "getbingwallpaper.h"
#include "getwinspotwallpaper.h"
#include "autostart_and_menuitem.h"
#include "setwallpaper.h"
#include "manage_database.h"

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QString>
#include <QAction>
#include <QTimer>
#include <QtNetwork/QNetworkRequest>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkAccessManager>

namespace Ui {
class MainWindow;
}

class QSystemTrayIcon;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void set_values();
    void check_dir();
    void load_wallpaper();
    void init_MainContextMenu();
    void init_SystemTrayIcon();
    void checkFiles();
    void manage_wallpapers();

signals:
    void downloaded();

private slots:
    void _menu_settings_click();
    void _menu_about_click();
    void _menu_bing_wall_option_click();
    void _menu_winspot_wall_option_click();
    void _menu_bingRefresh_click();
    void _menu_usa_click();
    void _menu_japan_click();
    void _menu_china_click();
    void _menu_australia_click();
    void _menu_gb_click();
    void _menu_germany_click();
    void _menu_canada_click();
    void _menu_nz_click();
    void _menu_brazil_click();
    void _menu_france_click();
    void _menu_bingRes_UHD_click();
    void _menu_bingRes_1920x1200_click();
    void _menu_bingRes_1920x1080_click();
    void _menu_bingRes_1366x768_click();
    void _menu_bingRes_1280x720_click();
    void _menu_bingRes_1024x768_click();
    void _wspotRef_click();
    void _show_photobrowser_click();
    void _gotoBing_click();
    void _gotoBing_bing_click();
    void _reset();
    void slotActive(QSystemTrayIcon::ActivationReason r);


private:
    Ui::MainWindow *ui;
    GetBingWallpaper getbingwallpaper;
    GetWinSpotWallpaper getwinspotwallpaper;
    setWallpaper _setwall;
    Autostart_and_MenuItem autostart_and_menuitem;
    QSystemTrayIcon *mSystemTrayIcon;
    QMenu * menu;
    QNetworkReply *reply;
    QNetworkAccessManager dl_manager;
    QTimer *_autoChangeTimer = NULL;

    QString _iniFilePath;
    QString _selected_binglocation;
    QString _selected_bing_photo_resolution;
    QString _WallpaperDir;
    QString _OldWallpaperDir;
    QString _Provider;
    QString _tooltip_title;
    QString _tooltip_message;
    QString _resolution;
    QString _binglocation;
    QString _country;
    QString _appVersion;
    QString _wallpaperfile;
    QString _copyright_description_photo;
    QString _copyright_link;
    QString _headline;

    QImage _loadImage;
    QImage _descImage;

    QAction * _bing_wall_option;
    QAction * _winspot_wall_option;

    int _Parameter;
    int _delete_older_than;
    int _time_hours;
    int _time_minutes;
    int _time_seconds;
    int _time_milliseconds;

    bool _Autostart;
    bool _SaveOldWallpaper;
    bool _AutoChange;
    bool _create_menu_item;
    bool _IsUnity;
    bool _delete_automatically;
    bool _photobrowser_specific_values;
    bool _set_reset;

    void set_autostart();
    void no_autostart();
    void set_autoChange();
    void no_autoChange();
    void _write_provider_settings();
    void _write_binglocation_settings();
    void _write_bing_photo_resolution_settings();
    void _keeporremove_old_wallpaper();
    void _show_tooltip();
    void _setBingWallpaper();
    void _setWinSpotWallpaper();
    void _check_internet_connection();
    void set_tooltip_string();
    void no_menu_item();
    void set_menu_item();
    void _write_AppVersion();
    void updateContextMenu();
    void delete_backgroundimages();
    void load_bgp_specific_settings();
};

#endif // MAINWINDOW_H
