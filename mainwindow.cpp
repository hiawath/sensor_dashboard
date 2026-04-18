#include "mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    serial = new QSerialPort(this);
    setupUI();
    applyStyles();

    connect(initBtn, &QPushButton::clicked, this, &MainWindow::onInitializeClicked);
    connect(serial, &QSerialPort::readyRead, this, &MainWindow::readData);
    
    // Set default values
    portCombo->addItem("COM3 - SERIAL_DEV");
    portCombo->addItem("ttyACM0");
    baudCombo->addItem("9600");
    baudCombo->addItem("115200");
    
    // Mock terminal initial data
    terminalDisplay->append("<span style='color: #44aaff;'>[14:22:01.03] TX >> 0x41 0x54 (AT+RST)</span>");
    terminalDisplay->append("<span style='color: #44aaff;'>[14:22:01.45] RX << 0x4F 0x4B (OK)</span>");
    terminalDisplay->append("<span style='color: #0f0; background-color: #004400;'>[14:22:02.35] INTERRUPT >> PIN_01 HIGH</span>");
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    // Central Widget
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // --- TOP BAR ---
    QFrame *topBar = new QFrame(this);
    topBar->setObjectName("topBar");
    topBar->setFixedHeight(60);
    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    
    QLabel *titleLabel = new QLabel("SERIAL_CORE_V1", this);
    titleLabel->setObjectName("topTitle");
    topLayout->addWidget(titleLabel);
    
    QLabel *navSystem = new QLabel("SYSTEM", this);
    navSystem->setObjectName("navLinkActive");
    QLabel *navTerminal = new QLabel("TERMINAL", this);
    navTerminal->setObjectName("navLink");
    QLabel *navLogs = new QLabel("LOGS", this);
    navLogs->setObjectName("navLink");
    
    topLayout->addSpacing(20);
    topLayout->addWidget(navSystem);
    topLayout->addWidget(navTerminal);
    topLayout->addWidget(navLogs);
    topLayout->addStretch();
    
    QLabel *settingsIcon = new QLabel("⚙", this);
    settingsIcon->setObjectName("iconText");
    QLabel *powerIcon = new QLabel("⏻", this);
    powerIcon->setObjectName("iconText");
    
    topLayout->addWidget(settingsIcon);
    topLayout->addWidget(powerIcon);
    
    mainLayout->addWidget(topBar);

    // --- MAIN BODY (Sidebar + Dashboard) ---
    QWidget *bodyWidget = new QWidget(this);
    QHBoxLayout *bodyLayout = new QHBoxLayout(bodyWidget);
    bodyLayout->setContentsMargins(0, 0, 0, 0);
    bodyLayout->setSpacing(0);
    
    // -- SIDEBAR --
    QFrame *sideBar = new QFrame(this);
    sideBar->setObjectName("sideBar");
    sideBar->setFixedWidth(250);
    QVBoxLayout *sideLayout = new QVBoxLayout(sideBar);
    
    QFrame *userInfoBox = new QFrame(this);
    userInfoBox->setObjectName("panelBox");
    QHBoxLayout *userLayout = new QHBoxLayout(userInfoBox);
    QLabel *userIcon = new QLabel("👤", this);
    userIcon->setObjectName("iconTextBig");
    QVBoxLayout *userTextLayout = new QVBoxLayout();
    QLabel *userTitle = new QLabel("ARCHITECT_01", this);
    userTitle->setObjectName("userTitle");
    QLabel *userSub = new QLabel("LEVEL 4 ACCESS", this);
    userSub->setObjectName("userSub");
    userTextLayout->addWidget(userTitle);
    userTextLayout->addWidget(userSub);
    userLayout->addWidget(userIcon);
    userLayout->addLayout(userTextLayout);
    sideLayout->addWidget(userInfoBox);
    
    sideLayout->addSpacing(20);
    QLabel *comLabel = new QLabel("COM PORT", this);
    comLabel->setObjectName("smallLabel");
    sideLayout->addWidget(comLabel);
    portCombo = new QComboBox(this);
    sideLayout->addWidget(portCombo);
    
    sideLayout->addSpacing(10);
    QLabel *baudLabel = new QLabel("BAUD RATE", this);
    baudLabel->setObjectName("smallLabel");
    sideLayout->addWidget(baudLabel);
    baudCombo = new QComboBox(this);
    sideLayout->addWidget(baudCombo);
    
    sideLayout->addSpacing(10);
    initBtn = new QPushButton("INITIALIZE", this);
    initBtn->setObjectName("primaryBtn");
    sideLayout->addWidget(initBtn);
    
    sideLayout->addSpacing(20);
    
    // Sidebar Menu
    QStringList menuItems = {"Dashboard", "Hardware", "Modules", "Connectivity", "Diagnostics"};
    for (int i = 0; i < menuItems.size(); ++i) {
        QPushButton *menuBtn = new QPushButton(menuItems[i], this);
        if (i == 0) {
            menuBtn->setObjectName("menuBtnActive");
        } else {
            menuBtn->setObjectName("menuBtn");
        }
        sideLayout->addWidget(menuBtn);
    }
    
    sideLayout->addStretch();
    QPushButton *newModuleBtn = new QPushButton("NEW_MODULE", this);
    newModuleBtn->setObjectName("secondaryBtn");
    sideLayout->addWidget(newModuleBtn);
    
    bodyLayout->addWidget(sideBar);
    
    // -- DASHBOARD AREA --
    QWidget *dashArea = new QWidget(this);
    QVBoxLayout *dashLayout = new QVBoxLayout(dashArea);
    dashLayout->setContentsMargins(20, 20, 20, 20);
    dashLayout->setSpacing(20);
    
    // Dashboard Header
    QHBoxLayout *headerLayout = new QHBoxLayout();
    QVBoxLayout *headerTitleLayout = new QVBoxLayout();
    QLabel *dashTitle = new QLabel("HARDWARE_MONITOR", this);
    dashTitle->setObjectName("dashTitle");
    QLabel *dashStatus = new QLabel("STATUS: <span style='color:blue;'>CONNECTED_ACTIVE</span> / UPTIME: 04:12:33", this);
    dashStatus->setObjectName("dashStatus");
    headerTitleLayout->addWidget(dashTitle);
    headerTitleLayout->addWidget(dashStatus);
    
    headerLayout->addLayout(headerTitleLayout);
    headerLayout->addStretch();
    
    // Stats
    auto addStat = [this, headerLayout](QString title, QString val) {
        QVBoxLayout *statL = new QVBoxLayout();
        QLabel *tL = new QLabel(title, this);
        tL->setObjectName("smallLabel");
        QLabel *vL = new QLabel(val, this);
        vL->setObjectName("statValue");
        statL->addWidget(tL);
        statL->addWidget(vL);
        headerLayout->addLayout(statL);
        headerLayout->addSpacing(20);
    };
    addStat("CPU_LOAD", "12.4%");
    addStat("MEM_USE", "256MB");
    addStat("REFRESH", "250ms");
    
    dashLayout->addLayout(headerLayout);
    
    // Row 1: Temp & Actuators
    QHBoxLayout *row1 = new QHBoxLayout();
    
    QFrame *tempPanel = new QFrame(this);
    tempPanel->setObjectName("panelBox");
    QVBoxLayout *tempLayout = new QVBoxLayout(tempPanel);
    QLabel *tempTitle = new QLabel("■ CORE_TEMPERATURE", this);
    tempTitle->setObjectName("panelTitle");
    tempLayout->addWidget(tempTitle);
    QLabel *tempValue = new QLabel("42.8 °C", this);
    tempValue->setObjectName("hugeTemp");
    tempLayout->addWidget(tempValue);
    
    // Mockup chart
    QHBoxLayout *chartLayout = new QHBoxLayout();
    chartLayout->setSpacing(2);
    chartLayout->setAlignment(Qt::AlignBottom);
    for (int i = 0; i < 15; ++i) {
        QFrame *bar = new QFrame(this);
        bar->setFixedWidth(15);
        bar->setFixedHeight(20 + i * 5);
        int b = 255 - (i * 10);
        bar->setStyleSheet(QString("background-color: rgb(%1, %1, 255); border: 1px solid black;").arg(b));
        chartLayout->addWidget(bar);
    }
    tempLayout->addLayout(chartLayout);
    row1->addWidget(tempPanel, 2);
    
    QFrame *actPanel = new QFrame(this);
    actPanel->setObjectName("panelBox");
    QVBoxLayout *actLayout = new QVBoxLayout(actPanel);
    QLabel *actTitle = new QLabel("■ LIGHT_ACTUATORS", this);
    actTitle->setObjectName("panelTitle");
    actTitle->setStyleSheet("color: red;");
    actLayout->addWidget(actTitle);
    
    auto addToggle = [this, actLayout](QString name, bool active, QString color) {
        QFrame *tBox = new QFrame(this);
        tBox->setObjectName("panelBox");
        tBox->setFixedHeight(50);
        QHBoxLayout *tL = new QHBoxLayout(tBox);
        QLabel *sym = new QLabel(active ? "●" : "○", this);
        sym->setStyleSheet(QString("color: %1; font-size: 20px;").arg(color));
        QLabel *lbl = new QLabel(name, this);
        tL->addWidget(sym);
        tL->addWidget(lbl);
        tL->addStretch();
        
        QLabel *toggleVis = new QLabel(active ? "[ ON ]" : "[ OFF ]", this);
        if (active) toggleVis->setStyleSheet("background-color: blue; color: white; padding: 5px; border: 2px solid black; font-weight: bold;");
        else toggleVis->setStyleSheet("background-color: #ddd; color: black; padding: 5px; border: 2px solid black; font-weight: bold;");
        tL->addWidget(toggleVis);
        actLayout->addWidget(tBox);
    };
    addToggle("STATUS_RED", true, "blue");
    addToggle("ALARM_YELLOW", false, "gray");
    addToggle("DEBUG_BLUE", true, "blue");
    
    actLayout->addStretch();
    QLabel *disableLink = new QLabel("<u>DISABLE_ALL_OUTPUTS</u>", this);
    disableLink->setAlignment(Qt::AlignCenter);
    disableLink->setStyleSheet("color: blue; font-weight: bold;");
    actLayout->addWidget(disableLink);
    
    row1->addWidget(actPanel, 1);
    dashLayout->addLayout(row1);
    
    // Row 2: GPIOs & Terminal
    QHBoxLayout *row2 = new QHBoxLayout();
    
    QFrame *gpioPanel = new QFrame(this);
    gpioPanel->setObjectName("panelBox");
    QVBoxLayout *gpioLayout = new QVBoxLayout(gpioPanel);
    QLabel *gpioTitle = new QLabel("INPUT_GPIO_STATES", this);
    gpioTitle->setObjectName("panelTitle");
    gpioLayout->addWidget(gpioTitle);
    
    QGridLayout *gpioGrid = new QGridLayout();
    auto addGpio = [this, gpioGrid](int row, int col, QString name, bool active) {
        QFrame *gBox = new QFrame(this);
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
        gpioGrid->addWidget(gBox, row, col);
    };
    addGpio(0, 0, "BTN_01", true);
    addGpio(0, 1, "BTN_02", false);
    addGpio(1, 0, "BTN_03", false);
    addGpio(1, 1, "BTN_04", false);
    gpioLayout->addLayout(gpioGrid);
    
    row2->addWidget(gpioPanel, 1);
    
    QFrame *termPanel = new QFrame(this);
    termPanel->setObjectName("termPanel");
    QVBoxLayout *termLayout = new QVBoxLayout(termPanel);
    
    QHBoxLayout *termHeader = new QHBoxLayout();
    QLabel *termTitle = new QLabel("📄 SERIAL_RAW_DATA_STREAM", this);
    termTitle->setObjectName("termTitle");
    termHeader->addWidget(termTitle);
    termHeader->addStretch();
    QLabel *clearLog = new QLabel("CLEAR_LOG", this);
    clearLog->setObjectName("termAction");
    QLabel *exportCsv = new QLabel("EXPORT_CSV", this);
    exportCsv->setObjectName("termAction");
    termHeader->addWidget(clearLog);
    termHeader->addSpacing(10);
    termHeader->addWidget(exportCsv);
    termLayout->addLayout(termHeader);
    
    terminalDisplay = new QTextBrowser(this);
    terminalDisplay->setObjectName("terminalDisplay");
    termLayout->addWidget(terminalDisplay);
    
    row2->addWidget(termPanel, 2);
    dashLayout->addLayout(row2);
    
    bodyLayout->addWidget(dashArea);
    mainLayout->addWidget(bodyWidget);
    
    resize(1024, 768);
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
        QLabel#navLink {
            font-weight: bold;
            padding: 5px 10px;
        }
        QLabel#navLinkActive {
            font-weight: bold;
            padding: 5px 10px;
            border-bottom: 3px solid blue;
            color: blue;
        }
        QLabel#iconText {
            font-size: 24px;
            padding: 0 10px;
        }
        QLabel#iconTextBig {
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
        QLabel#smallLabel {
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
        QPushButton#primaryBtn {
            background-color: #0022cc;
            color: white;
            border: 2px solid black;
            border-bottom: 4px solid black;
            border-right: 4px solid black;
        }
        QPushButton#primaryBtn:pressed {
            border-bottom: 2px solid black;
            border-right: 2px solid black;
            margin-top: 2px;
            margin-left: 2px;
        }
        QPushButton#secondaryBtn {
            background-color: #fff;
            border: 2px solid black;
            border-bottom: 4px solid black;
            border-right: 4px solid black;
            margin: 10px;
        }
        QPushButton#menuBtn {
            border: none;
            border-bottom: 1px solid #ccc;
            text-align: left;
            padding-left: 20px;
        }
        QPushButton#menuBtnActive {
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
        QLabel#statValue {
            font-size: 18px;
            font-weight: bold;
            color: blue;
        }
        QFrame#panelBox {
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
        QLabel#panelTitle {
            font-size: 14px;
            font-weight: bold;
            margin-bottom: 10px;
        }
        QLabel#hugeTemp {
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
        QLabel#termAction {
            color: white;
            font-size: 10px;
            text-decoration: underline;
        }
        QTextBrowser#terminalDisplay {
            background-color: #222;
            color: #ccc;
            border: none;
            font-family: 'Monospace';
        }
    )";
    this->setStyleSheet(qss);
}

void MainWindow::onInitializeClicked()
{
    QString portName = portCombo->currentText();
    if(portName.contains("ttyACM0")) {
        serial->setPortName("ttyACM0");
    } else {
        terminalDisplay->append("<span style='color: yellow;'>[SYS] Attempting to open " + portName + "</span>");
    }
    
    int baud = baudCombo->currentText().toInt();
    serial->setBaudRate(baud);
    serial->setDataBits(QSerialPort::Data8);
    serial->setParity(QSerialPort::NoParity);
    serial->setStopBits(QSerialPort::OneStop);
    serial->setFlowControl(QSerialPort::NoFlowControl);

    if(serial->open(QIODevice::ReadOnly)){
        terminalDisplay->append("<span style='color: #0f0;'>[SYS] Connection : Port Opened at " + QString::number(baud) + " bps</span>");
    }
    else{
        terminalDisplay->append("<span style='color: red;'>[ERR] Failed to open port!</span>");
    }
}

void MainWindow::readData() {
    QByteArray data = serial->readAll();
    if (!data.isEmpty()) {
        QString text = QString::fromUtf8(data);
        terminalDisplay->append("<span style='color: #0aa;'>[RX] " + text + "</span>");
    }
}
