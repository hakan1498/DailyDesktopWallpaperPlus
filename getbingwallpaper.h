#ifndef GETBINGWALLPAPER_H
#define GETBINGWALLPAPER_H

#include <QString>
#include <QByteArray>
#include <QObject>
#include <QSettings>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class GetBingWallpaper
{
public:
    QByteArray downloadedPhotoData() const;
    QString _no_internet_connection;

    void get_bing_wallpaper();
    void bing_basefile_parse();
    void read_settings();
    void download_file();
    void download_photo_option();
    void get_bing_basefile();

private:
    GetBingWallpaper *getbingwallpaper;
    QNetworkAccessManager dl_manager;
    QByteArray downloaded_photo_data;
    QNetworkReply *reply;

    bool download_photo;

    int _picture_size_height;
    int _picture_size_width;

    void fileDownloaded();
    void remove_baseFile();
    void write_settings();
    void saveImage();
    void add_record();

    QString _iniFilePath;
    QString _bing_photo_url;
    QString urlBase;
    QString _binglocation;
    QString _bingresolution;
    QString _WallpaperDir;
    QString _copyright_bing_photo;
    QString _headline_bing_desc;
    QString _copyright_link;
    QString _bing_reply;
    QString filename;
    QString _thumbfiledir;
    QString _thumb_filename;
};

#endif // GETBINGWALLPAPER_H
