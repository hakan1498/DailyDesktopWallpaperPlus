#ifndef PHOTOBROWSER_H
#define PHOTOBROWSER_H

#include "mainwindow.h"

#include <QDialog>
#include <QString>
#include <QModelIndex>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QFuture>

namespace Ui {
class PhotoBrowser;
}

class PhotoBrowser : public QDialog
{
    Q_OBJECT

public:
    explicit PhotoBrowser(QWidget *parent = 0);
    ~PhotoBrowser();

    bool wallchanged;

signals:
    void UpdateItem(int, QImage);

private slots:
    void List(QFileInfoList filesList, QSize size);
    void setThumbs(int index, QImage img);
    void on_pushButton_clicked();
    void setWallpaperFile();

private:
    Ui::PhotoBrowser *ui;
    setWallpaper _setwall;
    QFileSystemModel * fsmodel;
    QStandardItemModel *fmodel;
    QFuture<void> thread;

    QString _OldWallpaperDir;
    QString _WallpaperDir;
    QString _wallpaperfile;

    bool running;

    void _init_ImageList();
    void _read_path();
    void removeWallpaperFile();
    void closeEvent(QCloseEvent * event);
    void reject();
};

#endif // PHOTOBROWSER_H
