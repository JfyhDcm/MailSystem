#include "loginin.h"
#include "ui_loginin.h"
#include <QPainter>
#include <QLineEdit>
#include <QDebug>
#include <QTimer>
bool haveconnected=false;
LoginIn::LoginIn(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoginIn)
{
    ui->setupUi(this);
    this->setWindowIcon(QPixmap(":/icon/ICON/ficon.png"));
    ui->lineEdit_1->setStyleSheet("QLineEdit{background-color:transparent}");
    ui->lineEdit_2->setStyleSheet("QLineEdit{background-color:transparent}");
    ui->pushButton->setStyleSheet("QPushButton{border-style:groove;border-width: 1px;border-color: white;background:transparent;border-radius: 20px;}\
QPushButton::hover{border-color: black;}\
QPushButton::pressed{border-color: cyan;}");
    this->mw=new MainWindow;
            //this->mw->show();
    havesendfail=false;
    smtpisok=false;
    pop3isok=false;
    connect(ui->pushButton,&QPushButton::clicked,[=](){
        if(ui->lineEdit_1->text().isEmpty()||ui->lineEdit_2->text().isEmpty())
        {
            if(ui->lineEdit_2->text().isEmpty())
            QMessageBox::warning(this, "提示", "请输入密码!");
            else
            QMessageBox::warning(this, "提示", "请输入账号!");
        }
        else
        {
            this->tcpSocket1 = new QTcpSocket(this);
            this->tcpSocket2 = new QTcpSocket(this);

            connect(this,&LoginIn::smtpisokf,this,&LoginIn::loginsuccessful);
            connect(this,&LoginIn::pop3isokf,this,&LoginIn::loginsuccessful);


            this->senderMail=ui->lineEdit_1->text();
            //this->senderMail="836439982@qq.com";
            this->authCode=ui->lineEdit_2->text();
            //this->authCode="aydqwcqhajjvbfih";

            this->senderName="";
            for(int i=0;i<this->senderMail.length();i++)
                if(this->senderMail[i]!='@')
                    this->senderName+=this->senderMail[i];
                else
                    break;

            this->nextAction1 = &LoginIn::checkConnectState1;
            this->expectedReply1="220";
            this->expectedReply2 = "OK";
            this->nextAction2 =&LoginIn::checkConnectState2;

            connect(tcpSocket1, &QTcpSocket::readyRead,this, &LoginIn::readyReadSlot1);
            tcpSocket1->connectToHost("smtp.qq.com", 25);
            connect(tcpSocket2, &QTcpSocket::readyRead,this, &LoginIn::readyReadSlot2);
            tcpSocket2->connectToHost("pop.qq.com", 110);



            QTimer *timelimit=new QTimer(this);
            timelimit->start(10000);
            connect(timelimit,&QTimer::timeout,[=](){
                if(!haveconnected&&!havesendfail)
                {
                    disconnect(tcpSocket1, &QTcpSocket::readyRead,this, &LoginIn::readyReadSlot1);
                    disconnect(tcpSocket2, &QTcpSocket::readyRead,this, &LoginIn::readyReadSlot2);
                    QMessageBox::warning(this, "提示", "连接超时!");
                }
                timelimit->stop();
            });

        }
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
void LoginIn::readyReadSlot1()
{
    QByteArray buffer = tcpSocket1->readAll();
    qDebug()<<"收到服务器回复："<<buffer;
    if(buffer.contains(this->expectedReply1.toLatin1())){
        if(nextAction1) (this->*nextAction1)();
        else{
            smtpisok=true;
            emit smtpisokf();
        }
    }
    else{
        havesendfail=true;
        QMessageBox::warning(this, "提示", "连接失败1!");
    }
}
void LoginIn::checkConnectState1()
{
    if(tcpSocket1->state() == QAbstractSocket::ConnectedState){
        sendHelo();
    }
    else{
        QMessageBox::warning(this, "提示", "连接失败2！");
        havesendfail=true;
    }
}
void LoginIn::sendHelo()
{
    QString str = "helo qq.com\r\n";
    qDebug()<<"向服务器发送：　" + str;
    tcpSocket1->write(str.toLatin1());
    this->expectedReply1 = "250";
    this->nextAction1 = &LoginIn::sendAuthLogin;
}
void LoginIn::sendAuthLogin()
{
    QString str = "auth login\r\n";
    qDebug()<<"向服务器发送：　" + str;
    tcpSocket1->write(str.toLatin1());
    this->expectedReply1 = "334";
    this->nextAction1 = &LoginIn::sendUser;
}
void LoginIn::sendUser()
{
    QString str = QString("\r\n").prepend(senderMail.toLatin1().toBase64());
    qDebug()<<"向服务器发送：　" + str;
    tcpSocket1->write(str.toLatin1());
    this->expectedReply1 = "334 ";
    this->nextAction1 = &LoginIn::sendPassword;
}
void LoginIn::sendPassword()
{
    QString str = QString("\r\n").prepend(authCode.toLatin1().toBase64());
    qDebug()<<"向服务器发送：　" + str;
    tcpSocket1->write(str.toLatin1());
    this->expectedReply1 = "235";
    this->nextAction1 =NULL;
}
void LoginIn::readyReadSlot2()
{
    QByteArray buffer = tcpSocket2->readAll();
    qDebug()<<"收到服务器回复："<<buffer;
    if(buffer.contains(this->expectedReply2.toLatin1())){
        if(nextAction2) (this->*nextAction2)();
        else{
            pop3isok=true;
            emit pop3isokf();
        }
    }
    else{
        QMessageBox::warning(this, "提示", "连接失败3!");
        havesendfail=true;
    }
}
void LoginIn::checkConnectState2()
{
    if(tcpSocket2->state() == QAbstractSocket::ConnectedState){
        sendUserName();
    }
    else{
        QMessageBox::warning(this, "提示", "连接失败4！");
        havesendfail=true;
    }
}
void LoginIn::sendUserName()
{
    QString str = "user "+this->senderName+"\r\n";
    qDebug()<<"向服务器发送：　" + str;
    tcpSocket2->write(str.toLatin1());
    this->expectedReply2 = "OK";
    this->nextAction2 = &LoginIn::sendPass;
}
void LoginIn::sendPass()
{
    QString str = "pass "+this->authCode+"\r\n";
    qDebug()<<"向服务器发送：　" + str;
    tcpSocket2->write(str.toLatin1());
    this->expectedReply2 = "OK";
    this->nextAction2 = NULL;

}
void LoginIn::loginsuccessful()
{
    if(!smtpisok||!pop3isok)
        return;
    disconnect(tcpSocket1, &QTcpSocket::readyRead,this, &LoginIn::readyReadSlot1);
    disconnect(tcpSocket2, &QTcpSocket::readyRead,this, &LoginIn::readyReadSlot2);
    this->mw->senderMail=this->senderMail; //发送方邮箱地址
    this->mw->senderName=this->senderName;//发送方@前名字
    this->mw->authCode=this->authCode; //发送方邮箱授权码
    this->mw->receiveMail=this->receiveMail; //接收方邮箱地址
    this->mw->tcpSocket1=this->tcpSocket1;//smtp
    this->mw->tcpSocket2=this->tcpSocket2;//pop3

    haveconnected=true;
    this->hide();
    this->mw->show();
    this->mw->anotherMainWindow();
}
