#include "importui.h"
#include "ui_importui.h"
#include "dialogimport.h"
#include <QMessageBox>
#include <QFile>
#include <QDate>

ImportUI::ImportUI(Warehouse* wh,bool type,QWidget *parent) :
    QDialog(parent),wh(wh),type(type),
    ui(new Ui::ImportUI)
{
    ui->setupUi(this);
    setstyle();
    if(!type)
    {
        setWindowTitle("出库");
        ui->_import->setText("出库");
        ui->label_windowname->setText("出库");
    }
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不能修改表格
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(35);//设置行高

    //设置只能选单行
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    //ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

}

ImportUI::~ImportUI()
{
    delete ui;
}

void ImportUI::showtable()
{
    ui->tableWidget->clear();
    showtableheader();
    if(index.size()==0)
        return;
    ui->tableWidget->setRowCount(index.size());

    for(int i=0;i<index.size();i++)
    {
        int colu=0;
        ui->tableWidget->setItem(i,colu++,new QTableWidgetItem(wh->prds[index[i]].code));
        ui->tableWidget->setItem(i,colu++,new QTableWidgetItem(wh->prds[index[i]].name));
        ui->tableWidget->setItem(i,colu++,new QTableWidgetItem(QString::number(wh->prds[index[i]].number)));
        ui->tableWidget->setItem(i,colu++,new QTableWidgetItem(wh->prds[index[i]].location));
        ui->tableWidget->setItem(i,colu++,new QTableWidgetItem(wh->prds[index[i]].remark));


    };

}

void ImportUI::showtableheader()
{
    QStringList l;
    l<<"物料编码"<<"图号名称"<<"现有库存"<<"位置"<<"备注";
    ui->tableWidget->setColumnCount(l.size());
    ui->tableWidget->setHorizontalHeaderLabels(l);
}

void ImportUI::setstyle()
{
    QString s=QCoreApplication::applicationDirPath()+"/Mycss/importui_style.css";
    QFile file(s);
    if(!file.open(QIODeviceBase::ReadOnly))
    {
        return;
    }
    QString m=file.readAll();
    this->setStyleSheet(m);
}

void ImportUI::on__search_clicked()
{
    index=wh->searchPrd(ui->_name->text(),ui->_code->text());
    if(index.size()==0)
    {
        QMessageBox::warning(this,"警告","未找到该货品！");
        showtable();
        return;
    }

    //显示在表格上
    showtable();
}


void ImportUI::on__import_clicked()
{
    //获得所有被选行的行号
    int num=ui->tableWidget->selectedItems().size();
    auto items=ui->tableWidget->selectedItems();
    if(num==0)
        return;
    QList<Product*> prds;
    for(int i=0;i<num;i+=5)
        prds.append(&wh->prds[index[items[i]->row()]]);
    DialogImport* di=new DialogImport(prds,type);
    di->exec();
    showtable();
    return;
/*    int i=ui->tableWidget->currentRow();//获取表格当前选择的行号,未选则返回-1
    if(-1==i) return;
    if(ui->_number->text().size()*ui->_unit->text().size()==0)
    {
        QMessageBox::warning(this,"警告","数量与单位不能为空！");
        return;
    }
    int number=ui->_number->text().toInt();
    QString unit=ui->_unit->text();
    if(type)
    {
        wh->prds[index[i]].add(number);
    }
    else
    {
        if(!wh->prds[index[i]].delet(number))
        {
            QMessageBox::warning(this,"警告","库存不够用！");
            return;
        }
    }

    //修改data文件
    QString path=QCoreApplication::applicationDirPath()+"//data//Products//__"+wh->prds[index[i]].code;
    QFile file(path+"//data.dat");
    if(!file.open(QIODeviceBase::ReadOnly))
    {
        QMessageBox::warning(this,"修改data文件","未找到该产品原信息文件！");
        return;
    }
    file.close();
    file.open(QIODeviceBase::WriteOnly);
    QTextStream fout(&file);
    QString content = wh->prds[index[i]].code+" "
            +wh->prds[index[i]].name+" "
            +QString::number(wh->prds[index[i]].number)+" "
            +wh->prds[index[i]].location+" "
            +wh->prds[index[i]].remark;
    fout<<content<<"\n";
    file.close();
    showtable();
    QMessageBox::information(this,"","成功！");
    //记录产品文件夹中
    if(type)//入库
    {
        file.setFileName(path+"//import.dat");
        if(!file.open(QIODeviceBase::Append))
        {
            QMessageBox::warning(this,"修改import文件","未找到该产品原信息文件！");
            return;
        }

    }
    else//出库
    {
        file.setFileName(path+"//export.dat");
        if(!file.open(QIODeviceBase::Append))
        {
            QMessageBox::warning(this,"修改export文件","未找到该产品原信息文件！");
            return;
        }
    }
    fout.setDevice(&file);
    QDate d=QDate::currentDate();//获取当前日期
    QString date=QString::number(d.year())+"-"
            +QString::number(d.month())+"-"
            +QString::number(d.day());
    content=date+" "
            +wh->prds[index[i]].code+" "
            +wh->prds[index[i]].name+" "
            +QString::number(number)+" "
            +unit+" "
            +QString::number(wh->prds[index[i]].number);
    fout<<content<<"\n";
    file.close();
    //汇总
    path=QCoreApplication::applicationDirPath()+"//data//Records";
    if(type)//入库
    {

        file.setFileName(path+"//import//"+date+".dat");
        if(!file.open(QIODeviceBase::Append))
        {
            file.open(QIODeviceBase::WriteOnly);
        }
    }
    else//出库
    {
        file.setFileName(path+"//export//"+date+".dat");
        if(!file.open(QIODeviceBase::Append))
        {
            file.open(QIODeviceBase::WriteOnly);
        }
    }
    fout.setDevice(&file);
    fout<<content<<"\n";
    file.close();
*/
}

