 #include "item.h"

#include "downloadmanager.h"

using namespace QDownloader;


Item::ItemInfo::ItemInfo()
{
    bytesReceivedAtBegin = 0;
    speedLimit = 1024*200;
    duration = 0;
    progress = 0;
    state = NotStarted;
}


Item::ItemInfo::ItemInfo(const ItemInfo & itemInfo)
{
    url                  = itemInfo.url;
    redirectUrl          = itemInfo.redirectUrl;
    filename             = itemInfo.filename;
    bytesReceivedAtBegin = itemInfo.bytesReceivedAtBegin;
    speedLimit           = itemInfo.speedLimit;
    duration             = itemInfo.duration;
    progress             = itemInfo.progress;
    state             = itemInfo.state;
}

QDataStream& operator<<(QDataStream& out, const QDownloader::Item::ItemInfo& v) {


    out << v.url << v.redirectUrl
        << v.filename << v.bytesReceivedAtBegin
        << v.speedLimit << v.duration
        << v.progress << v.state;

    return out;
}

QDataStream& operator>>(QDataStream& in, QDownloader::Item::ItemInfo& v) {

    int stateInt;
    in >> v.url >> v.redirectUrl
        >> v.filename >> v.bytesReceivedAtBegin
        >> v.speedLimit >> v.duration
        >> v.progress >> stateInt;
    v.state = (Item::State)stateInt;
    return in;
}


Item::Item(QUrl url)
{
    _info.redirectUrl = _info.url = url;
    _reply = 0;
    _bytesReceived = 0;
    _currentInstantSpeed.duration = 0;
    _currentInstantSpeed.bytes = 0;
    _file.setFileName(filename());
}

Item::Item(const ItemInfo &info) : Item(QUrl())
{
    _info = info;
    _file.setFileName(filename());
}

void Item::saveInfo()
{
    QSettings settings;
    settings.beginGroup("Downloads");
    settings.setValue(QFileInfo(filename()).absoluteFilePath(), QVariant::fromValue<ItemInfo>(info()));
    qDebug()<<"save state";
}

QString Item::filename()
{
    if(_info.filename.isEmpty())
    {
        _info.filename = _info.url.fileName();
        if(_reply)
        {
            if(_info.filename.isEmpty())
            {
                QString nameHeader = _reply->header(QNetworkRequest::ContentDispositionHeader).toString();
                QRegExp extractNameHeader("filename=[\"]{0,1}(.*)[\"]{0,1}");
                qDebug()<<nameHeader;
                if(extractNameHeader.indexIn(nameHeader) != -1)
                {
                    _info.filename = extractNameHeader.capturedTexts().at(1);
                }
            }
        }
        QRegExp extractNameUrl("([a-z\\.\\-_0-9]+)[^a-z\\.\\-_0-9]*$", Qt::CaseInsensitive);
        if(extractNameUrl.indexIn(QUrl::fromPercentEncoding(_info.url.toEncoded())) != -1)
        {
            _info.filename = extractNameUrl.capturedTexts().at(1);
        }

        if(_info.filename.isEmpty())
        {
            _info.filename = "untitled_download";
        }
        if(QFileInfo(_info.filename).exists())
        {
            qDebug()<<"file exists => overwriting";
        }
    }

    return _info.filename;
}

qreal Item::progress()
{
    return _info.progress;
}

qint64 Item::elapsed()
{
    if(isPaused())
    {
        return _info.duration;
    }
    return _info.duration + _durationTimer.elapsed();
}

/*!
 * return the download speed in bytes per second
 */
qreal Item::downloadSpeed()
{
    if(_currentInstantSpeed.duration <= 0)
    {
        return 0;
    }
    return _currentInstantSpeed.bytes / qreal(_currentInstantSpeed.duration) * 1000.0;
}

void Item::setSpeedLimit(qint64 bytesBySecond)
{
    _info.speedLimit = bytesBySecond;
    if(_reply)
    {
        _reply->setReadBufferSize(2 * _info.speedLimit);
    }
}

void Item::start()
{
    _reply = DownloadManager::instance()->download(_info.redirectUrl);
    _reply->setReadBufferSize(2 * _info.speedLimit);
    if(!_file.open(QFile::WriteOnly))
    {
        qDebug()<<"unable to open file "<<filename();
    }

    connect(_reply,SIGNAL(finished()),this,SLOT(finished()));
    connect(_reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
    connect(_reply,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
    connect(_reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));
    _elapsedTimer.start();
    _durationTimer.start();
}

void Item::pause()
{
    qDebug() << "pause()";
    if( _reply == 0 ) {
        return;
    }
    disconnect(_reply,SIGNAL(finished()),this,SLOT(finished()));
    disconnect(_reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
    disconnect(_reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));
    disconnect(_reply,SIGNAL(readyRead()),this,SLOT(onReadyRead()));

    _file.write(_reply->readAll());
    _reply->abort();
    _file.close();
    _reply = 0;
    _info.bytesReceivedAtBegin += _bytesReceived;
    _bytesReceived = 0;
    _info.duration += _durationTimer.elapsed();
}

void Item::resume()
{
    if(!_file.open(QFile::WriteOnly | QFile::Append))
    {
        qDebug()<<"unable to open file "<<filename();
    }
    qDebug() << "resume()";

    quint64 size = _file.size();

    _reply = DownloadManager::instance()->resume(_info.redirectUrl, size);
    _reply->setReadBufferSize(2 * _info.speedLimit);
    connect(_reply,SIGNAL(finished()),this,SLOT(finished()));
    connect(_reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
    connect(_reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));
    connect(_reply,SIGNAL(readyRead()),this,SLOT(onReadyRead()));
    _currentInstantSpeed.duration = 0;
    _currentInstantSpeed.bytes = 0;
    _elapsedTimer.start();
    _durationTimer.start();
}

void Item::finished()
{
    if(_reply->bytesAvailable()) {
        _file.write(_reply->readAll());
    }
    _file.close();
    _info.duration += _durationTimer.elapsed();


    QVariant possibleRedirectUrl = _reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if(possibleRedirectUrl.isValid())
    {
        _info.redirectUrl = possibleRedirectUrl.toUrl();
        qDebug()<<"redirected";
        start();
        return;
    }
    _reply = 0;
    qDebug()<<"finished";
}


void Item::onReadyRead()
{
    if(!_reply)
    {
        return;
    }
    qint64 elapsed = _elapsedTimer.elapsed();
    _elapsedTimer.start();

    _currentInstantSpeed.duration += elapsed;

    while(_currentInstantSpeed.duration > 10000 && _speedList.size())
    {
        _currentInstantSpeed.duration -= _speedList.front().duration;
        _currentInstantSpeed.bytes -= _speedList.front().bytes;
        _speedList.pop_front();
    }

    qint64 availableBytes = _reply->bytesAvailable();
    qint64 readSize = availableBytes;

    if(_currentInstantSpeed.duration > 0 && _info.speedLimit > 0)
    {
        readSize = _currentInstantSpeed.duration * _info.speedLimit / 1000.0 - _currentInstantSpeed.bytes;
        if(readSize < 0)
            readSize = 0;
        if(readSize > availableBytes)
            readSize = availableBytes;
    }


    InstantSpeed speed;
    speed.duration = elapsed;
    speed.bytes = readSize;
    _speedList.append(speed);
    _currentInstantSpeed.bytes += speed.bytes;


    if(readSize > 0 && availableBytes) {
        _file.write(_reply->read(readSize));
    }
    //qDebug()<<"readSize "<<readSize<<" remaining "<<_reply->bytesAvailable()<<" filesize "<<(_file.size()/1024.0);

    if(_reply->bytesAvailable())
    {
        QTimer::singleShot(500, this, SLOT(onReadyRead()));
    }
    emit dataChanged(); // TODO: replace by a timer every second;
}

void Item::downloadProgress ( qint64 bytesReceived, qint64 bytesTotal )
{
    _bytesReceived = bytesReceived;
    if(bytesTotal > 0)
    {
        _info.progress = (_info.bytesReceivedAtBegin  + bytesReceived) / qreal(_info.bytesReceivedAtBegin + bytesTotal);
    }
    emit dataChanged();
}

void Item::error ( QNetworkReply::NetworkError code )
{
    _file.close();
    qDebug()<<"error: "<<code<<" "<<_reply->errorString();
}
