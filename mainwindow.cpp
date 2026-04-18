#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);

    setupDynamicUI();
    applyStyles();

    connect(ui->initBtn, &QPushButton::clicked, this, &MainWindow::onInitializeClicked);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    
    // Set default values
    ui->portCombo->addItem("COM3 - SERIAL_DEV");
    ui->portCombo->addItem("ttyACM0");
    ui->baudCombo->addItem("9600");
    ui->baudCombo->addItem("115200");
    
    // Mock terminal initial data
    ui->textBrowser->append("<span style='color: #44aaff;'>[14:22:01.03] TX >> 0x41 0x54 (AT+RST)</span>");
    ui->textBrowser->append("<span style='color: #44aaff;'>[14:22:01.45] RX << 0x4F 0x4B (OK)</span>");
    ui->textBrowser->append("<span style='color: #0f0; background-color: #004400;'>[14:22:02.35] INTERRUPT >> PIN_01 HIGH</span>");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupDynamicUI()
{
    // Generate the temperature chart bars dynamically
    ui->chartLayout->setSpacing(2);
    ui->chartLayout->setAlignment(Qt::AlignBottom);
    for (int i = 0; i < 15; ++i) {
        QFrame *bar = new QFrame(this);
        bar->setFixedWidth(15);
        bar->setFixedHeight(20 + i * 5);
        int b = 255 - (i * 10);
        bar->setStyleSheet(QString("background-color: rgb(%1, %1, 255); border: 1px solid black;").arg(b));
        ui->chartLayout->addWidget(bar);
    }
    
    // Setup GPIO buttons dynamically
    auto setupGpio = [this](QFrame* gBox, QString name, bool active) {
        gBox->setFixedSize(90, 110);
        if (active) gBox->setObjectName("panelBoxActive");
        else gBox->setObjectName("panelBox");
        QVBoxLayout *gL = new QVBoxLayout(gBox);
        gL->setAlignment(Qt::AlignCenter);
        QLabel *icon = new QLabel(active ? "◉" : "◯", this);
        icon->setObjectName("iconTextBig");
        if(active) icon->setStyleSheet("color: blue;");
        QLabel *lbl = new QLabel(name, this);
        QLabel *stat = new QLabel(active ? "PRESSED" : "RELEASED", this);
        if (active) stat->setStyleSheet("background-color: blue; color: white; font-weight: bold; padding: 2px;");
        else stat->setStyleSheet("color: gray; font-size: 10px;");
        gL->addWidget(icon, 0, Qt::AlignCenter);
        gL->addWidget(lbl, 0, Qt::AlignCenter);
        gL->addWidget(stat, 0, Qt::AlignCenter);
    };
    setupGpio(ui->g1, "BTN_01", true);
    setupGpio(ui->g2, "BTN_02", false);
    setupGpio(ui->g3, "BTN_03", false);
    setupGpio(ui->g4, "BTN_04", false);
}

void MainWindow::applyStyles()
{
    QString qss = R"(
        QWidget {
            font-family: 'Monospace';
            font-size: 12px;
            background-color: #ffffff;
            color: #000000;
        }
        QFrame#topBar {
            border-bottom: 4px solid black;
        }
        QLabel#topTitle {
            font-size: 20px;
            font-weight: bold;
            color: #0022cc;
            padding-left: 20px;
        }
        QLabel#navSystem {
            font-weight: bold;
            padding: 5px 10px;
            border-bottom: 3px solid blue;
            color: blue;
        }
        QLabel#navTerminal, QLabel#navLogs {
            font-weight: bold;
            padding: 5px 10px;
        }
        QLabel#iconSettings, QLabel#iconPower {
            font-size: 24px;
            padding: 0 10px;
        }
        QLabel#userIcon {
            font-size: 32px;
        }
        QFrame#sideBar {
            border-right: 4px solid black;
            background-color: #ffffff;
        }
        QLabel#userTitle {
            font-weight: bold;
            color: blue;
            font-size: 14px;
        }
        QLabel#userSub {
            font-size: 10px;
            color: gray;
        }
        QLabel#label_com, QLabel#label_baud {
            font-size: 10px;
            font-weight: bold;
            color: #333;
        }
        QComboBox {
            border: 2px solid black;
            padding: 5px;
            background-color: #fff;
            font-weight: bold;
        }
        QPushButton {
            border: 2px solid black;
            padding: 10px;
            font-weight: bold;
            background-color: #fff;
        }
        QPushButton#initBtn {
            background-color: #0022cc;
            color: white;
            border: 2px solid black;
            border-bottom: 4px solid black;
            border-right: 4px solid black;
        }
        QPushButton#initBtn:pressed {
            border-bottom: 2px solid black;
            border-right: 2px solid black;
            margin-top: 2px;
            margin-left: 2px;
        }
        QPushButton#newModuleBtn {
            background-color: #fff;
            border: 2px solid black;
            border-bottom: 4px solid black;
            border-right: 4px solid black;
            margin: 10px;
        }
        QPushButton#menuHard, QPushButton#menuMod, QPushButton#menuConn, QPushButton#menuDiag {
            border: none;
            border-bottom: 1px solid #ccc;
            text-align: left;
            padding-left: 20px;
        }
        QPushButton#menuDash {
            border: none;
            border-bottom: 1px solid #ccc;
            text-align: left;
            padding-left: 20px;
            background-color: #0022cc;
            color: white;
        }
        QLabel#dashTitle {
            font-size: 36px;
            font-weight: 900;
            letter-spacing: 2px;
        }
        QLabel#dashStatus {
            font-size: 12px;
            font-weight: bold;
            color: gray;
        }
        QLabel#valCpu, QLabel#valMem, QLabel#valRef {
            font-size: 18px;
            font-weight: bold;
            color: blue;
        }
        QLabel#lblCpu, QLabel#lblMem, QLabel#lblRef {
            font-size: 10px;
            font-weight: bold;
            color: #333;
        }
        QFrame#userInfoBox, QFrame#tempPanel, QFrame#actPanel, QFrame#gpioPanel {
            border: 4px solid black;
            background-color: #fff;
            padding: 10px;
            border-bottom: 8px solid black;
            border-right: 8px solid black;
        }
        QFrame#panelBoxActive {
            border: 4px solid blue;
            background-color: #fff;
            padding: 10px;
            border-bottom: 8px solid blue;
            border-right: 8px solid blue;
        }
        QFrame#panelBox {
            border: 4px solid black;
            background-color: #fff;
            padding: 10px;
            border-bottom: 8px solid black;
            border-right: 8px solid black;
        }
        QLabel#tempTitle, QLabel#actTitle, QLabel#gpioTitle {
            font-size: 14px;
            font-weight: bold;
            margin-bottom: 10px;
        }
        QLabel#actTitle {
            color: red;
        }
        QLabel#tempValue {
            font-size: 64px;
            font-weight: 900;
        }
        QFrame#termPanel {
            background-color: #222;
            border: 4px solid black;
            padding: 10px;
        }
        QLabel#termTitle {
            color: white;
            font-weight: bold;
        }
        QLabel#termClear, QLabel#termExport {
            color: white;
            font-size: 10px;
            text-decoration: underline;
        }
        QTextBrowser#textBrowser {
            background-color: #222;
            color: #ccc;
            border: none;
            font-family: 'Monospace';
        }
        QLabel#tgl1_sym, QLabel#tgl3_sym { color: blue; font-size: 20px; }
        QLabel#tgl2_sym { color: gray; font-size: 20px; }
        QLabel#tgl1_val, QLabel#tgl3_val { background-color: blue; color: white; padding: 5px; border: 2px solid black; font-weight: bold;}
        QLabel#tgl2_val { background-color: #ddd; color: black; padding: 5px; border: 2px solid black; font-weight: bold;}
    )";
    this->setStyleSheet(qss);
}

void MainWindow::onInitializeClicked()
{
    QString portName = ui->portCombo->currentText();
    if(portName.contains("ttyACM0")) {
        serial->setPortName("ttyACM0");
    } else {
        ui->textBrowser->append("<span style='color: yellow;'>[SYS] Attempting to open " + portName + "</span>");
    }
    
    int baud = ui->baudCombo->currentText().toInt();
    serial->setBaudRate(baud);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if(serial->open(QIODevice::ReadOnly)){
        ui->textBrowser->append("<span style='color: #0f0;'>[SYS] Connection : Port Opened at " + QString::number(baud) + " bps</span>");
    }
    else{
        ui->textBrowser->append("<span style='color: red;'>[ERR] Failed to open port!</span>");
    }
}

void MainWindow::readData() {
    QByteArray data = serial->readAll();
    if (!data.isEmpty()) {
        QString text = QString::fromUtf8(data);
        ui->textBrowser->append("<span style='color: #0aa;'>[RX] " + text + "</span>");
    }
}
