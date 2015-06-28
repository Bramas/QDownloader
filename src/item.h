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
#include <QSettings>


namespace QDownloader {



class Item : public QObject
{
    Q_OBJECT
public:
    enum State { NotStarted, Paused, Running, Finished, Error };
    struct InstantSpeed
    {
        qint64 duration;
        qint64 bytes;
    };
    class ItemInfo
    {
    public:
        ItemInfo();
        ItemInfo(const ItemInfo & itemInfo);

        QUrl url;
        QUrl redirectUrl;
        QString filename;
        qint64 bytesReceivedAtBegin;
        qint64 speedLimit;
        qint64 duration;
        qreal  progress;
        Item::State state;
    };

    explicit Item(QUrl url);
    explicit Item(const ItemInfo &info);
    void saveInfo();
    QUrl url() const { return _info.url; }
    QString filename();
    qint64 elapsed();
    qreal progress();
    qreal downloadSpeed();
    void setSpeedLimit(qint64 bytesBySecond);
    bool isPaused() { return !_reply; }


    const ItemInfo & info() { return _info; }

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

    ItemInfo _info;

    QNetworkReply * _reply;
    QFile _file;
    qint64 _bytesReceived;

    QLinkedList<InstantSpeed> _speedList;
    QElapsedTimer _elapsedTimer;
    InstantSpeed _currentInstantSpeed;
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
Q_DECLARE_METATYPE(QDownloader::Item::ItemInfo)

QDataStream& operator<<(QDataStream& out, const QDownloader::Item::ItemInfo& v);
QDataStream& operator>>(QDataStream& in, QDownloader::Item::ItemInfo& v);

#endif // DOWNLOADITEM_H
