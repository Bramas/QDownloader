#ifndef ADDITEMDIALOG_H
#define ADDITEMDIALOG_H

#include <QDialog>

namespace Ui {
class AddItemDialog;
}

namespace QDownloader {

class AddItemDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AddItemDialog(QWidget *parent = 0);
    ~AddItemDialog();

    QString urlString();

private:
    Ui::AddItemDialog *ui;
};

}
#endif // ADDITEMDIALOG_H
