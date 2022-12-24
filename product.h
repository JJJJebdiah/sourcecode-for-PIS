#ifndef PRODUCT_H
#define PRODUCT_H
#include <QString>

class Product//单类产品
{
public:
    QString name;//图号名称
    QString code;//物料编码
    int number;//数量
    QString location;//位置
    QString remark;//备注

public:
    Product(QString name="",QString code="",
            int number=0,QString location="",QString remark=""):name(name),
        code(code),number(number),location(location),remark(remark){}
    bool add(int num);//入库
    bool delet(int num);//出库,若出库后小于零，则返回false
};


#endif // PRODUCT_H
