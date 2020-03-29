#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QMessageBox>
#include <QDebug>
#include <QString>
#include "emessagewidget.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    Ui::MainWindow *ui;
    QString senderMail; //发送方邮箱地址
    QString senderName;//发送方@前名字
    QString authCode; //发送方邮箱授权码
    QString receiveMail; //接收方邮箱地址
    QString subject;
    QString contents;
    QString expectedReply; //期待收到的应答
    void (MainWindow::*nextAction)(); //收到正确应答后下一步要执行的方法
    QTcpSocket *tcpSocket1;
    QTcpSocket *tcpSocket2;

    void anotherMainWindow();
private:
    void sendData();
    void sendContents();
    void sendMailFrom();
    void sendMailTo();
    void sendSuccessfully();

    int messageNum;
    QWidget *scrollCanvas;
private slots:
    void readyReadSlot1();
    //void readyReadSlot2();
    void getNum();
    void drawMessage();

};
#endif // MAINWINDOW_H
