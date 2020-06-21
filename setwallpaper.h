#ifndef SETWALLPAPER_H
#define SETWALLPAPER_H

#include <QString>
#include <QStringList>

class setWallpaper
{
public:
    void _set_wallpaper();

private:
    QString backgroundfile;
    QString pufferpicture;
    QString _scriptfile;
    QString _WallpaperDir;
    QString _OldWallpaperDir;
    QString out;

    QStringList _detected_monitors;

    int _Parameter;

    void _export_PufferPicture();
    void _create_bashfile();
    void _remove_bashfile();
    void _remove_pufferpicture();
    void _read_settings();
    void _apply_wallpaper_gdbus();
    void _xfce4_monitor_detect();
    void _xfce4_detect_monitors();
};

#endif // SETWALLPAPER_H
