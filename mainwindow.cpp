#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    serial = new QSerialPort(this);

    //setupDynamicUI();
    //applyStyles();

    ui->initBtn->setProperty("connected", false);

    connect(ui->initBtn, &QPushButton::clicked, this, &MainWindow::onInitializeClicked);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    
    // Set default values
    ui->portCombo->addItem("ttyACM0");
    ui->portCombo->addItem("ttyACM1");
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
        QPushButton#initBtn[connected="false"] {
            background-color: gray;
            color: white;
            border: 2px solid black;
            border-bottom: 4px solid black;
            border-right: 4px solid black;
        }
        QPushButton#initBtn[connected="true"] {
            background-color: #00aa00;
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
    if (serial->isOpen()) {
        serial->close();
        ui->initBtn->setText("Disconnected");
        ui->initBtn->setProperty("connected", false);
        ui->initBtn->style()->unpolish(ui->initBtn);
        ui->initBtn->style()->polish(ui->initBtn);
        ui->textBrowser->append("<span style='color: yellow;'>[SYS] Port Closed</span>");
        ui->dashStatus->setText("STATUS: <span style='color:gray;'>DISCONNECTED</span>");
        return;
    }

    QString portName = ui->portCombo->currentText();
    if(portName.contains("ttyACM0")) {
        serial->setPortName("ttyACM0");
    } else {
        serial->setPortName(portName);
        ui->textBrowser->append("<span style='color: yellow;'>[SYS] Attempting to open " + portName + "</span>");
    }
    
    int baud = ui->baudCombo->currentText().toInt();
    serial->setBaudRate(baud);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if(serial->open(QIODevice::ReadWrite)){
        ui->textBrowser->append("<span style='color: #0f0;'>[SYS] Connection : Port Opened at " + QString::number(baud) + " bps</span>");
        ui->initBtn->setText("Connected");
        ui->initBtn->setProperty("connected", true);
        ui->initBtn->style()->unpolish(ui->initBtn);
        ui->initBtn->style()->polish(ui->initBtn);
        ui->dashStatus->setText("STATUS: <span style='color:blue;'>CONNECTED_ACTIVE</span> / UPTIME: 04:12:33");
    }
    else{
        ui->textBrowser->append("<span style='color: red;'>[ERR] Failed to open port!</span>");
        ui->initBtn->setText("Disconnected");
        ui->initBtn->setProperty("connected", false);
        ui->initBtn->style()->unpolish(ui->initBtn);
        ui->initBtn->style()->polish(ui->initBtn);
    }
}

void MainWindow::readData() {
    m_serialBuffer.append(serial->readAll());

    // 데이터가 '$'와 '#'를 모두 포함하고 있는지 확인
    while (m_serialBuffer.contains('$') && m_serialBuffer.contains('#')) {
        int start = m_serialBuffer.indexOf('$');
        int end = m_serialBuffer.indexOf('#', start);

        if (end == -1) break; // 불완전한 패킷

        // '$'와 '#' 사이의 데이터 추출
        QByteArray packet = m_serialBuffer.mid(start + 1, end - start - 1);
        m_serialBuffer.remove(0, end + 1);

        if (!packet.isEmpty()) {
            QString packetStr = QString::fromUtf8(packet);
            ui->textBrowser->append("<span style='color: #0aa;'>[RX] $" + packetStr + "#</span>");
            parseProtocol(packetStr);
        }
    }
}

void MainWindow::parseProtocol(const QString &data) {
    // 형식: count,id:type:val,id:type:val
    QStringList parts = data.split(',');
    if (parts.size() < 2) return;

    // parts[0]은 데이터 갯수, parts[1]부터 실제 데이터
    for (int i = 1; i < parts.size(); ++i) {
        QStringList item = parts[i].split(':');
        if (item.size() < 3) continue;

        int sensorId = item[0].toInt();
        // int type = item[1].toInt(); // DataType (사용하지 않음)
        QString valueStr = item[2];

        if (sensorId == ID_ENV_TEMP) {
            // 온도 업데이트
            ui->tempValue->setText(valueStr + " °C");
        } else if (sensorId == ID_OUT_LED_STATE) {
            // LED 상태 업데이트 (STATUS_RED에 적용)
            bool isOn = (valueStr.toInt() != 0);
            if (isOn) {
                ui->tgl1_val->setText("[ ON ]");
                ui->tgl1_val->setStyleSheet("background-color: blue; color: white; padding: 5px; border: 2px solid black; font-weight: bold;");
                ui->tgl1_sym->setStyleSheet("color: blue; font-size: 20px;");
            } else {
                ui->tgl1_val->setText("[ OFF ]");
                ui->tgl1_val->setStyleSheet("background-color: #ddd; color: black; padding: 5px; border: 2px solid black; font-weight: bold;");
                ui->tgl1_sym->setStyleSheet("color: gray; font-size: 20px;");
            }
        }
    }
}
