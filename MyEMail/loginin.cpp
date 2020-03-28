#include "loginin.h"
#include "ui_loginin.h"
#include <QPainter>
#include <QLineEdit>
#include <QDebug>
LoginIn::LoginIn(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginIn)
{
    ui->setupUi(this);
    ui->lineEdit_1->setStyleSheet("QLineEdit{background-color:transparent}");
    ui->lineEdit_2->setStyleSheet("QLineEdit{background-color:transparent}");
    ui->pushButton->setStyleSheet("QPushButton{border-style:groove;border-width: 1px;border-color: white;background:transparent;border-radius: 20px;}\
QPushButton::hover{border-color: black;}\
QPushButton::pressed{border-color: cyan;}");
    this->mw=new MainWindow;
    connect(ui->pushButton,&QPushButton::clicked,[=](){
        if(ui->lineEdit_1->text().isEmpty()||ui->lineEdit_2->text().isEmpty())
        {
            qDebug()<<"密码或账号缺失";
        }
        else
        {
            this->tcpSocket1 = new QTcpSocket(this);
            this->nextAction = &LoginIn::checkConnectState;
            this->expectedReply="220";
            //this->senderMail=ui->lineEdit_1->text();
            this->senderMail="836439982@qq.com";
            //this->authCode=ui->lineEdit_2->text();
            this->authCode="aydqwcqhajjvbfih";
            connect(tcpSocket1, &QTcpSocket::readyRead,this, &LoginIn::readyReadSlot);
            tcpSocket1->connectToHost("smtp.qq.com", 25);


        }

        //this->close();
        //this->mw->show();
    });

}

LoginIn::~LoginIn()
{
    delete ui;
}
void LoginIn::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(rect(),QPixmap(":/icon/ICON/loginbg2.png"),QRectF(290,320,750,600));
}
void LoginIn::readyReadSlot()
{
    QByteArray buffer = tcpSocket1->readAll();
    qDebug()<<"收到服务器回复："<<buffer;
    if(buffer.contains(this->expectedReply.toLatin1())){
        if(nextAction) (this->*nextAction)();
    }
    else{
        QMessageBox::warning(this, "提示", "失败!");
    }
}
void LoginIn::checkConnectState()
{
    if(tcpSocket1->state() == QAbstractSocket::ConnectedState){
        sendHelo();
    }
    else{
        QMessageBox::warning(this, "提示", "连接失败！");
    }
}
void LoginIn::sendHelo()
{
    QString str = "helo qq.com\r\n";
    qDebug()<<"向服务器发送：　" + str;
    tcpSocket1->write(str.toLatin1());
    this->expectedReply = "250";
    this->nextAction = &LoginIn::sendAuthLogin;
}
void LoginIn::sendAuthLogin()
{
    QString str = "auth login\r\n";
    qDebug()<<"向服务器发送：　" + str;
    tcpSocket1->write(str.toLatin1());
    this->expectedReply = "334";
    this->nextAction = &LoginIn::sendUser;
}
void LoginIn::sendUser()
{
    QString str = QString("\r\n").prepend(senderMail.toLatin1().toBase64());
    qDebug()<<"向服务器发送：　" + str;
    tcpSocket1->write(str.toLatin1());
    this->expectedReply = "334 ";
    this->nextAction = &LoginIn::sendPassword;
}
void LoginIn::sendPassword()
{

    QString str = QString("\r\n").prepend(authCode.toLatin1().toBase64());
    qDebug()<<"向服务器发送：　" + str;
    tcpSocket1->write(str.toLatin1());
    this->expectedReply = "235";
    this->nextAction =NULL; //&LoginIn::sendMailFrom;

//    QByteArray buffer = tcpSocket1->readAll();
//    if(buffer.contains(this->expectedReply.toLatin1())){
//         QMessageBox::warning(this, "提示", "成功!");
//    }
//    else{
//         QMessageBox::warning(this, "提示", "失败!");
//    }
}
void LoginIn::loginsuccessful()
{
    disconnect(tcpSocket1, &QTcpSocket::readyRead,this, &LoginIn::readyReadSlot);
    this->close();
    this->mw->show();
}
