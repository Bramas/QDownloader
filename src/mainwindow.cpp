#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "listmodel.h"
#include "itemdelegate.h"
#include <QStandardItemModel>

using namespace QDownloader;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(ui->verticalLayoutWidget);
    _model = new QDownloader::ItemsModel();
    ui->listView->setModel(_model);
    ui->listView->setItemDelegate(new ItemDelegate());

    connect(ui->addButton, &QPushButton::clicked, [this]() {
        QDownloader::Item * item = new QDownloader::Item(QUrl(ui->addLineEdit->text()));
        _model->addItem(item);
        item->start();
        ui->addLineEdit->clear();
    });

    connect(ui->actionPause, &QAction::triggered, [this](){
        QModelIndex index = ui->listView->selectionModel()->currentIndex();
        if(!index.isValid())
        {
            return;
        }
        index.data().value<QDownloader::ItemPtr>().item()->pause();
    });

    connect(ui->actionResume, &QAction::triggered, [this](){
        QModelIndex index = ui->listView->selectionModel()->currentIndex();
        if(!index.isValid())
        {
            return;
        }
        index.data().value<QDownloader::ItemPtr>().item()->resume();
    });

    connect(ui->actionRemove_from_list, &QAction::triggered, [this](){
        QModelIndex index = ui->listView->selectionModel()->currentIndex();
        if(!index.isValid())
        {
            return;
        }
        _model->remove(index);
    });
}

MainWindow::~MainWindow()
{
    _model->saveItemsInfo();
    delete ui;
}
