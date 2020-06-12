#ifndef AUTOSTART_AND_MENUITEM_H
#define AUTOSTART_AND_MENUITEM_H

#include <QString>

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

    void MainAppDef();
};

#endif // AUTOSTART_AND_MENUITEM_H
