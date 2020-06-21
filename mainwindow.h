#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "settingswindow.h"
#include "about.h"
#include "getbingwallpaper.h"
#include "getwinspotwallpaper.h"
#include "autostart_and_menuitem.h"
#include "setwallpaper.h"

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QString>
#include <QAction>
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
    void _menu_bingRes_1920x1200_click();
    void _menu_bingRes_1920x1080_click();
    void _menu_bingRes_1366x768_click();
    void _menu_bingRes_1280x720_click();
    void _menu_bingRes_1024x768_click();
    void _wspotRef_click();
    void _show_photobrowser_click();

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
    QString _appVersion;

    QAction * _bing_wall_option;
    QAction * _winspot_wall_option;

    int _Parameter;

    bool _Autostart;
    bool _SaveOldWallpaper;
    bool _AutoChange;
    bool _create_menu_item;

    void init_MainContextMenu();
    void init_SystemTrayIcon();
    void set_values();
    void set_autostart();
    void no_autostart();
    void _write_provider_settings();
    void _write_binglocation_settings();
    void _write_bing_photo_resolution_settings();
    void check_dir();
    void downloadPhotoFromURLNEW();
    void _keeporremove_old_wallpaper();
    void _show_tooltip();
    void _setBingWallpaper();
    void _setWinSpotWallpaper();
    void _check_internet_connection();
    void set_tooltip_string();
    void load_wallpaper();
    void no_menu_item();
    void set_menu_item();
    void _write_AppVersion();
};

#endif // MAINWINDOW_H
