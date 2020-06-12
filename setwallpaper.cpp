#include "setwallpaper.h"

#include <QProcess>
#include <QPixmap>
#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QTextStream>
#include <QDir>

void setWallpaper::_read_settings()
{
    QString _iniFilePath = QDir::homePath()+"/.DailyDesktopWallpaperPlus/settings.ini";

    QSettings settings(_iniFilePath, QSettings::IniFormat);

    settings.beginGroup("SETTINGS");
    _WallpaperDir = settings.value("WallpaperDir","").toString();
    _OldWallpaperDir = settings.value("OldWallpaperDir","").toString();
    settings.endGroup();

    settings.beginGroup("SETWALLPAPER");
    _Parameter = settings.value("Parameter","").toInt();
    settings.endGroup();
}

void setWallpaper::_set_wallpaper()
{
    _read_settings();

    backgroundfile = _WallpaperDir+"/background.jpg";
    pufferpicture = QApplication::applicationDirPath()+"/pufferpic.jpg";
    QString go;
    QProcess setWallpaper;

    if(_Parameter==0)
        //Budgie
        setWallpaper.execute("gsettings set org.gnome.desktop.background picture-uri  \"file://"+backgroundfile+"\"");
    else if(_Parameter==1)
        //Cinnamon
        setWallpaper.execute("gsettings set org.cinnamon.desktop.background picture-uri  \"file://"+backgroundfile+"\"");
    else if(_Parameter==2)
        //DDE
        setWallpaper.execute("gsettings set com.deepin.wrap.gnome.desktop.background picture-uri \"file://"+backgroundfile+"\"");
    else if(_Parameter==3)
        //GNOME
        setWallpaper.execute("gsettings set org.gnome.desktop.background picture-uri \"file://"+backgroundfile+"\"");
    else if(_Parameter==4)
        //KDE3.x or TDE
        setWallpaper.execute("dcop kdesktop KBackgroundIface setWallpaper \""+backgroundfile+"\" 8");
    else if(_Parameter==5) {
        //KDE Plasma 5.x
        _export_PufferPicture();
        _create_bashfile();
        setWallpaper.execute("/bin/bash "+_scriptfile);
        _remove_bashfile();
        _remove_pufferpicture();
    }
    else if(_Parameter==6)
        //LXDE
        setWallpaper.execute("pcmanfm --set-wallpaper=\""+backgroundfile+"\"");
    else if(_Parameter==7)
        //MATE
        setWallpaper.execute("gsettings set org.mate.background picture-filename \""+backgroundfile+"\"");
    else if(_Parameter==8)
        //XFCE
        setWallpaper.execute("xfconf-query --channel xfce4-desktop --property /backdrop/screen0/monitor0/workspace0/last-image --set \""+backgroundfile+"\"");
}

void setWallpaper::_export_PufferPicture()
{
    //export pufferpic.jpg from RessourceFile to AppDir to set the wallpaper on KDE Plasma 5.x

    QPixmap pufferpicture = QPixmap (":/pufferpic.jpg");
    pufferpicture.save(QApplication::applicationDirPath()+"/pufferpic.jpg");
}

void setWallpaper::_create_bashfile()
{
    // Create a scriptfile to execute the qdbus command correctly

    _scriptfile = QApplication::applicationDirPath()+"/"+"spw.sh";

    QFile set_wallpaper_plasma(_scriptfile);
    if(!set_wallpaper_plasma.exists(_scriptfile))
    {
        QString content = "#!/bin/bash\n"
                "wallpaperDir=\""+_WallpaperDir+"\"\n"
                "\n"
                "WallpaperDir="+QApplication::applicationDirPath()+"/"+"DailyDesktopWallpaperPlus_wallpapers\n"
                "qdbus org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell.evaluateScript 'var allDesktops = desktops();print (allDesktops);for (i=0;i<allDesktops.length;i++) {d = allDesktops[i];d.wallpaperPlugin = \"org.kde.image\";d.currentConfigGroup = Array(\"Wallpaper\", \"org.kde.image\", \"General\");d.writeConfig(\"Image\", \"file://"+pufferpicture+"\")}'\n"
                "qdbus org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell.evaluateScript 'var allDesktops = desktops();print (allDesktops);for (i=0;i<allDesktops.length;i++) {d = allDesktops[i];d.wallpaperPlugin = \"org.kde.image\";d.currentConfigGroup = Array(\"Wallpaper\", \"org.kde.image\", \"General\");d.writeConfig(\"Image\", \"file://"+backgroundfile+"\")}'\n";

        if (set_wallpaper_plasma.open(QIODevice::Append))
        {
            QTextStream stream(&set_wallpaper_plasma);
            stream <<content<<endl;
        }
    }
}

void setWallpaper::_remove_bashfile()
{
    QFile sr(_scriptfile);
    sr.remove();
}

void setWallpaper::_remove_pufferpicture()
{
    QFile pp(QApplication::applicationDirPath()+"/pufferpic.jpg");
    pp.remove();
}
