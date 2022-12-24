#ifndef WAREHOUSEUI_H
#define WAREHOUSEUI_H

#include <QDialog>
#include "warehouse.h"
#include "dialogaddprd.h"
#include "dialogreviseprd.h"
namespace Ui {
class WarehouseUI;
}

class WarehouseUI : public QDialog
{
    Q_OBJECT

public:
    Warehouse* wh;
    QList<int> index;//索引值列表
public:
    explicit WarehouseUI(Warehouse* wh,QWidget *parent = nullptr);
    ~WarehouseUI();
    void showtable();//将指定产品信息显示出来
    void showtableheader();//显示表头
    void refresh();//刷新表格
    void setstyle();//设置界面美化

private slots:

    void on__search_clicked();

    void on__add_clicked();

    void on__revise_clicked();

    void on__delete_clicked();

private:
    Ui::WarehouseUI *ui;
};

#endif // WAREHOUSEUI_H
