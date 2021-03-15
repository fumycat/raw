#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onfinish(QNetworkReply*)));

    //postData.append("{\"p1\": \"" + ui->lineEdit->text() + "\"}");
    //postData.append("{\"p2\": \"" + ui->lineEdit_2->text() + "\"}");
    //postData.append("{\"p3\": \"" + ui->lineEdit_3->text() + "\"}");

    //QByteArray postData;
    //postData.append("?p1=" + ui->lineEdit->text() + "&");
    //postData.append("p2=" + ui->lineEdit_2->text() + "&");
    //postData.append("p3=" + ui->lineEdit_3->text());

    QUrl xurl("http://localhost");
    xurl.setPort(6677);

    QNetworkRequest request(xurl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject obj;
    obj["p1"] = ui->lineEdit->text();
    obj["p2"] = ui->lineEdit_2->text();
    obj["p3"] = ui->lineEdit_3->text();
    QJsonDocument doc(obj);
    QByteArray postData = doc.toJson();


    qDebug() << request.url().toString();
    const QList<QByteArray>& rawHeaderList(request.rawHeaderList());
    foreach (QByteArray rawHeader, rawHeaderList) {
      qDebug() << request.rawHeader(rawHeader);
    }
    qDebug() << postData;

    manager->post(request, postData);
}

void MainWindow::onfinish(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
        {
            ui->label->setText("Error: " +  reply->errorString());
        }
        else
        {
            QByteArray responseData = reply->readAll();
            QString qstr(responseData);
            ui->label->setText(qstr);
        }
}
