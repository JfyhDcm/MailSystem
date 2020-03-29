#ifndef EMESSAGEWIDGET_H
#define EMESSAGEWIDGET_H

#include <QWidget>
#include <QString>
#include <QTcpSocket>
#include <QAbstractSocket>
#include <QMessageBox>
#include <QDebug>
namespace Ui {
class EMessageWidget;
}

class EMessageWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EMessageWidget(QWidget *parent = nullptr);
    ~EMessageWidget();
    void paintEvent(QPaintEvent *);

    int order;
    QString numByte;
    QTcpSocket *tcpSocket1;//smtp
    QTcpSocket *tcpSocket2;//pop3
    QString topFrom;
    QString topTo;
    QString topSubject;
    QString topContents;
    void anotherEMessage();
private:
    Ui::EMessageWidget *ui;
private slots:
    void deleJudge();
    void readMessage();
signals:
    void changePage();
    void allHaveread();
};

#endif // EMESSAGEWIDGET_H
