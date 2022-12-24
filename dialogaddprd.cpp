#include "dialogaddprd.h"
#include "ui_dialogaddprd.h"
#include <QMessageBox>
#include <QFile>
void DialogAddPrd::showtableheader()
{
    QStringList l;
    l<<"物料编码"<<"图号名称"<<"现有库存"<<"位置"<<"备注";
    ui->tableWidget->setColumnCount(l.size());
    ui->tableWidget->setHorizontalHeaderLabels(l);
}

void DialogAddPrd::setstyle()
{
    QString s=QCoreApplication::applicationDirPath()+"/Mycss/dialogaddprd_style.css";
    QFile file(s);
    if(!file.open(QIODeviceBase::ReadOnly))
    {
        return;
    }
    QString m=file.readAll();
    this->setStyleSheet(m);
}
DialogAddPrd::DialogAddPrd(Product* prd,QWidget *parent) :
    QDialog(parent),prd(prd),
    ui(new Ui::DialogAddPrd)
{
    ui->setupUi(this);
    setstyle();
    showtableheader();
    ui->tableWidget->setRowCount(1);

    for(int i=0;i<5;++i)
    {
        ui->tableWidget->setItem(0,i,new QTableWidgetItem(""));
    }
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
}

DialogAddPrd::~DialogAddPrd()
{
    delete ui;
}

void DialogAddPrd::on__exit_clicked()
{
    this->close();
}




void DialogAddPrd::on__save_clicked()
{

    QString name=ui->tableWidget->item(0,1)->text();
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
    QString code=ui->tableWidget->item(0,0)->text();
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
    QString number=ui->tableWidget->item(0,2)->text();
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
    QString location=ui->tableWidget->item(0,3)->text();
    if(location.isEmpty())
    {
        QMessageBox::warning(this,"错误","位置不能为空!");
        return;
    }
    QString remark=ui->tableWidget->item(0,4)->text();
    prd->name=name;
    prd->code=code;
    prd->number=number.toInt();
    prd->location=location;
    prd->remark=remark;
    this->close();
}

