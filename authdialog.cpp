#include "authdialog.h"
#include "ui_authdialog.h"

AuthDialog::AuthDialog(QString *token, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AuthDialog)
{
    ui->setupUi(this);
    ui->pushButton_auth->setEnabled(true);
    this->token = token;
}

AuthDialog::~AuthDialog()
{
    delete ui;
}

void AuthDialog::on_pushButton_auth_clicked()
{
    ui->pushButton_auth->setEnabled(false);
    lolcat(ui->label_status, "");

    QNetworkAccessManager *mgr = new QNetworkAccessManager(this);
    QNetworkRequest request(QUrl(QString(DEFAULT_URL) + AUTH_PATH));
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject j;
    j["username"] = ui->lineEdit_username->text();
    j["password"] = ui->lineEdit_password->text();
    QByteArray data = QJsonDocument(j).toJson();
    QNetworkReply *reply = mgr->post(request, data);

    connect(reply, &QNetworkReply::finished, this, [=](){
        if(reply->error() == QNetworkReply::NoError){
            QByteArray data = reply->readAll();
            QJsonDocument qjd = QJsonDocument::fromJson(data);
            QJsonObject qjo = qjd.object();
            if (qjo.contains("status")) {
                if (qjo.value("status").toString() == "Ok") {
                    lolcat(ui->label_status, "Ok");
                    *this->token = qjo.value("token").toString();
                    QDialog::accept();
                }
                else {
                    QString err_str = qjo.value("message").toString();
                    lolcat(ui->label_status, err_str, "red");
                    QTimer::singleShot(AUTH_DELAY, this, [this]() { ui->pushButton_auth->setEnabled(true); });
                }
            }
        }
        else {
            QString err = reply->errorString();
            qDebug() << "ERROR" << err;
            lolcat(ui->label_status, "Unknown error", "pink");
            QTimer::singleShot(AUTH_DELAY, this, [this]() { ui->pushButton_auth->setEnabled(true); });
        }
        reply->deleteLater();
    });
}

void AuthDialog::lolcat(QLabel* label, QString text, QString color)
{
    label->setStyleSheet(QString("QLabel { color : %1; }").arg(color));
    if (text != nullptr) {
        label->setText(text);
    }
}
