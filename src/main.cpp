#include <QApplication>
#include <QSettings>

#include "mainwindow.h"
#include "downloadmanager.h"
#include "item.h"

int main(int argc, char *argv[])
{
    qRegisterMetaTypeStreamOperators<QDownloader::Item::ItemInfo>("QDownloader::Item::ItemInfo");
    QCoreApplication::setOrganizationName("QDownloader");
    QCoreApplication::setOrganizationDomain("bramas.github.io/QDownloader");
    QCoreApplication::setApplicationName("QDownloader");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QApplication a(argc, argv);
    QDownloader::DownloadManager::init();
    QDownloader::MainWindow wid;
    wid.show();
    return a.exec();
}
