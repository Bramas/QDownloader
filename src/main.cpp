#include <QApplication>

#include "mainwindow.h"
#include "downloadmanager.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QDownloader::DownloadManager::init();
    QDownloader::MainWindow wid;
    wid.show();
    return a.exec();
}
