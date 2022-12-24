#include "dialogreviseprd.h"
#include "ui_dialogreviseprd.h"
#include<QMessageBox>
#include <QFile>
DialogRevisePrd::DialogRevisePrd(Product* prd,bool* ischanged,QWidget *parent) :
    QDialog(parent),prd(prd),ischanged(ischanged),
    ui(new Ui::DialogRevisePrd)
{
    ui->setupUi(this);
    setstyle();
    showtableheader();
    ui->tableWidget->setRowCount(1);
    ui->tableWidget->setItem(0,1,new QTableWidgetItem(prd->name));
    ui->tableWidget->setItem(0,0,new QTableWidgetItem(prd->code));
    ui->tableWidget->setItem(0,2,new QTableWidgetItem(QString::number(prd->number)));
    ui->tableWidget->setItem(0,3,new QTableWidgetItem(prd->location));
    ui->tableWidget->setItem(0,4,new QTableWidgetItem(prd->remark));
    connect(ui->tableWidget,&QTableWidget::cellChanged,this,&DialogRevisePrd::itemchanged);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

DialogRevisePrd::~DialogRevisePrd()
{

    delete ui;
}

void DialogRevisePrd::showtableheader()
{
    QStringList l;
    l<<"物料编码"<<"图号名称"<<"现有库存"<<"位置"<<"备注";
    ui->tableWidget->setColumnCount(l.size());
    ui->tableWidget->setHorizontalHeaderLabels(l);
}

void DialogRevisePrd::setstyle()
{
    QString s=QCoreApplication::applicationDirPath()+"/Mycss/dialogreviseprd_style.css";
    QFile file(s);
    if(!file.open(QIODeviceBase::ReadOnly))
    {
        return;
    }
    QString m=file.readAll();
    this->setStyleSheet(m);
}

void DialogRevisePrd::on__exit_clicked()
{
    *ischanged=false;
    this->close();
}

void DialogRevisePrd::itemchanged(int row, int column)
{
    *ischanged=true;
    return;

}


void DialogRevisePrd::on__save_clicked()
{
    if(*ischanged==false)
    {
        this->close();
        return;
    }
    QString name=ui->tableWidget->item(0,1)->text();
    if(name!=prd->name)
    {
        if(name.isEmpty())
        {
            QMessageBox::warning(this,"错误","图号名称不能为空!");
            return;
        }
        if(name.contains(" "))
        {
            QMessageBox::warning(this,"错误","图号名称不能有空格!");
            return;
        }
        prd->name=name;
    }

    QString code=ui->tableWidget->item(0,0)->text();
    if(code!=prd->code)
    {
        if(code.isEmpty())
        {
            QMessageBox::warning(this,"错误","物料编码不能为空!");
            return;
        }
        if(code.contains(" "))
        {
            QMessageBox::warning(this,"错误","物料编码不能有空格!");
            return;
        }
        //判断是否有汉字
        for(auto& i:code)
        {
            ushort u=i.unicode();
            if(u >= 0x4E00 && u <= 0x9FA5)
            {
                QMessageBox::warning(this,"错误","物料编码不能有汉字!");
                return;
            }
        }
        prd->code=code;
    }

    QString number=ui->tableWidget->item(0,2)->text();
    if(number!=QString::number(prd->number))
    {
        if(number.isEmpty())
        {
            QMessageBox::warning(this,"错误","库存不能为空!");
            return;
        }
        if(number.contains(" "))
        {
            QMessageBox::warning(this,"错误","库存不能有空格!");
            return;
        }
        for(auto& i:number)
        {
            if(i<'0'||i>'9')
            {
                QMessageBox::warning(this,"错误","库存必须为非负整数!");
                return;
            }
        }
        prd->number=number.toInt();
    }

    QString location=ui->tableWidget->item(0,3)->text();
    if(location!=prd->location)
    {
        if(location.isEmpty())
        {
            QMessageBox::warning(this,"错误","位置不能为空!");
            return;
        }
        prd->location=location;
    }

    QString remark=ui->tableWidget->item(0,4)->text();
    if(remark!=prd->remark)
    {
        prd->remark=remark;
    }
    this->close();
}

