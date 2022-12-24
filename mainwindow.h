#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFile>
#include "warehouse.h"
#include "summaryui.h"
#include "warehouseui.h"
#include "importui.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    Warehouse wh;

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool getData();
    bool traverseFiles(QString& path);
    QString getCorrectUnicode(const QByteArray &ba);
    void setstyle();//设置界面美化


private slots:
    void on__import_clicked();

    void on__export_clicked();

    void on__warehouse_clicked();

    void on__summary_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
