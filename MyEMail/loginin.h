#ifndef LOGININ_H
#define LOGININ_H

#include <QWidget>
#include <QString>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QMessageBox>
#include <QDebug>
#include "mainwindow.h"

extern bool haveconnected;

namespace Ui {
class LoginIn;
}

class LoginIn : public QWidget
{
    Q_OBJECT

public:
    explicit LoginIn(QWidget *parent = nullptr);
    ~LoginIn();
    void paintEvent(QPaintEvent *);
private:
    Ui::LoginIn *ui;
    QString account,password;
    MainWindow *mw;
    QString senderMail; //发送方邮箱地址
    QString senderName;//发送方@前名字
    QString authCode; //发送方邮箱授权码
    QString receiveMail; //接收方邮箱地址

    QString title; //邮件标题
    QString content; //邮件正文
    QString expectedReply1; //期待收到的应答
    QString expectedReply2; //期待收到的应答
    void (LoginIn::*nextAction1)(); //收到正确应答后下一步要执行的方法
    void (LoginIn::*nextAction2)(); //收到正确应答后下一步要执行的方法
    QTcpSocket *tcpSocket1;//smtp
    QTcpSocket *tcpSocket2;//pop3
    bool havesendfail;
    bool smtpisok;
    bool pop3isok;
    void sendHelo();
    void checkConnectState1();
    void checkConnectState2();
    void sendAuthLogin();
    void sendUser();
    void sendPassword(); 
    void sendUserName();
    void sendPass();
private slots:
    void readyReadSlot1();
    void readyReadSlot2();
    void loginsuccessful();
signals:
    void smtpisokf();
    void pop3isokf();
};

#endif // LOGININ_H
