#ifndef IMPORTUI_H
#define IMPORTUI_H

#include <QDialog>
#include "warehouse.h"
namespace Ui {
class ImportUI;
}

class ImportUI : public QDialog
{
    Q_OBJECT

public:
    Warehouse* wh;
    QList<int> index;//索引值列表
    bool type;//true表示入库，否则为出库

public:
    explicit ImportUI(Warehouse* wh,bool type=true,QWidget *parent = nullptr);
    ~ImportUI();
    void showtable();//将指定产品信息显示出来
    void showtableheader();//显示表头
    void setstyle();//设置界面美化

private slots:
    void on__search_clicked();

    void on__import_clicked();

private:
    Ui::ImportUI *ui;
};

#endif // IMPORTUI_H
