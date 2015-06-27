#include "downloadmanager.h"

#include "authenticatedialog.h"

#include <QDebug>
#include <QAuthenticator>

using namespace QDownloader;

DownloadManager * DownloadManager::_instance;


void DownloadManager::init()
{
    _instance = new DownloadManager();
}

DownloadManager * DownloadManager::instance()
{
    return _instance;
}

DownloadManager::DownloadManager() :
    mCurrentReply(0),mFile(0),mDownloadSizeAtPause(0)
{
    mManager = new QNetworkAccessManager( this );
    connect(mManager, &QNetworkAccessManager::authenticationRequired, this, &DownloadManager::onAuthenticationRequired);
}

QNetworkReply* DownloadManager::download(QUrl url)
{
    qDebug() << "download: URL=" <<url.toString();

    mDownloadSizeAtPause = 0;
    mCurrentRequest = QNetworkRequest(url);
    return mManager->get(mCurrentRequest);
}


QNetworkReply* DownloadManager::resume(QUrl url, quint64 size)
{
    QByteArray rangeHeaderValue = "bytes=" + QByteArray::number(size) + "-";

    mCurrentRequest = QNetworkRequest(url);
    mCurrentRequest.setRawHeader("Range",rangeHeaderValue);

    return mManager->get(mCurrentRequest);
}

void DownloadManager::onAuthenticationRequired(QNetworkReply * reply, QAuthenticator * authenticator)
{
    qDebug()<<"Auth required ";
    AuthenticateDialog dialog(0, authenticator);
    dialog.exec();
}

