#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QtNetwork/QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QFileDialog>
#include <QHttpMultiPart>
#include <QVector>

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
    // select 2nd file
    open_matrix_file(1, ui->tableWidget_1);
    /*
    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            this, SLOT(onfinish(QNetworkReply*)));

    QUrl xurl("http://localhost/ajax/");
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
    */
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
            ui->label_4->setText(qstr);
        }
}

void MainWindow::on_pushButton_2_clicked()
{
    // send request




    // TODO MULTIPART/DATA
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart textPart0;
    textPart0.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"text0\""));
    textPart0.setBody(arr[0].toUtf8());

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"text1\""));
    textPart1.setBody(arr[1].toUtf8());

    multiPart->append(textPart0);
    multiPart->append(textPart1);

    // request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");

//    qDebug() << request.url().toString();
//    const QList<QByteArray>& rawHeaderList(request.rawHeaderList());
//    foreach (QByteArray rawHeader, rawHeaderList) {
//      qDebug() << request.rawHeader(rawHeader);
//    }
//    qDebug() << f;

//    QByteArray data = f->readAll();
//    manager->post(request, data);

    QUrl xurl("http://localhost/upload/");
    xurl.setPort(6677);
    QNetworkRequest request(xurl);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onfinish(QNetworkReply*)));
    manager->post(request, multiPart);

    //QNetworkAccessManager manager;
    //QNetworkReply *reply = manager.post(request, multiPart);
    //qDebug() << reply->readAll();

    // multiPart->setParent(reply);
}


void MainWindow::on_pushButton_3_clicked()
{
    // select 1st file
    open_matrix_file(0, ui->tableWidget_0);
}

void MainWindow::open_matrix_file(int arr_id, QTableWidget *wid)
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open text file"), "", tr("Text files (*.txt)"));
    QFile f(fileName);

    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Can't open file";
        return;
    }
    QVector<QVector<QString>> matrix;
    QTextStream in(&f);
    QString xd;
    int x, y;
    in >> x >> y;
    for (int i = 0; i < x; i++) {
        QVector<QString> l;
        for (int j = 0; j < y; j++) {
            in >> xd;
            l.append(xd);
        }
        matrix.append(l);
    }
    put_data_into_widget(matrix, wid);
    /*
    for (int i = 0; i < x; i++) {
        QString test_list = "";
        for (int j = 0; j < y; j++) {
            test_list += matrix[i][j];
        }
        qDebug() << test_list;
    }
    */
    f.seek(0);
    arr[arr_id] = f.readAll();
}

void MainWindow::put_data_into_widget(QVector<QVector<QString>> data, QTableWidget *wid)
{
    wid->setRowCount(4);
    wid->setColumnCount(4);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            wid->setItem(i, j, new QTableWidgetItem(data[i][j]));
        }
    }
    wid->setItem(3, 0, new QTableWidgetItem("..."));
    wid->setItem(0, 3, new QTableWidgetItem("..."));
}
