#ifndef MANAGE_DATABASE_H
#define MANAGE_DATABASE_H

#include <QSqlQuery>
#include <QSqlDatabase>
#include <QStringList>

class manage_database
{
public:
    void init_database();
    void add_record();
    void delete_old_records();
    void read_date();
    void create_filenamelist();
    void create_full_filenamelist();
    void get_last_record();
    void delete_unused_records();
    void get_specific_values();

    QStringList datelist;
    QStringList selected_datelist;
    QStringList filenamelist;
    QStringList full_filenamelist;
    QStringList existing_files;

    QString _add_record_urlBase;
    QString _add_record_copyright_description;
    QString _add_record_copyright_link;
    QString _add_record_headline;
    QString _add_record_filename;
    QString date_timestamp;
    QString _last_rec_description_and_copyright;
    QString _last_rec_headline;
    QString _last_rec_filename;
    QString _last_browser_url;
    QString _photobrowser_specific_filename;
    QString _photobrowser_specific_desc;
    QString _photobrowser_specific_headline;
    QString _photobrowser_specific_browser_url;

    bool _initDB_failed;
    bool _request_id;

    int _size_height;
    int _size_width;
    int _last_rec_height;
    int _last_rec_width;
    int _out_height;
    int _out_width;

private:
    void init_table();
    void open_database();
    void close_database();

    bool _openDB_error;

    QString _databaseFilePath;

    QSqlDatabase ddwp_db = QSqlDatabase::addDatabase("QSQLITE");
};

#endif // MANAGE_DATABASE_H
