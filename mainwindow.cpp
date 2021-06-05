#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    for (int i = 0; i < 2; i++) {
        files_selected[i] = false;
    }

    ui->lineEdit->setText("C:\\Users\\login\\Documents\\x.txt");

    QString username;
    QString password;
    AuthDialog *d = new AuthDialog(&username, &password, this);
    d->exec();

    if (d->result() == QDialog::Rejected) {
        qDebug() << "rejected";
        QTimer::singleShot(0, this, SLOT(close()));
    } else if (d->result() == QDialog::Accepted) {
        qDebug() << "accepted" << username << password;
    }


    /*
    float xt1 = 320.54;
    QFile f("C:\\Users\\login\\Documents\\bin.bin");
    f.open(QIODevice::WriteOnly);
    QDataStream out(&f);
    out.setFloatingPointPrecision(QDataStream::SinglePrecision);
    for (int i = 0; i < 1024; i++) {
        out << xt1;
    }
    f.close();
    */
//    f.open(QIODevice::ReadOnly);
//    QDataStream in(&f);
//    float xo;
//    for (int i = 0; i < 8; i++) {
//        in >> xo;
//        qDebug() << xo;
//    }

}

MainWindow::~MainWindow()
{
    delete ui;
}

int MainWindow::prepare_file(QString filename, QVector<float>* data)
{
    float f_tmp;
    /*
    QTemporaryFile t;
    t.open();
    qDebug() << t.fileName();
    t.setAutoRemove(false);
    */
    QFile f(filename);
    f.open(QIODevice::WriteOnly);
    int x = 0;
    int y = -1;
    QTextStream stream(f.readAll());
    QString line;
    while (stream.readLineInto(&line)) {
        x++;
        QStringList xl = line.split(" ");
        if (y == -1) {
            y = xl.length();
        } else {
            if (y != xl.length()) {
                return -1;
            }
        }
        for (const auto& s : qAsConst(xl)) {
            f_tmp = s.toFloat();
            data->append(f_tmp);
        }
    }
    return 0;
}

void MainWindow::test_upload()
{
    QJsonObject j;
    QFile f(ui->lineEdit->text());
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Can't open file"; // TODO
        return;
    }
    j["data"] = QString::fromUtf8(f.readAll());

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl(QString(DEFAULT_URL) + "upload"));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QByteArray data = QJsonDocument(j).toJson();
    ui->statusbar->showMessage("posting...");
    QNetworkReply *reply = mgr->post(request, data);
    ui->statusbar->showMessage("sent?");

    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        if(reply->error() == QNetworkReply::NoError){
            QByteArray data = reply->readAll();
            QJsonDocument qjd = QJsonDocument::fromJson(data);
            QJsonObject qjo = qjd.object();
            qDebug() << qjo;
            if (qjo.contains("status")) {
                ui->statusbar->showMessage(qjo.value("status").toString());
            }
        }
        else{
            QString err = reply->errorString();
            qDebug() << "ERROR" << err;
            ui->statusbar->showMessage("Unknown error"); // maybe show messagebox?
        }
        reply->deleteLater();
    });
}

void MainWindow::on_pushButton_clicked()
{
    // select 2nd file
    open_matrix_file(1, ui->tableWidget_1);
}

void MainWindow::onfinish_test(QNetworkReply *reply)
{
    if(reply->error() != QNetworkReply::NoError)
    {
        qDebug() << "finish_test_err " << reply->errorString();
    } else
    {
        qDebug() << "finish_test " << reply->readAll();
    }
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
    if ((!files_selected[0]) || (!files_selected[1])) {
        QMessageBox::warning(this, tr("Ошибка"),
                                       tr("Нужно выбрать два файла."),
                                       QMessageBox::Ok);
        return;
    }
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
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open text file"), "", tr("Text files (*.txt, *)"));
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Can't open file";
        return;
    }
    fnames[arr_id] = fileName;
    files_selected[arr_id] = true;

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

void MainWindow::on_pushButton_4_clicked()
{
    // test button
    /*
    if ((!files_selected[0]) || (!files_selected[1])) {
        QMessageBox::warning(this, tr("Ошибка"),
                                       tr("Нужно выбрать два файла."),
                                       QMessageBox::Ok);
        return;
    }
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    const int parts = 2;
    QHttpPart qpart[parts];
    for (int i = 0; i< parts; ++i) {
        qpart[i].setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/octet-stream"));
        qpart[i].setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(QString("form-data; name=\"file%1\"").arg(i)));
        QFile *file = new QFile(fnames[i]);
        file->open(QIODevice::ReadOnly);
        qpart[i].setBodyDevice(file);
        multiPart->append(qpart[i]);
    }
    QUrl xurl("http://localhost/t/");
    xurl.setPort(6677);
    QNetworkRequest request(xurl);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onfinish_test(QNetworkReply*)));
    manager->post(request, multiPart);
    */
    test_upload();
}

void MainWindow::on_pushButton_5_clicked()
{
    QVector<float>* v = new QVector<float>;
    int status = prepare_file(ui->lineEdit->text(), v);
    qDebug() << "prep_file status" << status;
    qDebug() << "vector size" << v->size();

    delete v;
}
