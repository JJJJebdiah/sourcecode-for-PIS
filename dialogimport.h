#ifndef DIALOGIMPORT_H
#define DIALOGIMPORT_H

#include <QDialog>
#include <product.h>

namespace Ui {
class DialogImport;
}

class DialogImport : public QDialog
{
    Q_OBJECT
public:
    QList<Product*> prds;
    bool type;//true表示入库，否则为出库

public:
    explicit DialogImport(QList<Product*> &prds,bool type=true,QWidget *parent = nullptr);
    ~DialogImport();
    void showtableheader();
    void setstyle();//设置界面美化

private slots:
    void on__cancel_clicked();

    void on__save_clicked();
    void itemchanged(int row, int column);//需要connect

private:
    Ui::DialogImport *ui;
};

#endif // DIALOGIMPORT_H
