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
            // ui->label_4->setText(qstr);
            qDebug() << qstr;
            QString fileName = QFileDialog::getSaveFileName(this,
                    tr("Save Output Matrix"), "",
                    tr("All Files (*)"));
            QFile file(fileName);
            if (!file.open(QIODevice::WriteOnly)) {
                        QMessageBox::information(this, tr("Unable to open file"),
                            file.errorString());
                        return;
                    }
            file.write(qstr.toUtf8());


            QVector<QVector<QString>> matrix;
            QTextStream in(qstr.toUtf8());
            QString xd;
            int n;
            in >> n;
            for (int i = 0; i < n; i++) {
                QVector<QString> l;
                for (int j = 0; j < n; j++) {
                    in >> xd;
                    l.append(xd);
                }
                matrix.append(l);
            }
            put_data_into_widget(matrix, ui->tableWidget_Res);
        }
}

void MainWindow::on_pushButton_2_clicked()
{
    // send request
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart textPart0;
    textPart0.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"text0\""));
    textPart0.setBody(arr[0].toUtf8());

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"text1\""));
    textPart1.setBody(arr[1].toUtf8());

    multiPart->append(textPart0);
    multiPart->append(textPart1);

    QUrl xurl("http://localhost/upload/");
    xurl.setPort(6677);
    QNetworkRequest request(xurl);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onfinish(QNetworkReply*)));
    manager->post(request, multiPart);
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
    int n;
    in >> n;
    for (int i = 0; i < n; i++) {
        QVector<QString> l;
        for (int j = 0; j < n; j++) {
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
