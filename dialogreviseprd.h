#ifndef DIALOGREVISEPRD_H
#define DIALOGREVISEPRD_H

#include <QDialog>
#include "product.h"
namespace Ui {
class DialogRevisePrd;
}

class DialogRevisePrd : public QDialog
{
    Q_OBJECT
public:
    Product* prd;//要修改的信息
    bool* ischanged;
public:
    explicit DialogRevisePrd(Product* prd,bool* ischanged,QWidget *parent = nullptr);
    ~DialogRevisePrd();
    void showtableheader();
    void setstyle();//设置界面美化
private slots:
    void on__exit_clicked();
    void itemchanged(int row, int column);//需要connect

    void on__save_clicked();

private:
    Ui::DialogRevisePrd *ui;
};

#endif // DIALOGREVISEPRD_H
