#include "additemdialog.h"
#include "ui_additemdialog.h"

using namespace QDownloader;

AddItemDialog::AddItemDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AddItemDialog)
{
    ui->setupUi(this);
}

AddItemDialog::~AddItemDialog()
{
    delete ui;
}

QString AddItemDialog::urlString()
{
    return ui->urlLineEdit->text();
}
