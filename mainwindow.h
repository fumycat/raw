#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "raw_global.h"

#include <QMainWindow>
#include <QTableWidget>
#include <QMessageBox>
#include <QVector>
#include <QJsonDocument>
#include <QJsonObject>
#include <QFileDialog>
#include <QValidator>
#include <QPair>
#include <QStandardPaths>
#include <QDateTime>

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
    void on_tabWidget_currentChanged(int index);

private:
    Ui::MainWindow *ui;
    QValidator* args_validator;

    QString server_url;
    QString token;
    QTableWidget* table[6];
    QLabel* label_dim[6];
    QPushButton* go_btn[2];
    QString data[6];
    bool file_ok[6];
    QPair<int, int> dim[6];

    void dim_err_helper(QLabel*);
    void toggle_scrollbars(bool, int);
    void toggle_headers(bool, int);
    void update_dim_label(int);
    void go_send(int);
    void clear_input(int);

    void open_matrix_file(int, bool is_vec = false, QString debug_filename = nullptr);
    void put_data_into_widget(QVector<QVector<QString>> data, QTableWidget *wid);


};
#endif // MAINWINDOW_H
