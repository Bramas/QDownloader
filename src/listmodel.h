#ifndef LISTMODEL_H
#define LISTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <item.h>

namespace QDownloader
{

class ItemsModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    ItemsModel();

    int         rowCount(const QModelIndex & parent = QModelIndex()) const;
    int         columnCount(const QModelIndex & parent = QModelIndex()) const;
    QModelIndex index(int row, int column, const QModelIndex &parent) const;
    QModelIndex parent(const QModelIndex &child) const;
    QVariant	data(const QModelIndex & index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex & index, const QVariant & value, int role = Qt::EditRole);
    void addItem(Item * item);
    void remove(QModelIndex index);

    void saveItemsInfo();

private slots:
    void onItemChanged();

private:

    QList<Item *> _items;

};

}

#endif // LISTMODEL_H
