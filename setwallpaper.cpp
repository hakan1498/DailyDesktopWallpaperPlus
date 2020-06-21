#include "setwallpaper.h"

#include <QProcess>
#include <QDebug>
#include <QPixmap>
#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QTextStream>
#include <QDir>
#include <QTimer>
#include <QEventLoop>

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
    pufferpicture = QDir::homePath()+"/.DailyDesktopWallpaperPlus/pufferpic.jpg";
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
        //MATE
        setWallpaper.execute("gsettings set org.mate.background picture-filename \""+backgroundfile+"\"");
    else if(_Parameter==5)
        //Unity (Ubuntu)
        setWallpaper.execute("gsettings set org.gnome.desktop.background picture-uri  \"file://"+backgroundfile+"\"");
    else if(_Parameter==6) {
        //KDE Plasma 5.x
        _export_PufferPicture();
        _create_bashfile();
        setWallpaper.execute("/bin/bash "+_scriptfile);
        _remove_bashfile();
        _remove_pufferpicture();
        }
    else if(_Parameter==7)
        //KDE3.x or TDE
        setWallpaper.execute("dcop kdesktop KBackgroundIface setWallpaper \""+backgroundfile+"\" 8");
    else if(_Parameter==8)
        //LXDE
        setWallpaper.execute("pcmanfm --set-wallpaper=\""+backgroundfile+"\"");
    else if(_Parameter==9)
         //XFCE
        setWallpaper.execute("xfconf-query --channel xfce4-desktop --property /backdrop/screen0/monitor0/workspace0/last-image --set \""+backgroundfile+"\"");
    else if(_Parameter==10) {
         //XFCE with auto. Monitor detection
        _xfce4_detect_monitors();
        for (int i = 0; i < _detected_monitors.size(); i++)
        {
            QString _selected_monitor = _detected_monitors.at(i);
            setWallpaper.execute("xfconf-query --channel xfce4-desktop --property "+_selected_monitor+" --set \""+backgroundfile+"\"");

            //set delay for 100ms
            QEventLoop loop;
                QTimer::singleShot(100, &loop, SLOT(quit()));
            loop.exec();
        }
    }
}

void setWallpaper::_xfce4_detect_monitors()
{
    QProcess _list_monitors;
    _list_monitors.start("xfconf-query -c xfce4-desktop -l | grep \"last-image\" ");
    _list_monitors.waitForStarted();
    if(_list_monitors.state()==QProcess::NotRunning)
            qDebug() << "Monitor detection failed!";
    _list_monitors.waitForFinished();

    out = _list_monitors.readAllStandardOutput();
    _detected_monitors = out.split("\n");
}

void setWallpaper::_export_PufferPicture()
{
    //export pufferpic.jpg from RessourceFile to AppDir to set the wallpaper on KDE Plasma 5.x

    QPixmap pufferpicture_export = QPixmap (":/pufferpic.jpg");
    pufferpicture_export.save(pufferpicture);
}

void setWallpaper::_create_bashfile()
{
    // Create a scriptfile

    _scriptfile = QDir::homePath()+"/.DailyDesktopWallpaperPlus/spw.sh";

    QFile set_wallpaper_plasma(_scriptfile);
    if(!set_wallpaper_plasma.exists(_scriptfile))
    {
        QString content = "#!/bin/bash\n"
                "wallpaperDir=\""+_WallpaperDir+"\"\n"
                "\n"
                "WallpaperDir="+QApplication::applicationDirPath()+"/"+"DailyDesktopWallpaperPlus_wallpapers\n"
                " \n"
                "dbus-send --session --dest=org.kde.plasmashell --type=method_call /PlasmaShell org.kde.PlasmaShell.evaluateScript 'string: \n"
                "var Desktops = desktops(); \n"
                "print (Desktops); \n"
                "for (i=0;i<Desktops.length;i++) {\n"
                "        d = Desktops[i];\n"
                "        d.wallpaperPlugin = \"org.kde.image\";\n"
                "        d.currentConfigGroup = Array(\"Wallpaper\",\n"
                "                                    \"org.kde.image\",\n"
                "                                    \"General\");\n"
                "        d.writeConfig(\"Image\", \"file://"+pufferpicture+"\");\n"
                "}'\n"
                "sleep 0.2\n"
                " \n"
                "dbus-send --session --dest=org.kde.plasmashell --type=method_call /PlasmaShell org.kde.PlasmaShell.evaluateScript 'string: \n"
                "var Desktops = desktops(); \n"
                "print (Desktops); \n"
                "for (i=0;i<Desktops.length;i++) {\n"
                "        d = Desktops[i];\n"
                "        d.wallpaperPlugin = \"org.kde.image\";\n"
                "        d.currentConfigGroup = Array(\"Wallpaper\",\n"
                "                                    \"org.kde.image\",\n"
                "                                    \"General\");\n"
                "        d.writeConfig(\"Image\", \"file://"+backgroundfile+"\");\n"
                "}'";

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
    QFile pp(pufferpicture);
    pp.remove();
}
