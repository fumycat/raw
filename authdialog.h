#ifndef AUTHDIALOG_H
#define AUTHDIALOG_H

#include "raw_global.h"

#include <QDialog>
#include <QtNetwork>
#include <QDebug>
#include <QLabel>
#include <QTimer>

namespace Ui {
class AuthDialog;
}

class AuthDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AuthDialog(QString*, QWidget *parent = nullptr);
    ~AuthDialog();
    static void set_label_text_color(QLabel*, QString, QString = "black");

private slots:
    void on_pushButton_auth_clicked();

private:
    Ui::AuthDialog *ui;

    QString *token;
};

#endif // AUTHDIALOG_H
