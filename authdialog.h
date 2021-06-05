#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include <QDialog>
#include <QDebug>

namespace Ui {
class AuthDialog;
}

class AuthDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuthDialog(QString*, QString*, QWidget *parent = nullptr);
    ~AuthDialog();

private slots:
    void on_pushButton_auth_clicked();

private:
    Ui::AuthDialog *ui;

    QString *username;
    QString *password;
};

#endif // AUTHDIALOG_H
