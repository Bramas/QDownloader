#ifndef DOWNLOADITEM_H
#define DOWNLOADITEM_H

#include <QObject>
#include <QUrl>
#include <QNetworkReply>
#include <QFile>
#include <QPointer>
#include <QLinkedList>
#include <QElapsedTimer>
#include <QDebug>
#include <QFileInfo>
#include <QTimer>


namespace QDownloader {


class Item : public QObject
{
    Q_OBJECT
public:
    explicit Item(QUrl url);
    QUrl url() const { return _url; }
    QString filename();
    qint64 elapsed();
    qreal progress();
    qreal downloadSpeed();
    void setSpeedLimit(qint64 bytesBySecond);
    bool isPaused() { return !_reply; }

signals:
    void dataChanged();
public slots:
    void start();
    void pause();
    void resume();


private slots:

    void finished();
    void downloadProgress ( qint64 bytesReceived, qint64 bytesTotal );
    void onReadyRead();
    void error ( QNetworkReply::NetworkError code );

private:
    QUrl _url;
    QUrl _redirectUrl;
    QNetworkReply * _reply;
    QFile _file;
    QString _filename;
    qreal _progress;
    qint64 _bytesReceivedAtBegin;
    qint64 _bytesReceived;
    qint64 _speedLimit;

    struct InstantSpeed
    {
        qint64 duration;
        qint64 bytes;
    };

    QLinkedList<InstantSpeed> _speedList;
    QElapsedTimer _elapsedTimer;
    InstantSpeed _currentInstantSpeed;
    qint64 _duration;
    QElapsedTimer _durationTimer;
};

class ItemPtr : public QObject
{
public:
    ItemPtr() { _item = 0; }
    ItemPtr(const ItemPtr & itemInfo) : QObject(0) { _item = itemInfo._item; }
    ItemPtr(Item * item) { _item = item; }
    Item * item() { return _item; }

private:
    Item * _item;
};


} // namespace QDownloader

Q_DECLARE_METATYPE(QDownloader::ItemPtr)

#endif // DOWNLOADITEM_H
