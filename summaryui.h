#ifndef SUMMARYUI_H
#define SUMMARYUI_H

#include <QDialog>
namespace Ui {
class SummaryUI;
}

class Record
{
public:
    QString date;
    QString code;//物料编码
    QString name;//图号名称
    QString i_e;//入/出库数量
    QString unit;//单位
    QString number;//剩余库存
};

class SummaryUI : public QDialog
{
    Q_OBJECT

public:
    QList<Record> rds;
    QString type;
public:
    explicit SummaryUI(QWidget *parent = nullptr);
    ~SummaryUI();
    void showtable(QString type);
    void showtableheader(QString type);
    QString getCorrectUnicode(const QByteArray &ba);
    void setstyle();//设置界面美化

private slots:
    void on__search_clicked();

    void on__create_clicked();


private:
    Ui::SummaryUI *ui;
};

#endif // SUMMARYUI_H
