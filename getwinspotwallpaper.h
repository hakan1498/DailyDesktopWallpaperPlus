#ifndef GETWINSPOTWALLPAPER_H
#define GETWINSPOTWALLPAPER_H

#include <QString>
#include <QByteArray>
#include <QSettings>
#include <QFile>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class GetWinSpotWallpaper
{
public:
    QByteArray downloadedPhotoData() const;
    QString _wspot_photo_description;
    QString _wspot_title_text;
    QString _bing_searchlink;

    void get_wallpaper();
    void parse_json_wspot();
    void read_settings();
    void download_file();
    void download_photo_option();
    void get_json_file();

private:
    QFile jsonFile;
    QNetworkAccessManager dl_manager;
    QByteArray downloaded_photo_data;
    QString _reply;
    QNetworkReply *reply;

    bool download_photo;

    void fileDownloaded();
    void remove_jsonFile();
    void write_settings();
    void saveImage();

    QString _iniFilePath;
    QString _wspot_photo_url;
    QString urlBase;
    QString _bingresolution;
    QString _binglocation;
    QString _WallpaperDir;
};

#endif // GETWINSPOTWALLPAPER_H
