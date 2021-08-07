#ifndef PHOTOBROWSER_H
#define PHOTOBROWSER_H

#include "setwallpaper.h"

#include <QDialog>
#include <QString>
#include <QModelIndex>
#include <QFileSystemModel>
#include <QStandardItemModel>
#include <QFuture>
#include <QLabel>
#include <QVBoxLayout>
#include <QPushButton>

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

    QString _photobrowser_specific_wallpaperfilename;
    QString _pb_copyright_description_photo;
    QString _pb_headline;
    QString _pb_copyright_link;

signals:
    void UpdateItem(int, QImage);

private slots:
    void on_pushButton_clicked();
    void setWallpaperFile();
    void on_listView_clicked(const QModelIndex &index);
    void _getmoreInformationsButton_clicked();

private:
    Ui::PhotoBrowser *ui;
    setWallpaper _setwall;
    QVBoxLayout *_layout_details;
    QLabel *_label_Details;
    QLabel *_label_headline;
    QLabel *_label_headline1;
    QLabel *_label_description_and_copyright1;
    QLabel *_label_description_and_copyright;
    QLabel *_label_image_size1;
    QLabel *_label_image_size;
    QLabel *_preview_text;
    QLabel *_previewImageLabel;
    QPushButton *_getmoreInformations;

    QLabel _description;
    QLabel _headline;

    QString _OldWallpaperDir;
    QString _WallpaperDir;
    QString _wallpaperfile;
    QString _resolution;
    QString filename_new;
    QString _thumbfiledir;
    QString _thumbfilename;

    QStringList _filenamelist;
    QStringList _filenamelist_with_fullpath;
    QStringList _thumbfilelist;
    QStringList _idlist;

    int _scaled_picture_width;
    int _scaled_picture_height;
    int _size_height;
    int _size_width;

    bool running;
    bool _get_specific_values;
    bool _first_run;

    void _init_ImageList();
    void _read_settings();
    void removeWallpaperFile();
    void _setPictureRes();
    void _readResolution();
    void _get_values();
    void _display_details();
    void _select_first_item(const QModelIndex & index);
    void closeEvent(QCloseEvent * event);
    void reject();
};

#endif // PHOTOBROWSER_H
