#include "dialogimport.h"
#include "ui_dialogimport.h"
#include <QDate>
#include <QFile>
#include <QMessageBox>
#include <QFileDialog>
#include <QProcess>

DialogImport::DialogImport(QList<Product*> &prds,bool type,QWidget *parent) :
    QDialog(parent),prds(prds),type(type),
    ui(new Ui::DialogImport)
{
    ui->setupUi(this);
    ui->dateEdit->setDate(QDate::currentDate());
    setstyle();
    showtableheader();
    if(!type)
    {
        setWindowTitle("出库");
        ui->label_windowname->setText("出库");
    }
    ui->tableWidget->setRowCount(prds.size());
    for(int i=0;i<prds.size();++i)
    {
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(prds[i]->code));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(prds[i]->name));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(""));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(""));

    }
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    connect(ui->tableWidget,&QTableWidget::cellChanged,this,&DialogImport::itemchanged);
}

DialogImport::~DialogImport()
{
    delete ui;
}

void DialogImport::showtableheader()
{
    QStringList l;
    if(type)
    {
        l<<"物料编码"<<"图号名称"<<"入库数量"<<"备注";
    }
    else
    {
        l<<"物料编码"<<"图号名称"<<"出库数量"<<"备注";
    }

    ui->tableWidget->setColumnCount(l.size());
    ui->tableWidget->setHorizontalHeaderLabels(l);
}

void DialogImport::setstyle()
{
    QString s=QCoreApplication::applicationDirPath()+"/Mycss/dialogimport_style.css";
    QFile file(s);
    if(!file.open(QIODeviceBase::ReadOnly))
    {
        return;
    }
    QString m=file.readAll();
    this->setStyleSheet(m);
}

void DialogImport::on__cancel_clicked()
{
    this->close();
}


void DialogImport::on__save_clicked()
{
    //检查错误
    QString unit=ui->_unit->text();
    if(unit.isEmpty())
    {
        QMessageBox::warning(this,"错误","客户名称不能为空!");
        return;
    }
    if(unit.contains(" "))
    {
        QMessageBox::warning(this,"错误","客户名称不能有空格!");
        return;
    }
    QStringList numbers,remarks;
    for(int i=0;i<ui->tableWidget->rowCount();i++)
    {
        QString number=ui->tableWidget->item(i,2)->text();
        if(number.isEmpty())
        {
            QMessageBox::warning(this,"错误","数量不能为空!");
            return;
        }
        if(number.contains(" "))
        {
            QMessageBox::warning(this,"错误","数量不能有空格!");
            return;
        }
        for(auto& i:number)
        {
            if(i<'0'||i>'9')
            {
                QMessageBox::warning(this,"错误","数量必须为非负整数!");
                return;
            }
        }
        numbers.append(number);
        QString remark=ui->tableWidget->item(i,3)->text();
        if(unit.contains(" "))
        {
            QMessageBox::warning(this,"错误","备注不能有空格!");
            return;
        }
        remarks.append(remark);

    }
    //保存
    QString date;
    for(int i=0;i<prds.size();i++)
    {

        if(type)
        {
            prds[i]->add(numbers[i].toInt());
        }
        else
        {
            if(!prds[i]->delet(numbers[i].toInt()))
            {
                QMessageBox::warning(this,"警告",QString("%1库存不够用！").arg(prds[i]->code));
                return;
            }
        }
        //修改data文件
        QString path=QCoreApplication::applicationDirPath()+"//data//Products//__"+prds[i]->code;
        QFile file(path+"//data.dat");
        if(!file.open(QIODeviceBase::ReadOnly))
        {
            QMessageBox::warning(this,"修改data文件","未找到该产品原信息文件！");
            return;
        }
        file.close();
        file.open(QIODeviceBase::WriteOnly);
        QTextStream fout(&file);
        QString content = prds[i]->code+" "
                +prds[i]->name+" "
                +QString::number(prds[i]->number)+" "
                +prds[i]->location+" "
                +prds[i]->remark;
        fout<<content<<"\n";
        file.close();
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
        date=QString::number(d.year())+"-"
                +QString::number(d.month())+"-"
                +QString::number(d.day());
        content=date+" "
                +prds[i]->code+" "
                +prds[i]->name+" "
                +numbers[i]+" "
                +unit+" "
                +QString::number(prds[i]->number);
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

    }
    //通知
    if(QMessageBox::Ok==QMessageBox::warning(this,"保存成功","保存成功！请确认是否要生成报表",QMessageBox::Ok,QMessageBox::Cancel))
    {
        QString filename = QFileDialog::getSaveFileName(this,"请选择生成文件的位置",QCoreApplication::applicationDirPath(),"EXCEL(*.xlsx)");
        if(filename.isEmpty())
            return;
        QString pyname=QDir::currentPath();
        QFile file(pyname+"\\DialogImport.dat");
        file.open(QIODeviceBase::WriteOnly);
        QTextStream fout(&file);
        QString content;
        if(type)
            fout<<"import"<<"\n";
        else
            fout<<"export"<<"\n";
        fout<<filename<<"\n";
        fout<<date<<"\n";
        fout<<unit<<"\n";
        for(int i=0;i<prds.size();i++)
        {
            content=
                    prds[i]->code+" "
                    +prds[i]->name+" "
                    +numbers[i]+" "
                    +remarks[i];
            fout<<content<<"\n";
        }
        file.close();
        QMessageBox::about(this,"","生成成功！");
        //执行python文件
        QProcess p(this);
        p.startDetached(pyname+"\\pyscript"+"\\DialogImport.exe");
    }
    this->close();


}

void DialogImport::itemchanged(int row, int column)
{
    if(column==0)
    {
        ui->tableWidget->item(row,column)->setText(prds[row]->code);
        return;
    }
    if(column==1)
    {
        ui->tableWidget->item(row,column)->setText(prds[row]->name);
        return;
    }
}

