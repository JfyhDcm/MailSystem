#include <QString>
#include <QDebug>
#include <QFile>
#include <QIODevice>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loginin.h"
#include "emessagewidget.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFile file(":/qss/QSS/Ubuntu.qss",this);
    file.open(QFile::ReadOnly);
    QString stylesheet = QLatin1String(file.readAll());
    this->setStyleSheet(stylesheet);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::anotherMainWindow()
{
    this->setWindowIcon(QPixmap(":/icon/ICON/loginicon.png"));
    ui->stackedWidget->setCurrentIndex(0);
    ui->lineEdit_1->setText(this->senderMail);
    connect(ui->pushButton_1,&QPushButton::clicked,[=](){
        if(ui->label_2->text().isEmpty()||ui->label_3->text().isEmpty()||\
                ui->label_3->text().isEmpty()||ui->textEdit->toPlainText().isEmpty()){
            qDebug()<<"请将信件填写完整！";
                      return;
        }
        this->receiveMail=ui->lineEdit_2->text();
        this->subject=ui->lineEdit_3->text();
        this->contents=ui->textEdit->toPlainText();
        connect(tcpSocket1, &QTcpSocket::readyRead,this, &MainWindow::readyReadSlot1);
        sendMailFrom();
    });

    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(ui->pushButton_3,&QPushButton::clicked,[=](){
        QString str = "stat \r\n";
        qDebug()<<"向服务器2发送：　" + str;
        tcpSocket2->write(str.toLatin1());
        connect(tcpSocket2, &QTcpSocket::readyRead,this,&MainWindow::getNum);
        ui->stackedWidget->setCurrentIndex(1);
    });
}
void MainWindow::sendMailFrom()
{
    QString str="mail from:<"+this->senderMail+">\r\n";
    this->expectedReply="250";
    this->nextAction =&MainWindow::sendMailTo;
    qDebug()<<"向服务器发送：　" + str;
    tcpSocket1->write(str.toLatin1());
}
void MainWindow::sendMailTo()
{
    QString str="rcpt to:<"+this->receiveMail+">\r\n";
    this->expectedReply="250";
    this->nextAction =&MainWindow::sendData;
    qDebug()<<"向服务器发送：　" + str;
    tcpSocket1->write(str.toLatin1());
}
void MainWindow::sendData()
{
    QString str="data\r\n";
    this->expectedReply="354";
    this->nextAction =&MainWindow::sendContents;
    qDebug()<<"向服务器发送：　" + str;
    tcpSocket1->write(str.toLatin1());
}
void MainWindow::sendContents()
{
    QString str="From: "+this->senderMail+" \r\n";
    str+="To: "+this->receiveMail+" \r\n";
    str+="Subject: "+this->subject+" \r\n";
    str+=this->contents+"\r\n.\r\n";
    this->expectedReply="250";
    this->nextAction =&MainWindow::sendSuccessfully;
    qDebug()<<"向服务器发送：　" + str;
    tcpSocket1->write(str.toLatin1());
}
void MainWindow::sendSuccessfully()
{
    disconnect(tcpSocket1, &QTcpSocket::readyRead,this, &MainWindow::readyReadSlot1);
    ui->lineEdit_3->setText("");
    ui->textEdit->setText("");
    qDebug()<<"send message successfully!";
}
void MainWindow::readyReadSlot1()
{
    QByteArray buffer = tcpSocket1->readAll();
    qDebug()<<"收到服务器回复："<<buffer;
    if(buffer.contains(this->expectedReply.toLatin1())){
        if(nextAction) (this->*nextAction)();
    }
    else{
        QMessageBox::warning(this, "提示", "失败5!");
    }
}
void MainWindow::readyReadSlot2()
{
    qDebug()<<"draw!";
}
void MainWindow::getNum()
{
    QString num="",str;
    QByteArray buffer = tcpSocket2->readAll();
    str=QString(buffer);
    for(int i=4;i<str.length();i++)
    {
        if(str[i]==' ')
            break;
        else
            num+=str[i];
    }
    this->messageNum=num.toInt();
    //qDebug()<<this->messageNum;
    disconnect(tcpSocket2, &QTcpSocket::readyRead,this, &MainWindow::getNum);
    str = "list \r\n";
    qDebug()<<"向服务器2发送：　" + str;
    tcpSocket2->write(str.toLatin1());
    connect(tcpSocket2, &QTcpSocket::readyRead,this,&MainWindow::drawMessage);
}
void MainWindow::drawMessage()
{
    disconnect(tcpSocket2, &QTcpSocket::readyRead,this,&MainWindow::drawMessage);
    this->scrollCanvas=new QWidget(ui->scrollArea);
    ui->scrollArea->setWidget(this->scrollCanvas);
    this->scrollCanvas->setFixedSize(600,this->messageNum*85+15);
    QString str;
    QByteArray buffer = tcpSocket2->readAll();
    str=QString(buffer);

    int k=7;
    for(int i=1;i<=this->messageNum;i++)
    {
        QString num="";
        while(str[k]!='\xd')
        {
            num+=str[k];
            k++;
        }
        k+=4;

        EMessageWidget *emw=new EMessageWidget;
        emw->setParent(scrollCanvas);
        emw->order=i;
        emw->numByte=num;
        emw->tcpSocket1=this->tcpSocket1;
        emw->tcpSocket2=this->tcpSocket2;
        connect(emw,&EMessageWidget::changePage,[=](){
            ui->stackedWidget->setCurrentIndex(2);
        });
        connect(emw,&EMessageWidget::allHaveread,[=](){
            ui->lineEdit_4->setText(emw->topFrom);
            ui->lineEdit_5->setText(emw->topTo);
            ui->lineEdit_6->setText(emw->topSubject);
            ui->textEdit_2->setText(emw->topContents);
        });
        emw->anotherEMessage();
        emw->move(10,(i-1)*85+10);
        emw->show();
    }

}
