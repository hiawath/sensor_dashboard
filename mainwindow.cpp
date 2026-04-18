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

    this->setProperty("theme", "white");
    ui->initBtn->setProperty("connected", false);

    connect(ui->initBtn, &QPushButton::clicked, this, &MainWindow::onInitializeClicked);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    
    // Set default values
    ui->portCombo->addItem("ttyACM0");
    ui->portCombo->addItem("ttyACM1");
    ui->baudCombo->addItem("9600");
    ui->baudCombo->addItem("115200");
    
    // Mock terminal initial data
    ui->textBrowser->append(QString("<span style='color: %1;'>[14:22:01.03] TX >> 0x41 0x54 (AT+RST)</span>").arg(getLogColor("TX")));
    ui->textBrowser->append(QString("<span style='color: %1;'>[14:22:01.45] RX << 0x4F 0x4B (OK)</span>").arg(getLogColor("RX")));
    ui->textBrowser->append(QString("<span style='color: #0f0; background-color: #004400;'>[14:22:02.35] INTERRUPT >> PIN_01 HIGH</span>"));
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
        /* --- Base Layout --- */
        QWidget {
            font-family: 'Monospace';
            font-size: 12px;
        }

        /* --- Theme: WHITE (Neo-Brutalist) --- */
        QMainWindow[theme="white"], QWidget[theme="white"] {
            background-color: #ffffff;
            color: #000000;
        }

        /* --- Theme: DARK (Neo-Brutalist) --- */
        QMainWindow[theme="dark"], QWidget[theme="dark"] {
            background-color: #1a1a1a;
            color: #ffffff;
        }
        QMainWindow[theme="dark"] QFrame#userInfoBox, 
        QMainWindow[theme="dark"] QFrame#tempPanel, 
        QMainWindow[theme="dark"] QFrame#actPanel, 
        QMainWindow[theme="dark"] QFrame#gpioPanel, 
        QMainWindow[theme="dark"] QFrame#panelBox {
            background-color: #2a2a2a;
            border: 4px solid white;
            border-bottom: 8px solid white;
            border-right: 8px solid white;
        }
        QMainWindow[theme="dark"] QPushButton {
            background-color: #333333;
            color: white;
            border: 2px solid white;
        }
        QMainWindow[theme="dark"] QLabel#topTitle { color: #5588ff; }
        QMainWindow[theme="dark"] QLabel#userTitle { color: #5588ff; }

        /* --- Components --- */
        QFrame#topBar { border-bottom: 4px solid black; }
        QMainWindow[theme="dark"] QFrame#topBar { border-bottom: 4px solid white; }
        QFrame#sideBar { border-right: 4px solid black; }
        QMainWindow[theme="dark"] QFrame#sideBar { border-right: 4px solid white; }

        /* Panels & Boxes */
        QFrame#userInfoBox, QFrame#tempPanel, QFrame#actPanel, QFrame#gpioPanel, QFrame#panelBox {
            border: 4px solid black;
            background-color: #ffffff;
            border-bottom: 8px solid black;
            border-right: 8px solid black;
        }

        /* --- Specialized Colors --- */
        QLabel#topTitle { font-size: 20px; font-weight: bold; color: #0022cc; }
        QLabel#userTitle { font-weight: bold; color: blue; }
        QLabel#dashTitle { font-size: 36px; font-weight: 900; }
        QLabel#tempValue { font-size: 64px; font-weight: 900; }
        
        /* Buttons */
        QPushButton {
            border: 2px solid black;
            padding: 10px;
            font-weight: bold;
            background-color: #ffffff;
        }
        QPushButton:pressed {
            margin-top: 2px; margin-left: 2px;
            border-bottom: 2px solid black; border-right: 2px solid black;
        }

        /* --- Dynamic States --- */
        
        /* Connect Button State */
        QPushButton#initBtn[connected="false"] { background-color: #aaaaaa; color: white; }
        QPushButton#initBtn[connected="true"]  { background-color: #00aa00; color: white; }

        /* LED / Toggle States */
        QLabel[active="true"] { color: blue; }
        QLabel#tgl1_val, QLabel#tgl2_val, QLabel#tgl3_val {
            background-color: #eeeeee;
            border: 2px solid black;
            padding: 5px;
        }
        QLabel#tgl1_val[active="true"], QLabel#tgl2_val[active="true"], QLabel#tgl3_val[active="true"] {
            background-color: blue;
            color: white;
        }

        /* Terminal */
        QFrame#termPanel { background-color: #222222; border: 4px solid black; }
        QTextBrowser#textBrowser { background-color: #222222; color: #cccccc; border: none; }
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
        ui->textBrowser->append(QString("<span style='color: %1;'>[SYS] Port Closed</span>").arg(getLogColor("SYS")));
        ui->dashStatus->setText("STATUS: <span style='color:gray;'>DISCONNECTED</span>");
        return;
    }

    QString portName = ui->portCombo->currentText();
    if(portName.contains("ttyACM0")) {
        serial->setPortName("ttyACM0");
    } else {
        serial->setPortName(portName);
        ui->textBrowser->append(QString("<span style='color: %1;'>[SYS] Attempting to open %2</span>").arg(getLogColor("SYS"), portName));
    }
    
    int baud = ui->baudCombo->currentText().toInt();
    serial->setBaudRate(baud);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if(serial->open(QIODevice::ReadWrite)){
        ui->textBrowser->append(QString("<span style='color: %1;'>[SYS] Connection : Port Opened at %2 bps</span>").arg(getLogColor("SYS"), QString::number(baud)));
        ui->initBtn->setText("Connected");
        ui->initBtn->setProperty("connected", true);
        ui->initBtn->style()->unpolish(ui->initBtn);
        ui->initBtn->style()->polish(ui->initBtn);
        ui->dashStatus->setText("STATUS: <span style='color:blue;'>CONNECTED_ACTIVE</span> / UPTIME: 04:12:33");
    }
    else{
        ui->textBrowser->append(QString("<span style='color: %1;'>[ERR] Failed to open port!</span>").arg(getLogColor("ERR")));
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
            ui->textBrowser->append(QString("<span style='color: %1;'>[RX] $%2#</span>").arg(getLogColor("RX"), packetStr));
            parseProtocol(packetStr);
        }
    }
}

QString MainWindow::getLogColor(const QString &type) {
    bool isDark = (this->property("theme").toString() == "dark");
    if (type == "SYS") return isDark ? "#ffff00" : "#aa6600";
    if (type == "ERR") return "#ff0000";
    if (type == "TX")  return isDark ? "#44aaff" : "#0055ff";
    if (type == "RX")  return isDark ? "#00ffff" : "#008888";
    return isDark ? "#ffffff" : "#000000";
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
            
            ui->tgl1_val->setText(isOn ? "[ ON ]" : "[ OFF ]");
            ui->tgl1_val->setProperty("active", isOn);
            ui->tgl1_sym->setProperty("active", isOn);

            // 스타일 갱신 (Dynamic Property 반영)
            ui->tgl1_val->style()->unpolish(ui->tgl1_val);
            ui->tgl1_val->style()->polish(ui->tgl1_val);
            ui->tgl1_sym->style()->unpolish(ui->tgl1_sym);
            ui->tgl1_sym->style()->polish(ui->tgl1_sym);
        }
    }
}
