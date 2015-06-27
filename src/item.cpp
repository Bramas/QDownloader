#include "item.h"

#include "downloadmanager.h"

#include <QDebug>
#include <QFileInfo>

using namespace QDownloader;

Item::Item(QUrl url)
{
    _redirectUrl = _url = url;
    _reply = 0;
    _progress = 0;
    _bytesReceived = 0;
    _bytesReceivedAtBegin = 0;
    _currentInstantSpeed.duration = 0;
    _currentInstantSpeed.bytes = 0;
}

QString Item::filename()
{
    if(_filename.isEmpty())
    {
        if(_reply)
        {
            _filename = _reply->url().fileName();
            if(_filename.isEmpty())
            {
                QString nameHeader = _reply->header(QNetworkRequest::ContentDispositionHeader).toString();
                QRegExp extractNameHeader("filename=[\"]{0,1}(.*)[\"]{0,1}");
                qDebug()<<nameHeader;
                if(extractNameHeader.indexIn(nameHeader) != -1)
                {
                    _filename = extractNameHeader.capturedTexts().at(1);
                }
                QRegExp extractNameUrl("([a-z\\.\\-_0-9]+)[^a-z\\.\\-_0-9]*$", Qt::CaseInsensitive);
                if(extractNameUrl.indexIn(QUrl::fromPercentEncoding(_url.toEncoded())) != -1)
                {
                    _filename = extractNameUrl.capturedTexts().at(1);
                }

                if(_filename.isEmpty())
                {
                    _filename = "untitled_download";
                }
            }
            if(QFileInfo(_filename).exists())
            {
                qDebug()<<"file exists => overwriting";
            }
        }
    }

    return _filename;
}

qreal Item::progress()
{
    return _progress;
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

void Item::start()
{
    _reply = DownloadManager::instance()->download(_redirectUrl);
    _file.setFileName(filename());
    if(!_file.open(QFile::WriteOnly))
    {
        qDebug()<<"unable to open file "<<filename();
    }

    connect(_reply,SIGNAL(finished()),this,SLOT(finished()));
    connect(_reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
    connect(_reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));
    _elapsedTimer.start();
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

    _file.write(_reply->readAll());
    _reply->abort();
    _file.close();
    _reply = 0;
    _bytesReceivedAtBegin += _bytesReceived;
    _bytesReceived = 0;
}

void Item::resume()
{
    if(!_file.open(QFile::WriteOnly | QFile::Append))
    {
        qDebug()<<"unable to open file "<<filename();
    }
    qDebug() << "resume()";

    quint64 size = _file.size();

    _reply = DownloadManager::instance()->resume(_redirectUrl, size);
    connect(_reply,SIGNAL(finished()),this,SLOT(finished()));
    connect(_reply,SIGNAL(downloadProgress(qint64,qint64)),this,SLOT(downloadProgress(qint64,qint64)));
    connect(_reply,SIGNAL(error(QNetworkReply::NetworkError)),this,SLOT(error(QNetworkReply::NetworkError)));
    _currentInstantSpeed.duration = 0;
    _currentInstantSpeed.bytes = 0;
    _elapsedTimer.start();
}

void Item::finished()
{
    if(_reply->bytesAvailable()) {
        _file.write(_reply->readAll());
    }
    _file.close();


    QVariant possibleRedirectUrl = _reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if(possibleRedirectUrl.isValid())
    {
        _redirectUrl = possibleRedirectUrl.toUrl();
        qDebug()<<"redirected";
        start();
        return;
    }
    qDebug()<<"finished";
}

void Item::downloadProgress ( qint64 bytesReceived, qint64 bytesTotal )
{
    if(_reply->bytesAvailable()) {
        _file.write(_reply->readAll());
    }

    InstantSpeed speed;
    speed.duration = _elapsedTimer.elapsed();
    speed.bytes = bytesReceived - _bytesReceived;
    _speedList.append(speed);
    _elapsedTimer.start();

    _currentInstantSpeed.duration += speed.duration;
    _currentInstantSpeed.bytes += speed.bytes;

    while(_currentInstantSpeed.duration > 10000 && _speedList.size())
    {
        _currentInstantSpeed.duration -= _speedList.front().duration;
        _currentInstantSpeed.bytes -= _speedList.front().bytes;
        _speedList.pop_front();
    }



    _bytesReceived = bytesReceived;
    if(bytesTotal > 0)
    {
        _progress = (_bytesReceivedAtBegin  + bytesReceived) / qreal(_bytesReceivedAtBegin + bytesTotal);
    }


    emit dataChanged();
}

void Item::error ( QNetworkReply::NetworkError code )
{
    _file.close();
    qDebug()<<"error: "<<code<<" "<<_reply->errorString();
}
