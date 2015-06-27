#ifndef DOWNLOADMANAGER_H
#define DOWNLOADMANAGER_H

#include "item.h"

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>

namespace QDownloader {

class DownloadManager : public QObject
{
    Q_OBJECT
public:
    static DownloadManager * instance();
    static void init();

    QNetworkReply *download(QUrl url);
    QNetworkReply *resume(QUrl url, quint64 size);

private slots:
    void onAuthenticationRequired(QNetworkReply * reply, QAuthenticator * authenticator);

private:

    explicit DownloadManager();
    static DownloadManager * _instance;

    QNetworkAccessManager* mManager;
    QNetworkRequest mCurrentRequest;
    QNetworkReply* mCurrentReply;
    QFile* mFile;
    int mDownloadSizeAtPause;
};

}
#endif // DOWNLOADMANAGER_H
