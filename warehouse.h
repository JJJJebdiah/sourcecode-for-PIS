#ifndef WAREHOUSE_H
#define WAREHOUSE_H

#include "product.h"
#include <QList>
class Warehouse//仓库
{
public:
    QList<Product> prds;//产品
public:
    Warehouse();
    bool addPrd(QString name,QString code,
                int number,QString location,QString remark);//添加产品
    bool addPrd(Product& p);
    bool delPrd(int pos);//删除索引值为pos的产品
    QList<int> searchPrd(QString name="",QString code="",QString location="");//查找产品，返回符合的索引值列表

};

#endif // WAREHOUSE_H
