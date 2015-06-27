#include "authenticatedialog.h"
#include "ui_authenticatedialog.h"

#include <QAuthenticator>

using namespace QDownloader;

AuthenticateDialog::AuthenticateDialog(QWidget *parent, QAuthenticator *authenticator) :
    QDialog(parent),
    ui(new Ui::AuthenticateDialog)
{
    _authenticator = authenticator;
    ui->setupUi(this);
}

AuthenticateDialog::~AuthenticateDialog()
{
    delete ui;
}


void AuthenticateDialog::accept()
{
    _authenticator->setUser(ui->userLineEdit->text());
    _authenticator->setPassword(ui->passwordLineEdit->text());
    QDialog::accept();
}
