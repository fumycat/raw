#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "raw_global.h"

#include <QMainWindow>
#include <QNetworkReply>
#include <QTableWidget>
#include <QMessageBox>
#include <QVector>
#include <QtNetwork/QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonObject>
#include <QUrl>
#include <QFileDialog>
#include <QHttpMultiPart>
#include <QDataStream>
#include <QTemporaryFile>
#include <QTimer>
#include <QValidator>
#include <QPair>

#include "authdialog.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onfinish(QNetworkReply *rep);
    void onfinish_test(QNetworkReply *rep);
    void on_tabWidget_currentChanged(int index);

private:
    QString token;
    QTableWidget* table[6];
    QLabel* label_dim[6];
    QString data[6];
    bool file_ok[6];
    QPair<int, int> dim[6];

    QValidator* args_validator;

    void dim_err_helper(QLabel*);
    void toggle_scrollbars(bool, int);
    void toggle_headers(bool, int);
    void update_dim_label(int);

    void test_upload();
    int prepare_file(QString, QVector<float>*);
    void open_matrix_file(int, bool = false);
    void put_data_into_widget(QVector<QVector<QString>> data, QTableWidget *wid);
    Ui::MainWindow *ui;
    // QString arr[2];
    // QString fnames[2];

};
#endif // MAINWINDOW_H
