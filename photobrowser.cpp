#include "photobrowser.h"
#include "ui_photobrowser.h"
#include "itemdelegate.h"

#include <QSize>
#include <QFile>
#include <QDir>
#include <QSettings>
#include <QPixmap>
#include <QImage>
#include <QIcon>
#include <QStandardItem>
#include <QObject>
#include <QtConcurrent/QtConcurrent>
#include <QThreadPool>
#include <QCloseEvent>

PhotoBrowser::PhotoBrowser(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PhotoBrowser)
{
    ui->setupUi(this);

    _read_path();

    QThreadPool::globalInstance()->setMaxThreadCount(1);

    fsmodel = new QFileSystemModel(this);
    fsmodel->setRootPath(_OldWallpaperDir);

    fmodel = new QStandardItemModel(this);

    ui->listView->setResizeMode(QListView::Adjust);
    ui->listView->setFlow(QListView::LeftToRight);
    ui->listView->setSpacing(15);
    ui->listView->setWrapping(true);
    ui->listView->setUniformItemSizes(true);
    ui->listView->setIconSize(QSize (160, 120));
    ui->listView->setModel(fmodel);
    ui->listView->setViewMode(QListView::IconMode);
    ui->listView->setMovement(QListView::Static);
    ui->listView->setItemDelegate(new ItemDelegate(this));

    connect(this, SIGNAL(UpdateItem(int,QImage)), SLOT(setThumbs(int,QImage)));

    running = false;
    wallchanged = false;

    _init_ImageList();
}

void PhotoBrowser::closeEvent(QCloseEvent * event)
{
    // stop close event and hide window;
    event->ignore();
    this->hide();
}

void PhotoBrowser::reject() {
    this->hide();
}

PhotoBrowser::~PhotoBrowser()
{
    delete ui;
}

void PhotoBrowser::_read_path()
{
    QString _iniFilePath = QDir::homePath()+"/.DailyDesktopWallpaperPlus/settings.ini";

    // read settings from INI file
    QSettings settings(_iniFilePath, QSettings::IniFormat);

    settings.beginGroup("SETTINGS");
    _WallpaperDir = settings.value("WallpaperDir","").toString();
    _OldWallpaperDir = settings.value("OldWallpaperDir","").toString();
    settings.endGroup();
}

void PhotoBrowser::_init_ImageList()
{
    fmodel->clear();
    running = false;
    thread.waitForFinished();  
    QDir imageDir(_OldWallpaperDir);
    QFileInfoList WallpaperList = imageDir.entryInfoList(QStringList() << "*.jpg", QDir::Files);
    int WallpaperCount = WallpaperList.size();
    QPixmap placeholder = QPixmap(ui->listView->iconSize());
    placeholder.fill(Qt::gray);
    for (int i = 0; i < WallpaperCount; i++)
        fmodel->setItem(i, new QStandardItem(QIcon(placeholder), WallpaperList[i].baseName()));
    running = true;
    thread = QtConcurrent::run(this, &PhotoBrowser::List, WallpaperList, ui->listView->iconSize());
}

void PhotoBrowser::List(QFileInfoList WallpaperList, QSize size)
{
    int WallpaperCount = WallpaperList.size();

    for (int i = 0; running && i < WallpaperCount; i++)
    {
        QImage originalImage(WallpaperList[i].filePath());
        if (!originalImage.isNull())
        {
            QImage scaledImage = originalImage.scaled(size);
            if (!running) return;
            emit UpdateItem(i, scaledImage);
        }
    }
}

void PhotoBrowser::setThumbs(int index, QImage img)
{
    QIcon icon = QIcon(QPixmap::fromImage(img));
    QStandardItem *item = fmodel->item(index);
    fmodel->setItem(index, new QStandardItem(icon, item->text()));
}

void PhotoBrowser::on_pushButton_clicked()
{
    wallchanged = true;
    setWallpaperFile();
}

void PhotoBrowser::setWallpaperFile()
{
    QModelIndex index = ui->listView->currentIndex();
    QString _selected_wallpaperfile = index.data(Qt::DisplayRole).toString()+".jpg";

    QString _filename_bg = "background.jpg";

    QFile wallpaperfile(_WallpaperDir+"/background.jpg");
    wallpaperfile.remove();

    QFile::copy(_OldWallpaperDir+"/"+_selected_wallpaperfile, _WallpaperDir+"/"+_filename_bg);

    _setwall._set_wallpaper();
}
