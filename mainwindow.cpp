#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    server_url = DEFAULT_URL;

    // Validator init
    args_validator = new QDoubleValidator(this);
    args_validator->setLocale(QLocale::C);

    auto edits_for_valid = findChildren<QLineEdit*>(QRegularExpression("^(label).*(alpha|beta)$"));
    foreach (auto line_e, edits_for_valid) {
        line_e->setValidator(args_validator);
    }

    table[0] = ui->tableWidget_tab1_A;
    table[1] = ui->tableWidget_tab1_B;
    table[2] = ui->tableWidget_tab1_C;
    table[3] = ui->tableWidget_tab2_A;
    table[4] = ui->tableWidget_tab2_x;
    table[5] = ui->tableWidget_tab2_y;

    label_dim[0] = ui->label_tab1_A_dim;
    label_dim[1] = ui->label_tab1_B_dim;
    label_dim[2] = ui->label_tab1_C_dim;
    label_dim[3] = ui->label_tab2_A_dim;
    label_dim[4] = ui->label_tab2_x_dim;
    label_dim[5] = ui->label_tab2_y_dim;

    go_btn[0] = ui->btn_1_go;
    go_btn[1] = ui->btn_2_go;

    for (const auto &la : label_dim) {
        la->setText("");
    }

    for (int i = 0; i < 6; i++) {
        file_ok[i] = false;
    }

#ifdef DEBUG_TOKEN
    qDebug() << "DEBUG_TOKEN is ON";
    token = DEBUG_TOKEN;
#else // DEBUG_TOKEN
    AuthDialog *d = new AuthDialog(&token, this);
    d->exec();

    if (d->result() == QDialog::Rejected) {
        qDebug() << "rejected";
        QTimer::singleShot(0, this, SLOT(close()));
    }
#endif // DEBUG_TOKEN

    // Menu
    ui->actionShow_scrollbars->setChecked(true);
    connect(ui->actionShow_scrollbars, &QAction::toggled, this, [=](bool checked) {
        toggle_scrollbars(checked, ui->tabWidget->currentIndex());
    });
    connect(ui->actionShow_headers, &QAction::toggled, this, [=](bool checked) {
        toggle_headers(checked, ui->tabWidget->currentIndex());
    });
    connect(ui->actionAbout_program, &QAction::triggered, this, [=]() {
        QMessageBox::information(this, tr("About program"), tr("Program\nAuthor: Vladislav Loginov"));
    });


    // PushButtons
    connect(ui->btn_tab1_A, &QPushButton::clicked, this, [=](){ open_matrix_file(0, false, DEBUG_FILE_A); });
    connect(ui->btn_tab1_B, &QPushButton::clicked, this, [=](){ open_matrix_file(1, false, DEBUG_FILE_B); });
    connect(ui->btn_tab1_C, &QPushButton::clicked, this, [=](){ open_matrix_file(2, false, DEBUG_FILE_C); });
    connect(ui->btn_tab2_A, &QPushButton::clicked, this, [=](){ open_matrix_file(3, false, DEBUG_FILE_A2); });
    connect(ui->btn_tab2_x, &QPushButton::clicked, this, [=](){ open_matrix_file(4, true, DEBUG_FILE_x); });
    connect(ui->btn_tab2_y, &QPushButton::clicked, this, [=](){ open_matrix_file(5, true, DEBUG_FILE_y); });

#ifdef DEBUG_AUTO_FILES
    qDebug() << "DEBUG_AUTO_FILES is ON";
    open_matrix_file(0, false, DEBUG_FILE_A);
    open_matrix_file(1, false, DEBUG_FILE_B);
    open_matrix_file(2, false, DEBUG_FILE_C);
    open_matrix_file(3, false, DEBUG_FILE_A2);
    open_matrix_file(4, true, DEBUG_FILE_x);
    open_matrix_file(5, true, DEBUG_FILE_y);
#endif // DEBUG_AUTO_FILES

    connect(ui->btn_1_go, &QPushButton::clicked, this, [=](){ go_send(0); });
    connect(ui->btn_2_go, &QPushButton::clicked, this, [=](){ go_send(1); });

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::go_send(int tab_index)
{
    if (!file_ok[0 + 3*tab_index] || !file_ok[1 + 3*tab_index] || !file_ok[2 + 3*tab_index]) {
        QMessageBox::warning(this, tr("Input error"), tr("One or more files are not selected."));
        return;
    }
    bool bad_ab_flag = false;
    switch (tab_index) {
    case 0:
        if (ui->lineEdit_tab1_alpha->text().isEmpty() || ui->lineEdit_tab1_beta->text().isEmpty()) {
            bad_ab_flag = true;
        }
        break;
    case 1:
        if (ui->lineEdit_tab2_alpha->text().isEmpty() || ui->lineEdit_tab2_beta->text().isEmpty()) {
            bad_ab_flag = true;
        }
        break;
    }
    if (bad_ab_flag) {
        QMessageBox::warning(this, tr("Input error"), tr("Enter alpha and beta values."));
        return;
    }
    bool save_C = tab_index == 0 && ui->checkBox_tab1->isChecked();
    bool save_y = tab_index == 1 && ui->checkBox_tab2->isChecked();

    go_btn[tab_index]->setEnabled(false);

    QJsonObject j;
    j["token"] = token;
    switch (tab_index) {
    case 0:
        j["A"] = data[0];
        j["B"] = data[1];
        j["C"] = data[2];
        j["alpha"] = ui->lineEdit_tab1_alpha->text();
        j["beta"] = ui->lineEdit_tab1_beta->text();
        break;
    case 1:
        j["A"] = data[3];
        j["x"] = data[4];
        j["y"] = data[5];
        j["alpha"] = ui->lineEdit_tab2_alpha->text();
        j["beta"] = ui->lineEdit_tab2_beta->text();
        break;
    }
    QByteArray data = QJsonDocument(j).toJson();

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl(QString(server_url) + (tab_index ? GEMV_PATH : GEMM_PATH)));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = mgr->post(request, data);


    QObject::connect(reply, &QNetworkReply::finished, this, [=](){
        if(reply->error() == QNetworkReply::NoError){
            QByteArray data = reply->readAll();
            QJsonDocument qjd = QJsonDocument::fromJson(data);
            QJsonObject qjo = qjd.object();
            qDebug() << qjo;
            if (qjo.contains("status")) {
                if (qjo.value("status").toString() == "Error") {
                    ui->statusbar->showMessage(qjo.value("message").toString());
                }
                else {
                    // BIG TODO
                    qDebug() << "success";
                }
            }
        }
        else{
            QString err = reply->errorString();
            qDebug() << "ERROR" << err;
            ui->statusbar->showMessage(tr("Unknown error"));
            QMessageBox::critical(this, tr("Server error"), err);
        }
        reply->deleteLater();
        go_btn[tab_index]->setEnabled(true);
    });
}

void MainWindow::test_upload()
{
    QJsonObject j;
    QFile f("C:\\as.txt");
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Can't open file"; // TODO
        return;
    }
    j["data"] = QString::fromUtf8(f.readAll());

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl(QString(server_url) + "upload"));
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
            qDebug() << reply->errorString();
        }
        else
        {
            QByteArray responseData = reply->readAll();
            QString qstr(responseData);
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
            // put_data_into_widget(matrix, ui->tableWidget_Res);
        }
}

// TODO MULTIPART
/*
{
    // send request
    if ((!file_ok[0]) || (!file_ok[1])) {
        QMessageBox::warning(this, tr("Ошибка"),
                                       tr("Нужно выбрать два файла."),
                                       QMessageBox::Ok);
        return;
    }
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);
    QHttpPart textPart0;
    textPart0.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"text0\""));
    // textPart0.setBody(arr[0].toUtf8());

    QHttpPart textPart1;
    textPart1.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant("form-data; name=\"text1\""));
    // textPart1.setBody(arr[1].toUtf8());

    multiPart->append(textPart0);
    multiPart->append(textPart1);

    QUrl xurl("http://localhost/upload/");
    xurl.setPort(6677);
    QNetworkRequest request(xurl);

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(onfinish(QNetworkReply*)));
    manager->post(request, multiPart);
}
*/

void MainWindow::open_matrix_file(int arr_id, bool is_vec, QString debug_filename)
{
    QString fileName = debug_filename == nullptr ?
                QFileDialog::getOpenFileName(this, tr("Open text file"), "", tr("Text files (*.txt, *)")) : debug_filename;

    if (fileName.isEmpty()) {
        return;
    }
    QFile f(fileName);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "Can't open file";
        return;
    }

    data[arr_id] = f.readAll();

    // for preview
    QVector<QVector<QString>> matrix;
    int _row = 0;
    int _col = -1;
    QString line;
    QTextStream stream(&data[arr_id]);
    while (stream.readLineInto(&line)) {
        _row++;
        QStringList xl = line.split(" ");
        if (_col == -1) {
            _col = xl.length();
        } else {
            if (_col != xl.length()) {
                QMessageBox::warning(this, tr("Bad file"), tr("No valid matrix in file"));
                return;
            }
        }
        QVector<QString> l;
        foreach (QString e, xl) {
            l.push_back(e);
        }
        matrix.push_back(l);
    }

    // check vectors
    if (is_vec && _row != 1) {
        QMessageBox::warning(this, tr("Bad file"), tr("Vectors must have exactly 1 row"));
        return;
    }

    // check other dims
    switch (arr_id) {
        case 0:
        {
            if ((dim[1].first != 0 && _col != dim[1].first) || (dim[2].first != 0 && _row != dim[2].first)) {
                dim_err_helper(ui->label_tab1_A);
                return;
            }
            else {
                AuthDialog::lolcat(ui->label_tab1_A);
            }
            break;
        }
        case 1:
        {
            if ((dim[0].second != 0 && _row != dim[0].second) || (dim[2].second != 0 && _col != dim[2].second)) {
                dim_err_helper(ui->label_tab1_B);
                return;
            }
            else {
                AuthDialog::lolcat(ui->label_tab1_B);
            }
            break;
        }
        case 2:
        {
            if ((dim[0].first != 0 && _row != dim[0].first) || (dim[1].second != 0 && _col != dim[1].second)) {
                dim_err_helper(ui->label_tab1_C);
                return;
            }
            else {
                AuthDialog::lolcat(ui->label_tab1_C);
            }
            break;
        }
        case 3:
        {
            if ((dim[4].second && _col != dim[4].second) || (dim[5].second && _row != dim[5].second)) {
                dim_err_helper(ui->label_tab2_A);
                return;
            }
            else {
                AuthDialog::lolcat(ui->label_tab2_A);
            }
            break;
        }
        case 4:
        {
            if (dim[3].second && _col != dim[3].second) {
                dim_err_helper(ui->label_tab2_x);
                return;
            }
            else {
                AuthDialog::lolcat(ui->label_tab2_x);
            }
            break;
        }
        case 5:
        {
            if (dim[3].first && _col != dim[3].first) {
                dim_err_helper(ui->label_tab2_y);
                return;
            }
            else {
                AuthDialog::lolcat(ui->label_tab2_y);
            }
            break;
        }
    }

    file_ok[arr_id] = true;
    dim[arr_id] = qMakePair(_row, _col);
    update_dim_label(arr_id);
    put_data_into_widget(matrix, table[arr_id]);
}

void MainWindow::put_data_into_widget(QVector<QVector<QString>> data, QTableWidget *wid)
{
    int _rows = qMin(data.length(), PREVIEW_DIM_ROWS);
    int _cols = qMin(data[0].length(), PREVIEW_DIM_COLS);

    int extra_row = (data.length() > PREVIEW_DIM_ROWS) ? 1 : 0;
    int extra_col = (data[0].length() > PREVIEW_DIM_COLS) ? 1 : 0;

    wid->setRowCount(_rows + extra_row);
    wid->setColumnCount(_cols + extra_col);

    for (int i = 0; i < _rows; i++) {
        for (int j = 0; j < _cols; j++) {
            wid->setItem(i, j, new QTableWidgetItem(data[i][j]));
        }
    }
    if (extra_row) {
        wid->setItem(_rows, 0, new QTableWidgetItem("..."));
    }
    if (extra_col) {
        wid->setItem(0, _cols, new QTableWidgetItem("..."));
    }
}

void MainWindow::dim_err_helper(QLabel *label)
{
    AuthDialog::lolcat(label, nullptr, "red");
    QMessageBox::warning(this, tr("Bad dimensions"), tr("Try again with diferent file."));
}


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
    // test_upload();

void MainWindow::toggle_scrollbars(bool show, int tab_index)
{
    Qt::ScrollBarPolicy policy = Qt::ScrollBarAlwaysOff;
    if (show) {
        policy = Qt::ScrollBarAsNeeded;
    }
    for (int i = 3 * tab_index; i < 3 * tab_index + 3; i++) {
        table[i]->setVerticalScrollBarPolicy(policy);
        table[i]->setHorizontalScrollBarPolicy(policy);
    }
}

void MainWindow::toggle_headers(bool show, int tab_index)
{
    for (int i = 3 * tab_index; i < 3 * tab_index + 3; i++) {
        if (show) {
            table[i]->verticalHeader()->show();
            table[i]->horizontalHeader()->show();
        }
        else {
            table[i]->verticalHeader()->hide();
            table[i]->horizontalHeader()->hide();
        }
    }
}

void MainWindow::update_dim_label(int index)
{
    label_dim[index]->setText(QString("(%1x%2)").arg(QString::number(dim[index].first), QString::number(dim[index].second)));
}

void MainWindow::on_tabWidget_currentChanged(int index)
{
    toggle_scrollbars(ui->actionShow_scrollbars->isChecked(), index);
    toggle_headers(ui->actionShow_headers->isChecked(), index);
}
