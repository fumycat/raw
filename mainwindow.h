#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkReply>
#include <QTableWidget>
#include <QMessageBox>

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
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

private:
    void open_matrix_file(int arr_id, QTableWidget *wid);
    void put_data_into_widget(QVector<QVector<QString>> data, QTableWidget *wid);
    Ui::MainWindow *ui;
    QString arr[2];
    QString fnames[2];
    bool files_selected[2];
};
#endif // MAINWINDOW_H
