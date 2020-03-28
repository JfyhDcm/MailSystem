#ifndef LOGININ_H
#define LOGININ_H

#include <QWidget>
#include <QString>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QMessageBox>
#include <QDebug>
#include "mainwindow.h"
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
    QString authCode; //发送方邮箱授权码
    QString receiveMail; //接收方邮箱地址

    QString title; //邮件标题
    QString content; //邮件正文
    QString expectedReply; //期待收到的应答
    void (LoginIn::*nextAction)(); //收到正确应答后下一步要执行的方法
    QTcpSocket *tcpSocket1;

    void sendHelo();
    void checkConnectState();
    void sendAuthLogin();
    void sendUser();
    void sendPassword();
    void loginsuccessful();
private slots:
    void readyReadSlot();
};

#endif // LOGININ_H
