#include "settingswindow.h"
#include "ui_settingswindow.h"

#include <QFile>
#include <QSettings>
#include <QApplication>
#include <QFileDialog>
#include <QDir>
#include <QMessageBox>
#include <QCloseEvent>

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    init_settings();
}

void SettingsWindow::closeEvent(QCloseEvent * event)
{
    // stop close event and hide window;
    event->ignore();
    this->hide();
}

void SettingsWindow::reject() {
    this->hide();
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::init_settings()
{
    _iniFilePath = QDir::homePath()+"/.DailyDesktopWallpaperPlus/settings.ini";

    // read settings from INI file
    QSettings settings(_iniFilePath, QSettings::IniFormat);

    settings.beginGroup("SETTINGS");
    _WallpaperDir = settings.value("WallpaperDir","").toString();
    _OldWallpaperDir = settings.value("OldWallpaperDir","").toString();
    _Autostart = settings.value("Autostart","").toBool();
    _SaveOldWallpaper = settings.value("SaveOldWallpaper","").toBool();
    _create_menu_item = settings.value("create_menu_item","").toBool();
    _delete_automatically = settings.value("delete_automatically","").toBool();
    _delete_older_than = settings.value("delete_older_than","").toInt();
    settings.endGroup();

    settings.beginGroup("SETWALLPAPER");
    _AutoChange = settings.value("AutoChange","").toBool();
    int _Parameter = settings.value("Parameter","").toInt();
    settings.endGroup();

    ui->lineEdit_2->setEnabled(false);
    ui->label_2->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->radioButton_2->setEnabled(true);
    ui->checkBox_2->setChecked(false);
    ui->checkBox_3->setChecked(false);
    ui->lineEdit_2->setReadOnly(true);
    ui->lineEdit->setReadOnly(true);
    ui->lineEdit_2->setReadOnly(true);
    ui->label_3->setEnabled(false);
    ui->comboBox->setEnabled(false);
    ui->checkBox_4->setEnabled(false);
    ui->spinBox->setEnabled(false);

    ui->lineEdit->setText(_WallpaperDir);

    if (_create_menu_item == true) {
        ui->checkBox->setChecked(true);
    } else {
        ui->checkBox->setChecked(false);
    }

    if (_SaveOldWallpaper == true)
    {
        ui->radioButton->setChecked(true);
        ui->lineEdit_2->setEnabled(true);
        ui->pushButton_2->setEnabled(true);
        ui->label_2->setEnabled(true);
        ui->lineEdit_2->setText(_OldWallpaperDir);
        ui->checkBox_4->setEnabled(true);
        if(_delete_automatically==true) {
            ui->checkBox_4->setChecked(true);
            ui->spinBox->setEnabled(true);
        }
        ui->spinBox->setValue(_delete_older_than);
    } else
    {
        ui->radioButton_2->setChecked(true);
    }

    if (_Autostart == true)
    {
        ui->checkBox_2->setChecked(true);
    }

    if (_AutoChange == true)
    {
        ui->checkBox_3->setChecked(true);
        ui->label_3->setEnabled(true);
        ui->comboBox->setEnabled(true);
    }

    ui->comboBox->addItem("Budgie");
    ui->comboBox->addItem("Cinnamon");
    ui->comboBox->addItem("Deepin Desktop Environment");
    ui->comboBox->addItem("GNOME");
    ui->comboBox->addItem("MATE");
    ui->comboBox->addItem("Unity");
    ui->comboBox->addItem("KDE Plasma 5.x");
    ui->comboBox->addItem("KDE 3.x / Trinity Desktop Environment");
    ui->comboBox->addItem("LXDE");
    ui->comboBox->addItem("XFCE");

    ui->comboBox->setCurrentIndex(_Parameter);
}

void SettingsWindow::write_settings()
{
    QString combobox_index;
    combobox_index.sprintf("%d", ui->comboBox->currentIndex());

    _WallpaperDir = ui->lineEdit->text();

    _Autostart = ui->checkBox_2->checkState();
    _AutoChange = ui->checkBox_3->checkState();

    _delete_automatically = ui->checkBox_4->checkState();
    _delete_older_than = ui->spinBox->value();

    if(ui->radioButton->isChecked() == true)
    {
        _SaveOldWallpaper = true;
        _OldWallpaperDir = ui->lineEdit_2->text();
    } else
    {
        _SaveOldWallpaper = false;
    }

    if(ui->checkBox->isChecked() == true) {
        _create_menu_item = true;
    } else {
        _create_menu_item = false;
    }

    QSettings settings(_iniFilePath, QSettings::IniFormat);

    settings.beginGroup("SETTINGS");
    settings.setValue("WallpaperDir", _WallpaperDir);
    settings.setValue("Autostart", _Autostart);
    settings.setValue("create_menu_item", _create_menu_item);

    if(_SaveOldWallpaper == true) {
        settings.setValue("delete_automatically", _delete_automatically);
        settings.setValue("delete_older_than", _delete_older_than);
        settings.setValue("OldWallpaperDir", _OldWallpaperDir);
    }

    settings.setValue("SaveOldWallpaper", _SaveOldWallpaper);
    settings.endGroup();
    settings.sync();

    settings.beginGroup("SETWALLPAPER");
    settings.setValue("AutoChange", _AutoChange);
    settings.setValue("Parameter", combobox_index);
    settings.endGroup();
    settings.sync();
}

void SettingsWindow::on_pushButton_4_clicked()
{
    this->hide();
}

void SettingsWindow::on_pushButton_3_clicked()
{
    write_settings();

    if(_Autostart==true){
        autostart_and_menuitem.set_autostart();
    } else
    {
        autostart_and_menuitem.no_autostart();
    }

    this->hide();
}

void SettingsWindow::on_radioButton_clicked()
{
    ui->label_2->setEnabled(true);
    ui->lineEdit_2->setEnabled(true);
    ui->pushButton_2->setEnabled(true);
    ui->lineEdit_2->setText(_OldWallpaperDir);

    if(ui->lineEdit_2->text() == "")
    {
        selectOldWallpaperDir();
    }
}

void SettingsWindow::on_radioButton_2_clicked()
{
    ui->label_2->setEnabled(false);
    ui->lineEdit_2->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->lineEdit_2->clear();
}

void SettingsWindow::on_pushButton_clicked()
{
    selectWallpaperDir();
}

void SettingsWindow::on_pushButton_2_clicked()
{
    selectOldWallpaperDir();
}

void SettingsWindow::selectWallpaperDir()
{
    select_dir = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                 QDir::homePath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    ui->lineEdit->setText(select_dir);
}

void SettingsWindow::selectOldWallpaperDir()
{
    select_dir_2 = QFileDialog::getExistingDirectory(this, tr("Select Directory"),
                                                 QDir::homePath(),
                                                 QFileDialog::ShowDirsOnly
                                                 | QFileDialog::DontResolveSymlinks);
    ui->lineEdit_2->setText(select_dir_2);
}

void SettingsWindow::on_checkBox_3_clicked()
{
    if(ui->checkBox_3->isChecked() == true) {
        ui->label_3->setEnabled(true);
        ui->comboBox->setEnabled(true);
    } else {
        ui->label_3->setEnabled(false);
        ui->comboBox->setEnabled(false);
    }
}

void SettingsWindow::on_checkBox_4_clicked()
{
    if(ui->checkBox_4->isChecked() == true) {
        ui->spinBox->setEnabled(true);
    } else {
        ui->spinBox->setEnabled(false);
    }
}
