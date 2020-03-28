#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "loginin.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //this->setWindowIcon(QPixmap(":/icon/ICON/loginicon.png"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

