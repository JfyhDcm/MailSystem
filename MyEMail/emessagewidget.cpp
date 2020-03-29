#include "emessagewidget.h"
#include "ui_emessagewidget.h"
#include <QPainter>
EMessageWidget::EMessageWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::EMessageWidget)
{
    ui->setupUi(this);
}

EMessageWidget::~EMessageWidget()
{
    delete ui;
}
void EMessageWidget::anotherEMessage()
{
    ui->label_1->setNum(order);
    if(numByte.toInt()<1024)
    ui->label_2->setText(numByte+"Bytes");
    else if(numByte.toInt()<1024*1024)
    ui->label_2->setText(static_cast<QString>(numByte.toInt()/1024)+"KB");
    else if(numByte.toInt()<1024*1024*1024)
    ui->label_2->setText(static_cast<QString>(numByte.toInt()/(1024*1024))+"MB");

    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        QString str="dele "+QString::number(order)+" \r\n";
        qDebug()<<"向服务器发送：　" + str;
        tcpSocket2->write(str.toLatin1());
        connect(tcpSocket2, &QTcpSocket::readyRead,this, &EMessageWidget::deleJudge);
    });

    connect(ui->pushButton_1,&QPushButton::clicked,[=](){
        // 读取信件=
        QString str="top "+QString::number(order)+" \r\n";
        qDebug()<<"向服务器发送：　" + str;
        tcpSocket2->write(str.toLatin1());
        connect(tcpSocket2, &QTcpSocket::readyRead,this, &EMessageWidget::readMessage);
        emit changePage();
    });
}
void EMessageWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawRect(0,0,this->width(),this->height());
}
void EMessageWidget::deleJudge()
{
    disconnect(tcpSocket2, &QTcpSocket::readyRead,this, &EMessageWidget::deleJudge);
    QByteArray buffer = tcpSocket2->readAll();
    qDebug()<<"收到服务器回复："<<buffer;
    if(buffer.contains("OK")){
        qDebug()<<"成功删除！";
        ui->pushButton_1->setEnabled(false);
        ui->pushButton_2->setEnabled(false);
    }
    else{
        QMessageBox::warning(this, "提示", "失败5!");
    }
}
void EMessageWidget::readMessage()
{
    disconnect(tcpSocket2, &QTcpSocket::readyRead,this, &EMessageWidget::readMessage);
    QByteArray buffer = tcpSocket2->readAll();
    qDebug()<<"收到服务器回复："<<buffer;
    QString str=buffer;
    this->topTo="";
    this->topFrom="";
    this->topSubject="";
    this->topContents="";

    int k=str.indexOf("From: ",0);
    qDebug()<<k<<endl;
    QString temp;

    for(int i=k+QString("From: ").length();;i++)
        if(str[i]!=' ')
            this->topFrom+=str[i];
        else
            break;

    k=str.indexOf("To: ",0);
    for(int i=k+QString("To: ").length();;i++)
        if(str[i]!=' ')
            this->topTo+=str[i];
        else
            break;
    k=str.indexOf("Subject: ",0);
    for(int i=k+QString("Subject: ").length();;i++)
        if(str[i]!=' ')
            this->topSubject+=str[i];
        else{
            k=i+3;break;}


    for(int i=k;i<str.length();i++)
            this->topContents+=str[i];
    this->topContents=this->topContents.left(this->topContents.length()-5);
    emit allHaveread();
}
