#ifndef AUTOSTART_AND_MENUITEM_H
#define AUTOSTART_AND_MENUITEM_H

#include <QString>
#include <QStringList>

class Autostart_and_MenuItem
{
public:
    void no_autostart();
    void set_autostart();
    void no_menuitem();
    void set_menuitem();

private:
    QString mainApp;
    QString mainApp_name;
    QString _file_content;
    QString _autostart_dir;

    QStringList _detected_autostart_dirs;

    void MainAppDef();
    void set_autostartLocation();
};

#endif // AUTOSTART_AND_MENUITEM_H
