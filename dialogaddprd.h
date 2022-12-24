#ifndef DIALOGADDPRD_H
#define DIALOGADDPRD_H

#include <QDialog>
#include "product.h"
namespace Ui {
class DialogAddPrd;
}

class DialogAddPrd : public QDialog
{
    //获得要添加的信息
    Q_OBJECT
public:
    Product* prd;//要添加的信息
public:
    explicit DialogAddPrd(Product* prd,QWidget *parent = nullptr);
    ~DialogAddPrd();
    void showtableheader();
    void setstyle();//设置界面美化

private slots:
    void on__exit_clicked();


    void on__save_clicked();

private:
    Ui::DialogAddPrd *ui;
};

#endif // DIALOGADDPRD_H
