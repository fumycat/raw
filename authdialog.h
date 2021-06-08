#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include "raw_global.h"

#include <QDialog>
#include <QLabel>

namespace Ui {
class AuthDialog;
}

class AuthDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuthDialog(QString*, QWidget *parent = nullptr);
    ~AuthDialog();
    static void lolcat(QLabel*, QString = nullptr, QString = "black");

private slots:
    void on_pushButton_auth_clicked();

private:
    Ui::AuthDialog *ui;

    QString *token;
};

#endif // AUTHDIALOG_H
