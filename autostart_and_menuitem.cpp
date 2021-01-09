#include "autostart_and_menuitem.h"

#include <QFile>
#include <QPixmap>
#include <QDir>
#include <QApplication>
#include <QTextStream>
#include <QProcess>
#include <QDebug>
#include <QtGlobal>

void Autostart_and_MenuItem::set_autostartLocation()
{
    /* read variable $XDG_CONFIG_HOME.
     * If the variable is empty then set default directory $HOME/.config
     * If the variable contains more than one directory (e.g. PATH1:PATH2...)
     * then split by ":" and select the first directory as autostartDir. */

    QString _confDirVariable = qgetenv("XDG_CONFIG_HOME");

    if(_confDirVariable.contains(":")) {
        _detected_autostart_dirs = _confDirVariable.split(":");
        _autostart_dir = _detected_autostart_dirs.at(0)+"/autostart";
    } else {
        //check if the variable contains a filepath; if empty then set default path
        if(_confDirVariable.contains("/")) {
            _autostart_dir = _confDirVariable.toUtf8()+"/autostart";
        } else {
            _autostart_dir = QDir::homePath()+"/.config/autostart";
        }
    }
}

void Autostart_and_MenuItem::MainAppDef()
{
    //export the iconfile for the App
    QPixmap mainicon = QPixmap (":/128.png");
    mainicon.save(QDir::homePath()+"/.DailyDesktopWallpaperPlus/mainicon.png");

    mainApp = QApplication::applicationFilePath();
    mainApp_name = QApplication::applicationName();
    _file_content ="[Desktop Entry]\n"
        "Type=Application\n"
        "Exec=/bin/bash "+QApplication::applicationDirPath()+"/DailyDesktopWallpaperPlus.sh\n"
        "Hidden=false\n"
        "Name="+mainApp_name+"\n"
        "Comment=Use the daily wallpaper from bing or from Windows Spotlight on your Desktop!\n"
        "Terminal=false\n"
        "Keywords=background;wallpaper;bing;\n"
        "Icon="+QDir::homePath()+"/.DailyDesktopWallpaperPlus/mainicon.png";
}

void Autostart_and_MenuItem::no_autostart()
{
    set_autostartLocation();
    QString mainApp_name = QApplication::applicationName();
    QFile autostart_file(_autostart_dir+"/"+mainApp_name+".desktop");
    autostart_file.remove();
}

void Autostart_and_MenuItem::set_autostart()
{
    MainAppDef();
    set_autostartLocation();

    QDir autostart_dir(_autostart_dir);
    if (!autostart_dir.exists())
    {
        autostart_dir.mkpath(_autostart_dir);
    }

    QFile autostart_file(_autostart_dir+"/"+mainApp_name+".desktop");
    if(autostart_file.exists())
    {
        QFile _desktop_file(_autostart_dir+"/"+mainApp_name+".desktop");
        _desktop_file.remove();
    }

    // write .desktop file
    if (autostart_file.open(QIODevice::Append))
    {
        QTextStream stream(&autostart_file);
        stream <<_file_content<<endl;
    }
}

void Autostart_and_MenuItem::no_menuitem()
{
    QString mainApp_name = QApplication::applicationName();
    QString _path_menuitem = QDir::homePath()+"/.local/share/applications";
    QFile menuitem_file(_path_menuitem+"/"+mainApp_name+".desktop");
    menuitem_file.remove();
}

void Autostart_and_MenuItem::set_menuitem()
{
    MainAppDef();

    QString _path_menuitem = QDir::homePath()+"/.local/share/applications";

    QDir menuitem_dir(_path_menuitem);
    if (!menuitem_dir.exists())
    {
        menuitem_dir.mkpath(_path_menuitem);
    }

    QFile menuitem_file(_path_menuitem+"/"+mainApp_name+".desktop");
    if(menuitem_file.exists())
    {
        QFile _desktop_file(_path_menuitem+"/"+mainApp_name+".desktop");
        _desktop_file.remove();
    }

    // write .desktop file
    if (menuitem_file.open(QIODevice::Append))
    {
        QTextStream stream(&menuitem_file);
        stream <<_file_content<<endl;
    }
}
