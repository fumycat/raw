#include "authdialog.h"
#include "ui_authdialog.h"

AuthDialog::AuthDialog(QString *username, QString *password, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AuthDialog)
{
    ui->setupUi(this);
    this->username = username;
    this->password = password;
}

AuthDialog::~AuthDialog()
{
    delete ui;
}

void AuthDialog::on_pushButton_auth_clicked()
{
    *this->username = ui->lineEdit_username->text();
    *this->password = ui->lineEdit_password->text();
    QDialog::accept();
}
