#include <QString>
#include <QDebug>
#include <QFile>
#include <QIODevice>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loginin.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFile file(":/qss/QSS/Ubuntu.qss",this);
    file.open(QFile::ReadOnly);
    QString stylesheet = QLatin1String(file.readAll());
    this->setStyleSheet(stylesheet);

    ui->stackedWidget->setCurrentIndex(0);
    connect(ui->pushButton_2,&QPushButton::clicked,[=](){
        ui->stackedWidget->setCurrentIndex(0);
    });
    connect(ui->pushButton_3,&QPushButton::clicked,[=](){
        ui->stackedWidget->setCurrentIndex(1);
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::anotherMainWindow()
{
    this->setWindowIcon(QPixmap(":/icon/ICON/loginicon.png"));
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
    qDebug()<<"YEEEEEEEES!";
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

}
