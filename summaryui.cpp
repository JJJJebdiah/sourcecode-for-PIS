#include "summaryui.h"
#include "ui_summaryui.h"
#include <QDate>
#include <QMessageBox>
#include <QDir>
#include <QTextCodec>
#include <QFileDialog>
#include <QProcess>

SummaryUI::SummaryUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SummaryUI)
{
    ui->setupUi(this);
    setstyle();
    ui->_beginDate->setDate(QDate::currentDate());
    ui->_endDate->setDate(QDate::currentDate());
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    ui->tableWidget->verticalHeader()->setDefaultSectionSize(35);//设置行高

    //设置只能选单行
    ui->tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->tableWidget->setSelectionMode(QAbstractItemView::SingleSelection);
}

SummaryUI::~SummaryUI()
{
    delete ui;
}

void SummaryUI::showtable(QString type)
{
    ui->tableWidget->clear();
    showtableheader(type);
    ui->tableWidget->setRowCount(rds.size());
    for(int i=0;i<rds.size();i++)
    {
        ui->tableWidget->setItem(i,0,new QTableWidgetItem(rds[i].date));
        ui->tableWidget->setItem(i,1,new QTableWidgetItem(rds[i].code));
        ui->tableWidget->setItem(i,2,new QTableWidgetItem(rds[i].name));
        ui->tableWidget->setItem(i,3,new QTableWidgetItem(rds[i].i_e));
        ui->tableWidget->setItem(i,4,new QTableWidgetItem(rds[i].unit));
        ui->tableWidget->setItem(i,5,new QTableWidgetItem(rds[i].number));

    }
    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);//不能修改表格
}

void SummaryUI::showtableheader(QString type)
{
    QStringList l;
    if(type=="import")
        l<<"日期"<<"物料编码"<<"图号名称"<<"入库"<<"单位"<<"剩余库存";
    else
        l<<"日期"<<"物料编码"<<"图号名称"<<"出库"<<"单位"<<"剩余库存";
    ui->tableWidget->setColumnCount(l.size());
    ui->tableWidget->setHorizontalHeaderLabels(l);
}


QString SummaryUI::getCorrectUnicode(const QByteArray &ba)
{
    /*自动将从文件读来的GBK编码或UTF-8编码的字符串转换成Unicode（qt默认编码格式），防止乱码
     需要头文件<QTextCodec>
     需要.pro文件添加 QT+=core5compat
*/
    QTextCodec::ConverterState state;
    QTextCodec* codec=QTextCodec::codecForName("utf-8");
    QString text=codec->toUnicode(ba.constData(),ba.size(),&state);
    if(state.invalidChars>0)
    {
        text=QTextCodec::codecForName("GBK")->toUnicode(ba.constData());
    }
    return text;
}

void SummaryUI::setstyle()
{
    QString s=QCoreApplication::applicationDirPath()+"/Mycss/summaryui_style.css";
    QFile file(s);
    if(!file.open(QIODeviceBase::ReadOnly))
    {
        return;
    }
    QString m=file.readAll();
    this->setStyleSheet(m);
}

void SummaryUI::on__search_clicked()
{
    rds.clear();
    QDate bd=ui->_beginDate->date();
    QString begindate=QString::number(bd.year())+"-"
            +QString::number(bd.month())+"-"
            +QString::number(bd.day());
    QDate ed=ui->_endDate->date();
    QString enddate=QString::number(ed.year())+"-"
            +QString::number(ed.month())+"-"
            +QString::number(ed.day());
    QString code=ui->_code->text();
    if(ui->_export->isChecked())
        type="export";
    else
        type="import";
    if(begindate>enddate)
    {
        QMessageBox::warning(this,"查找失败","终止日期不能早于起始日期！");
        return;
    }
    if(code.isEmpty())//在Records文件夹中找
    {
        QString path=QCoreApplication::applicationDirPath()+"//data//Records//"+type;
        QDir dir(path);
        if(!dir.exists())//路径不存在
        {
            QMessageBox::warning(this,"查找失败","原文件不存在！请退出程序！");
            return;
        }
        dir.setFilter(QDir::Files|QDir::NoDotAndDotDot);//设置entryInfoList()筛选方式:只获得文件
        QFileInfoList list=dir.entryInfoList();
        for(int i=0;i<list.size();i++)
        {
            //判断是否为目标文件
            if(list[i].suffix()!="dat")//必须是.dat文件
                continue;
            if(!list[i].baseName().startsWith("202"))
                continue;
            //筛选
            if(list[i].baseName()>=begindate&&list[i].baseName()<=enddate)
            {
                QFile file(list[i].absoluteFilePath());
                if(!file.open(QIODeviceBase::ReadOnly))
                {
                    QMessageBox::warning(this,"查找失败",list[i].fileName()+"文件打不开！");
                    return;
                }
                QString content;
                QByteArray ba;
                while(!file.atEnd())
                {
                    ba =file.readLine();
                    content = getCorrectUnicode(ba);
                    QTextStream sin(&content);
                    Record rd;
                    sin>>rd.date>>rd.code>>rd.name>>rd.i_e>>rd.unit>>rd.number;
                    rds.append(rd);
                }
                file.close();

            }

        }
    }
    else//在产品文件夹中找
    {
        if(code.contains(" "))
        {
            QMessageBox::warning(this,"查找失败","物料编码不能有空格!");
            return;
        }
        //判断是否有汉字
        for(auto& i:code)
        {
            ushort u=i.unicode();
            if(u >= 0x4E00 && u <= 0x9FA5)
            {
                QMessageBox::warning(this,"查找失败","物料编码不能有汉字!");
                return;
            }
        }
        //读取文件
        QString path=QCoreApplication::applicationDirPath()+"//data//Products//__"+code;
        QFile file(path+"//"+type+".dat");
        if(!file.open(QIODeviceBase::ReadOnly))
        {
            QMessageBox::warning(this,"查找失败",QString("未找到该产品%1文件！").arg(type));
            return;
        }
        QString content;
        QByteArray ba;
        while(!file.atEnd())
        {
            ba =file.readLine();
            content = getCorrectUnicode(ba);
            QTextStream sin(&content);
            Record rd;
            sin>>rd.date;
            if(rd.date>=begindate&&rd.date<=enddate)
            {
                sin>>rd.code>>rd.name>>rd.i_e>>rd.unit>>rd.number;
                rds.append(rd);
            }
            if(rd.date>enddate)
                break;

        }
        file.close();
    }
    showtable(type);
}


void SummaryUI::on__create_clicked()
{
    if(rds.isEmpty())
    {
        QMessageBox::warning(this,"生成失败","未选择内容");
        return;
    }
    //获得要保存的文件的绝对路径
    QString filename = QFileDialog::getSaveFileName(this,"请选择生成文件的位置",QCoreApplication::applicationDirPath(),"EXCEL(*.xlsx)");
    if(filename.isEmpty())
        return;
    QString pyname=QDir::currentPath();
    QFile file(pyname+"\\text.dat");
    file.open(QIODeviceBase::WriteOnly);
    QTextStream fout(&file);
    QString content;
    fout<<type<<"\n";
    fout<<filename<<"\n";
    for(int i=0;i<rds.size();i++)
    {
        content=rds[i].date+" "
                +rds[i].code+" "
                +rds[i].name+" "
                +rds[i].i_e+" "
                +rds[i].unit+" "
                +rds[i].number;
        fout<<content<<"\n";
    }
    file.close();
    QMessageBox::about(this,"","生成成功！");
    //执行python文件
    QProcess p(this);
    p.startDetached(pyname+"\\pyscript"+"\\main.exe");
}



