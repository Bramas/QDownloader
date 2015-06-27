#include "itemdelegate.h"
#include <QPainter>
#include <QDebug>
#include <QAbstractItemView>
#include <QStyleOptionProgressBar>
#include <QApplication>

#include "item.h"
using namespace QDownloader;

ItemDelegate::ItemDelegate()
{

}


void ItemDelegate::emitSizeHintChanged(const QModelIndex &index)
{
    emit sizeHintChanged(index);
}

void ItemDelegate::currentChanged(const QModelIndex &current, const QModelIndex &previous)
{
    emit sizeHintChanged(current);
    emit sizeHintChanged(previous);
}

QSize ItemDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    return QSize(200,50);
}

void ItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Item * item = index.data().value<ItemPtr>().item();
    QFontMetrics fm(option.font);
    QColor backgroundColor;
    QColor textColor;

    const QAbstractItemView * view = qobject_cast<const QAbstractItemView *>(option.widget);
    bool selected = view->selectionModel()->currentIndex() == index;
    if (selected) {
        painter->setBrush(option.palette.highlight().color());
        backgroundColor = option.palette.highlight().color();
    } else {
        painter->setBrush(option.palette.background().color());
        backgroundColor = option.palette.background().color();
    }
    painter->setPen(Qt::NoPen);
    painter->drawRect(option.rect);

    // Set Text Color
    if (selected)
        textColor = option.palette.highlightedText().color();
    else
        textColor = option.palette.text().color();

    painter->setPen(textColor);
    painter->drawText(option.rect, item->url().toString());


    painter->drawText(QRect(option.rect.right()-200,option.rect.top(),100,20), QString("%1 Ko/s").arg(QString::number(item->downloadSpeed() / 1024.0, 'f', 2)));

    painter->drawText(QRect(option.rect.right()-250,option.rect.top(),50,20), QString("%1s").arg(QString::number(item->elapsed() / 1000)));

    QStyleOptionProgressBar progressOptions;

    progressOptions.rect = QRect(option.rect.right()-100,option.rect.top(),100,20);
    progressOptions.minimum = 0;
    progressOptions.maximum = 100;
    progressOptions.progress = item->progress() * 100;
    progressOptions.text = QString("%1%").arg(int(item->progress() * 100));
    progressOptions.textVisible = true;

    QApplication::style()->drawControl(QStyle::CE_ProgressBar, &progressOptions, painter, 0);
}
