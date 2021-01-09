#include "manage_database.h"

#include <QSettings>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QSql>
#include <QSqlRelationalTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <QSqlRecord>
#include <QSqlError>
#include <QSqlRecord>
#include <QString>
#include <QDate>
#include <QDateTime>

void manage_database::init_database()
{
    open_database();
    if(_openDB_error==false)
    {
        init_table();
        close_database();
        _initDB_failed=false;
    } else
    {
        _initDB_failed=true;
    }
}

void manage_database::open_database()
{
    _databaseFilePath = QDir::homePath()+"/.DailyDesktopWallpaperPlus/ddwp_database.sqlite";
    ddwp_db.setDatabaseName(_databaseFilePath);
    ddwp_db.open();
    if(!ddwp_db.open())
    {
        qDebug() << ddwp_db.lastError();
        qDebug() << "Database could not be open.";
        _openDB_error = true;
    } else
    {
        _openDB_error = false;
    }
}

void manage_database::init_table()
{
    if(!ddwp_db.tables().contains(QLatin1String("ddwp_history")))
    {
        QSqlQuery ddwp_query(ddwp_db);
        ddwp_query.exec("create table ddwp_history (id int primary key, "
                   "date varchar(30), description_and_copyright varchar(900), title varchar(500),"
                   "filename varchar(150), browser_url varchar(500), size_width int, size_height int)");
        qDebug() << "New ddwp_history table created.";
    } else
    {
        qDebug() << "Table ddwp_history exists already.";
    }
}

void manage_database::add_record()
{
    open_database();
    if(_openDB_error==false)
    {
        QSqlQuery ddwp_query(ddwp_db);
        ddwp_query.prepare("INSERT INTO ddwp_history (id, date, description_and_copyright, title, filename, browser_url, size_width ,size_height) "
                                    "VALUES (:id, :date, :description_and_copyright, :title, :filename, :browser_url, :size_width, :size_height)");
        ddwp_query.bindValue(":id", QDateTime::currentDateTime().toString("yyyyMMddHHmmss"));
        ddwp_query.bindValue(":date", QDate::currentDate().toString("yyyyMMdd"));
        ddwp_query.bindValue(":description_and_copyright", _add_record_copyright_description);
        ddwp_query.bindValue(":title", _add_record_headline);
        ddwp_query.bindValue(":filename", _add_record_filename);
        ddwp_query.bindValue(":browser_url", _add_record_copyright_link);
        ddwp_query.bindValue(":size_width", _size_width);
        ddwp_query.bindValue(":size_height", _size_height);
        if(!ddwp_query.exec())
          qDebug() << ddwp_query.lastError();
        else
          qDebug() << "Entry added successful.";
        close_database();
    } else
    {
        qDebug() << "Error while connect Database.";
    }
}

void manage_database::read_date()
{
    open_database();
    if(_openDB_error==false)
    {
        QSqlQuery ddwp_query(ddwp_db);
        ddwp_query.exec("SELECT date FROM ddwp_history");
        QSqlRecord record_date = ddwp_query.record();
        while (ddwp_query.next())
        {
            datelist.append(ddwp_query.value(record_date.indexOf("date")).toString());
        }
        close_database();
    }
}

void manage_database::create_filenamelist()
{
    // Add the filenames to a list who are in the row
    // of these dates from the QStringList "datelist"
    open_database();
    if(_openDB_error==false)
    {
        QSqlQuery ddwp_query(ddwp_db);
        for(int i = 0; i < selected_datelist.size(); i++)
        {
            ddwp_query.prepare("SELECT filename FROM ddwp_history WHERE date=\'"+selected_datelist.at(i).toUtf8()+"\'");
            if(ddwp_query.exec() && ddwp_query.next())
            {
               if(!filenamelist.contains(ddwp_query.value(0).toString().toUtf8()))
               {
                   filenamelist.append(ddwp_query.value(0).toString().toUtf8());
               }
            }
        }
        //Check if more entries with the same date and add it.
        while(ddwp_query.next()){
            if(!filenamelist.contains(ddwp_query.value(0).toString().toUtf8()))
            {
                filenamelist.append(ddwp_query.value(0).toString().toUtf8());
            }
        }
        ddwp_query.clear();
        close_database();
    }
}

void manage_database::delete_old_records()
{
    //delete records with contains the entries of the QStringList "selected_datelist" (old Wallpapers)
    open_database();
    if(_openDB_error==false)
    {
        QSqlQuery ddwp_query(ddwp_db);
        for(int i = 0; i < selected_datelist.size(); i++)
        {
            ddwp_query.prepare("DELETE FROM ddwp_history WHERE date = \'"+selected_datelist.at(i).toUtf8()+"\'");
            if(!ddwp_query.exec())
            {
                qDebug() << ddwp_query.lastError();
            }
        }
        ddwp_query.clear();
        close_database();
    }
}

void manage_database::get_last_record()
{
    open_database();
    if(_openDB_error==false)
    {
        QSqlQuery ddwp_query(ddwp_db);

        ddwp_query.prepare("SELECT description_and_copyright FROM ddwp_history");
        ddwp_query.exec();
        while(ddwp_query.next()){
            if(ddwp_query.last()) {
                _last_rec_description_and_copyright = ddwp_query.value(0).toString();
            }
        }
        ddwp_query.prepare("SELECT title FROM ddwp_history");
        ddwp_query.exec();
        while(ddwp_query.next()){
            if(ddwp_query.last()) {
                _last_rec_headline = ddwp_query.value(0).toString();
            }
        }
        ddwp_query.prepare("SELECT filename FROM ddwp_history");
        ddwp_query.exec();
        while(ddwp_query.next()){
            if(ddwp_query.last()) {
                _last_rec_filename = ddwp_query.value(0).toString();
            }
        }
        ddwp_query.prepare("SELECT browser_url FROM ddwp_history");
        ddwp_query.exec();
        while(ddwp_query.next()){
            if(ddwp_query.last()) {
                _last_browser_url = ddwp_query.value(0).toString();
            }
        }
        ddwp_query.prepare("SELECT size_width FROM ddwp_history");
        ddwp_query.exec();
        while(ddwp_query.next()){
            if(ddwp_query.last()) {
                _last_rec_width = ddwp_query.value(0).toInt();
            }
        }
        ddwp_query.prepare("SELECT size_height FROM ddwp_history");
        ddwp_query.exec();
        while(ddwp_query.next()){
            if(ddwp_query.last()) {
                _last_rec_height = ddwp_query.value(0).toInt();
            }
        }
        ddwp_query.clear();
        close_database();
    }
}

void manage_database::create_full_filenamelist()
{
    open_database();
    if(_openDB_error==false)
    {
        QSqlQuery ddwp_query(ddwp_db);
        ddwp_query.prepare("SELECT filename FROM ddwp_history");
        ddwp_query.exec();
        while(ddwp_query.next())
        {
            if(!full_filenamelist.contains(ddwp_query.value(0).toString().toUtf8()))
            {
                full_filenamelist.append(ddwp_query.value(0).toString().toUtf8());
            }
        }
        ddwp_query.clear();
        close_database();
    }
}

void manage_database::delete_unused_records()
{
    /* check if "death" records in the database, if the filename is not
     * foundable in _OldWallpaperDir and delete it */

    open_database();
    if(_openDB_error==false)
    {
        QStringList files_from_db;
        QStringList not_existing_files;
        QSqlQuery ddwp_query(ddwp_db);
        ddwp_query.prepare("SELECT filename FROM ddwp_history");
        ddwp_query.exec();
        while(ddwp_query.next())
        {
            if(!files_from_db.contains(ddwp_query.value(0).toString().toUtf8()))
            {
                files_from_db.append(ddwp_query.value(0).toString().toUtf8());
            }
        }
        for(int j = 0; j < files_from_db.size(); j++)
        {
            if(!existing_files.contains(files_from_db.at(j)))
            {
                not_existing_files.append(files_from_db.at(j));
            }
        }
        for(int k = 0; k < not_existing_files.size(); k++)
        {
            ddwp_query.prepare("DELETE FROM ddwp_history WHERE filename = \'"+not_existing_files.at(k).toUtf8()+"\'");
            if(!ddwp_query.exec())
            {
                qDebug() << ddwp_query.lastError();
            }
        }
        ddwp_query.clear();
        close_database();
    }
}

void manage_database::get_specific_values()
{
    open_database();
    if(_openDB_error==false)
    {
        QSqlQuery ddwp_query(ddwp_db);
        ddwp_query.prepare("SELECT description_and_copyright FROM ddwp_history WHERE filename = \'"+_photobrowser_specific_filename+"\'");
        if(ddwp_query.exec() && ddwp_query.next())
        {
            _photobrowser_specific_desc = ddwp_query.value(0).toString();
        }
        ddwp_query.prepare("SELECT title FROM ddwp_history WHERE filename = \'"+_photobrowser_specific_filename+"\'");
        if(ddwp_query.exec() && ddwp_query.next())
        {
            _photobrowser_specific_headline = ddwp_query.value(0).toString();
        }
        ddwp_query.prepare("SELECT browser_url FROM ddwp_history WHERE filename = \'"+_photobrowser_specific_filename+"\'");
        if(ddwp_query.exec() && ddwp_query.next())
        {
            _photobrowser_specific_browser_url = ddwp_query.value(0).toString();

        }
        ddwp_query.prepare("SELECT size_width FROM ddwp_history WHERE filename = \'"+_photobrowser_specific_filename+"\'");
        if(ddwp_query.exec() && ddwp_query.next())
        {
            _out_width = ddwp_query.value(0).toInt();

        }
        ddwp_query.prepare("SELECT size_height FROM ddwp_history WHERE filename = \'"+_photobrowser_specific_filename+"\'");
        if(ddwp_query.exec() && ddwp_query.next())
        {
            _out_height = ddwp_query.value(0).toInt();

        }
        ddwp_query.clear();
        close_database();
    }
}

void manage_database::close_database()
{
    ddwp_db.close();
    QSqlDatabase::removeDatabase(_databaseFilePath);
}
