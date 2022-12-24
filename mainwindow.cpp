#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMessageBox>
#include<QDir>
#include<QTextCodec>
void traverseFiles(QString& path);
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setstyle();
    QString dir=QCoreApplication::applicationDirPath()+"//data//Products";
    traverseFiles(dir);
    //getData();
}

MainWindow::~MainWindow()
{
    delete ui;
}
QString MainWindow::getCorrectUnicode(const QByteArray &ba)
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

void MainWindow::setstyle()
{
    QString s=QCoreApplication::applicationDirPath()+"/Mycss/mainwindow_style.css";
    QFile file(s);
    if(!file.open(QIODeviceBase::ReadOnly))
    {
        return;
    }
    QString m=file.readAll();
    this->setStyleSheet(m);
}
bool MainWindow::getData()
{
    QString dir=QCoreApplication::applicationDirPath();
    QFile file(dir+"//data//products.txt");
    if(!file.open(QIODeviceBase::ReadOnly))
    {
        QMessageBox::warning(this,"错误","未找到数据文件！");
        return false;
    }
    while(!file.atEnd())
    {
        QString content = file.readLine();
        QTextStream sin(&content);
        Product p;
        sin>>p.name>>p.code>>p.number>>p.location;
        //qDebug()<<content.sliced(sin.pos());//QTextStream.pos返回当前位置
        p.remark=content.sliced(sin.pos());
        wh.addPrd(p);
    }
    file.close();
    return true;
}


void MainWindow::on__import_clicked()
{
    ImportUI* iui = new ImportUI(&wh);
    iui->exec();
}


void MainWindow::on__export_clicked()
{
    ImportUI* eui=new ImportUI(&wh,false);
    eui->exec();
}


void MainWindow::on__warehouse_clicked()
{
    WarehouseUI* wui= new WarehouseUI(&wh);
    wui->exec();
}




void MainWindow::on__summary_clicked()
{
    SummaryUI* sui=new SummaryUI();
    sui->exec();
}

bool MainWindow::traverseFiles(QString& path)
{
    /* 遍历文件夹里的所有路径  */
    QDir dir(path);
    if(!dir.exists())//路径不存在
    {
        QMessageBox::warning(this,"错误","原文件不存在！请退出程序！");
        return false;
    }
    dir.setFilter(QDir::Dirs|QDir::NoDotAndDotDot);//设置entryInfoList()筛选方式:只获得文件夹
    QFileInfoList list=dir.entryInfoList();
    for(int i=0;i<list.size();++i)
    {
        /*读取data文件*/
//        if(list[i].suffix()!="dat")//文件后缀不是dat
//        {
//            continue;
//        }
        if(!list[i].baseName().startsWith("__"))//如果文件夹开头不是__
        {
            continue;
        }
        QFile file(list[i].absoluteFilePath()+"//data.dat");
        if(!file.open(QIODeviceBase::ReadOnly))
        {
            QMessageBox::warning(this,"错误",QString("未找到%1文件夹！").arg(list[i].baseName()));
            continue;
        }
        QByteArray ba =file.readLine();
        QString content = getCorrectUnicode(ba);
        QTextStream sin(&content);
        Product p;
        sin>>p.code>>p.name>>p.number>>p.location;
        //qDebug()<<content.sliced(sin.pos());//QTextStream.pos返回当前位置
        p.remark=content.sliced(sin.pos());
        wh.addPrd(p);
        file.close();
    }
    return true;
}

