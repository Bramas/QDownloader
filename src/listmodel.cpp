#include "listmodel.h"

#include <QDebug>
#include <QSettings>

using namespace QDownloader;

ItemsModel::ItemsModel()
{
    QSettings settings;
    settings.beginGroup("Downloads");
    foreach (const QString &key, settings.allKeys()) {
        Item * item = new Item(settings.value(key).value<Item::ItemInfo>());
        addItem(item);
    }
}


int ItemsModel::rowCount(const QModelIndex &parent) const
{
    return parent.isValid() ? 0 : _items.count();
}

int ItemsModel::columnCount(const QModelIndex &parent) const
{
        return parent.isValid() ? 0 : 1;
}


QModelIndex ItemsModel::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid())
        return QModelIndex();
    return createIndex(row, column);
}

QModelIndex ItemsModel::parent(const QModelIndex &child) const
{
    Q_UNUSED(child)
    return QModelIndex();
}


QVariant ItemsModel::data(const QModelIndex & index, int role) const
{
    return QVariant::fromValue<ItemPtr>(ItemPtr(_items.at(index.row())));
}

bool ItemsModel::setData(const QModelIndex & index, const QVariant & value, int role /*= Qt::EditRole*/)
{
    Item * item = value.value<ItemPtr>().item();
    connect(item, &Item::dataChanged, this, &ItemsModel::onItemChanged);
    _items[index.row()] = item;
    emit dataChanged(index, index);
    return true;
}

void ItemsModel::addItem(Item *item)
{
    beginInsertRows(QModelIndex(), 1, 1);
    connect(item, &Item::dataChanged, this, &ItemsModel::onItemChanged);
    _items << item;
    qDebug()<<"item inserted";
    endInsertRows();
    QSettings settings;
    settings.beginGroup("Downloads");
    settings.setValue(QFileInfo(item->filename()).absoluteFilePath(), QVariant::fromValue<Item::ItemInfo>(item->info()));
}

void ItemsModel::remove(QModelIndex index)
{
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    _items.removeAt(index.row());
    endRemoveRows();
}

void ItemsModel::saveItemsInfo()
{
    foreach(Item * item, _items)
    {
        item->saveInfo();
    }
}

void ItemsModel::onItemChanged()
{
    //qDebug()<<"item changed";
    Item * item = qobject_cast<Item*>(sender());
    if(!item)
    {
        qWarning()<<"slots onItemChanged received by object of non type Item*";
        return;
    }
    int i = _items.indexOf(item);
    //qDebug()<<i;
    QModelIndex idx = index(i, 0, QModelIndex());
    emit dataChanged(idx, idx);
}
