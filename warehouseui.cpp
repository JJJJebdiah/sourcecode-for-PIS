#include "warehouseui.h"
#include "ui_warehouseui.h"
#include <QMessageBox>
#include <QFile>
#include <QDir>
#include <QDate>

WarehouseUI::WarehouseUI(Warehouse* wh,QWidget *parent) :
    QDialog(parent),wh(wh),
    ui(new Ui::WarehouseUI)
{
    ui->setupUi(this);
    setstyle();
    this->size().setHeight(600);
    this->size().setWidth(800);
    for(int i=0;i<wh->prds.size();i++)
        index.append(i);
    showtable();
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(35);//设置行高

    //设置只能选单行
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);

}

WarehouseUI::~WarehouseUI()
{
    delete ui;
}

void WarehouseUI::showtable()
{
    ui->tableWidget->clear();
    showtableheader();
    if(index.size()==0)
    {
         return;
    }

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
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不能修改表格

}

void WarehouseUI::showtableheader()
{
    QStringList l;
    l<<"物料编码"<<"图号名称"<<"现有库存"<<"位置"<<"备注";
    ui->tableWidget->setColumnCount(l.size());
    ui->tableWidget->setHorizontalHeaderLabels(l);
}

void WarehouseUI::refresh()
{
    QList<int> index;
    for(int i=0;i<wh->prds.size();i++)
    {
        index.append(i);
    }
    this->index=index;
    showtable();
}

void WarehouseUI::setstyle()
{
    QString s=QCoreApplication::applicationDirPath()+"/Mycss/warehouseui_style.css";
    QFile file(s);
    if(!file.open(QIODeviceBase::ReadOnly))
    {
        return;
    }
    QString m=file.readAll();
    this->setStyleSheet(m);
}



void WarehouseUI::on__search_clicked()
{
    index=wh->searchPrd(ui->_name->text(),ui->_code->text(),ui->_location->text());
    if(index.size()==0)
    {
        QMessageBox::warning(this,"警告","未找到该货品！");
        showtable();
        return;
    }

    //显示在表格上
    showtable();
}

bool addDir(Product &prd)
{
    /*添加产品信息文件夹*/
    QString path=QCoreApplication::applicationDirPath()+"//data//Products//__"+prd.code;
    QDir dir;
    dir.mkpath(path);//添加文件夹
    //添加data文件
    QFile file(path+"//data.dat");
    file.open(QIODeviceBase::WriteOnly);
    QTextStream fout(&file);
    QString content = prd.code+" "
            +prd.name+" "
            +QString::number(prd.number)+" "
            +prd.location+" "
            +prd.remark;
    fout<<content<<"\n";
    file.close();
    //添加import文件
    file.setFileName(path+"//import.dat");
    file.open(QIODeviceBase::WriteOnly);
    fout.setDevice(&file);
    QDate d=QDate::currentDate();//获取当前日期
    QString date=QString::number(d.year())+"-"
            +QString::number(d.month())+"-"
            +QString::number(d.day());
    content=date+" "
            +prd.code+" "
            +prd.name+" "
            +QString::number(prd.number)+" "
            +"新增"+" "
            +QString::number(prd.number);
    fout<<content<<"\n";
    file.close();
    //添加export文件
    file.setFileName(path+"//export.dat");
    file.open(QIODeviceBase::WriteOnly);
    file.close();
    return true;
}
void WarehouseUI::on__add_clicked()
{
    Product prd;
    DialogAddPrd* dap= new DialogAddPrd(&prd);
    dap->exec();
    if(prd.name.isEmpty())//如果未添加
        {
        qDebug()<<"未添加";
        return;
    }

    //判断物料编码是否重复
    QList<int> index=wh->searchPrd("",prd.code,"");
    if(index.size()!=0)
    {
        QMessageBox::warning(this,"添加失败","物料编码与已有的信息重复！");
        this->index=index;
        showtable();//显示重复的信息
        return;
    }
    //添加
    wh->addPrd(prd);

    //添加产品信息文件夹
    addDir(prd);
    QMessageBox::about(this,"","添加成功！");
    refresh();
}


void WarehouseUI::on__revise_clicked()
{
    int i=ui->tableWidget->currentRow();//获取表格当前选择的行号,未选则返回-1
    if(-1==i) return;
    Product prd=wh->prds[i];
    bool is=false;
    bool* ischanged=&is;
    DialogRevisePrd* drp = new DialogRevisePrd(&prd,ischanged);
    drp->exec();
    if(*ischanged==false)
        return;
    QString path=QCoreApplication::applicationDirPath()+"//data//Products//__";
    if(prd.code!=wh->prds[i].code)//如果物料编码被修改
    {
        //判断物料编码是否重复
        QList<int> index=wh->searchPrd("",prd.code,"");
        if(index.size()!=0)
        {
            QMessageBox::warning(this,"修改失败","物料编码与已有的信息重复！");
            this->index=index;
            showtable();//显示重复的信息
            return;
        }
        //删除原文件夹
        QDir dir(path+wh->prds[i].code);
        dir.removeRecursively();
        //新建文件夹
        addDir(prd);
        //修改成功
        wh->prds[i]=prd;
        QMessageBox::about(this,"","修改成功！");
        refresh();
        return;


    }
    //修改data文件
    QFile file(path+wh->prds[i].code+"//data.dat");
    file.open(QIODeviceBase::WriteOnly);
    QTextStream fout(&file);
    QString content = prd.code+" "
            +prd.name+" "
            +QString::number(prd.number)+" "
            +prd.location+" "
            +prd.remark;
    fout<<content<<"\n";
    file.close();
    //修改成功
    wh->prds[i]=prd;
    QMessageBox::about(this,"","修改成功！");
    refresh();

}


void WarehouseUI::on__delete_clicked()
{
    int i=ui->tableWidget->currentRow();
    if(-1==i) return;
    if(QMessageBox::Ok==QMessageBox::warning(this,"警告","请确认是否删除！删除后信息将无法找回！",QMessageBox::Ok,QMessageBox::Cancel))
    {
        QString path=QCoreApplication::applicationDirPath()+"//data//Products//__";
        QDir dir(path+wh->prds[i].code);
        if(!dir.removeRecursively())
        {
            QMessageBox::warning(this,"错误","删除原文件夹失败！");
            return;
        }
        wh->prds.removeAt(i);
        refresh();
    }
}

