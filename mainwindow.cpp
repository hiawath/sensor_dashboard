#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    textBrowser=new QTextBrowser(this);
    setCentralWidget(textBrowser);
    //resize(400,400);

    serial=new QSerialPort(this);
    serial->setPortName("ttyACM0");
    serial->setBaudRate(QSerialPort::Baud9600);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if(serial->open(QIODevice::ReadOnly)){
        textBrowser->append("Connection : Port Opened at 9600 bps");
    }
    else{

        QMessageBox::critical(this,"error", "cannot open port!!");
    }

    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btnRead_clicked()
{

}
void MainWindow::readData() {
    // 버퍼에 있는 모든 데이터 읽기
    QByteArray data = serial->readAll();

    if (!data.isEmpty()) {
        // 문자열로 변환하여 출력 (끝에 커서 유지)
        textBrowser->insertPlainText(QString::fromUtf8(data));
        textBrowser->moveCursor(QTextCursor::End);
    }
}
