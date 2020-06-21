#include "about.h"
#include "ui_about.h"

#include <QApplication>
#include <QSettings>
#include <QString>
#include <QDesktopServices>
#include <QDir>
#include <QUrl>
#include <QCloseEvent>

About::About(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::About)
{
    ui->setupUi(this);
    readVersion();
}

About::~About()
{
    delete ui;
}

void About::closeEvent(QCloseEvent * event)
{
    // stop close event and hide window;
    event->ignore();
    this->hide();
}

void About::reject() {
    this->hide();
}

void About::readVersion()
{
    QString _iniFilePath = QDir::homePath()+"/.DailyDesktopWallpaperPlus/settings.ini";

    QSettings _settings(_iniFilePath, QSettings::IniFormat);

    _settings.beginGroup("VERSION");
    QString _version = _settings.value("Version","").toString();
    _settings.endGroup();

    ui->label_3->clear();
    ui->label_3->setText("Version: "+_version);
}

void About::on_pushButton_clicked()
{
    this->hide();
}

void About::on_pushButton_2_clicked()
{
    QDesktopServices::openUrl(QUrl("https://paypal.me/PGC1991"));
}

void About::on_pushButton_3_clicked()
{
    QDesktopServices::openUrl(QUrl("https://sourceforge.net/projects/dailydesktopwallpaperplus/"));
}
