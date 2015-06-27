#ifndef AUTHENTICATEDIALOG_H
#define AUTHENTICATEDIALOG_H

#include <QDialog>

class QAuthenticator;

namespace Ui {
class AuthenticateDialog;
}


namespace QDownloader
{


class AuthenticateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuthenticateDialog(QWidget *parent, QAuthenticator * authenticator);
    ~AuthenticateDialog();

public slots:
    void accept();

private:
    Ui::AuthenticateDialog *ui;
    QAuthenticator * _authenticator;
};

}
#endif // AUTHENTICATEDIALOG_H
