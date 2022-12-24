#include "warehouse.h"

Warehouse::Warehouse()
{

}

bool Warehouse::addPrd(QString name, QString code, int number, QString location, QString remark)
{

    prds.append(Product(name,code,number,location,remark));
    return true;
}

bool Warehouse::addPrd(Product &p)
{
    prds.append(p);
    return true;
}

bool Warehouse::delPrd(int pos)
{
    prds.remove(pos);
    return true;
}



QList<int> Warehouse::searchPrd(QString name, QString code,QString location)
{
    QList<int> index;
    QString s;
    for(int i=0;i<prds.size();++i)
    {
        if(prds[i].code.contains(code)&&prds[i].name.contains(name)&&prds[i].location.contains(location))
        {
            index.append(i);
        }
    }
    return index;
}
