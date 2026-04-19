#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  m_viewModel = new DashboardViewModel(this);

  // Minimize, Maximize 버튼 제거
  setWindowFlags(windowFlags() & ~Qt::WindowMinimizeButtonHint &
                 ~Qt::WindowMaximizeButtonHint &
                 ~Qt::WindowContextHelpButtonHint);

  setupDynamicUI();
  applyStyles();

  this->setProperty("theme", "white");
  ui->initBtn->setProperty("connected", false);

  // 아이콘 클릭 감지를 위해 이벤트 필터 설치
  ui->iconSettings->installEventFilter(this);
  ui->iconPower->installEventFilter(this);

  // ViewModel Connections
  connect(m_viewModel, &DashboardViewModel::temperatureChanged, this, &MainWindow::updateTemperature);
  connect(m_viewModel, &DashboardViewModel::connectionChanged, this, &MainWindow::updateConnectionState);
  connect(m_viewModel, &DashboardViewModel::ledActiveChanged, this, &MainWindow::updateLedState);
  connect(m_viewModel, &DashboardViewModel::logMessage, this, &MainWindow::appendLog);
  connect(m_viewModel, &DashboardViewModel::rawDataReceived, [this](const QString &packet) {
      // Any additional raw data handling if needed
  });

  connect(ui->initBtn, &QPushButton::clicked, this, &MainWindow::onInitializeClicked);

  // Set default values
  ui->portCombo->addItem("ttyACM0");
  ui->portCombo->addItem("ttyACM1");
  ui->baudCombo->addItem("9600");
  ui->baudCombo->addItem("115200");

  // Initial log
  appendLog("SYS", "Application Started");
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::setupDynamicUI() {
  // Generate the temperature chart dynamically
  ui->chartLayout->setSpacing(0);
  ui->chartLayout->setContentsMargins(0, 0, 0, 0);
  
  m_chartWidget = new ChartWidget(600, 10.0, 50.0, QColor(0, 34, 204), this);
  ui->chartLayout->addWidget(m_chartWidget);

  // Setup GPIO buttons dynamically
  auto setupGpio = [this](QFrame *gBox, QString name, bool active) {
    gBox->setFixedSize(90, 110);
    if (active)
      gBox->setObjectName("panelBoxActive");
    else
      gBox->setObjectName("panelBox");
    QVBoxLayout *gL = new QVBoxLayout(gBox);
    gL->setAlignment(Qt::AlignCenter);
    QLabel *icon = new QLabel(active ? "◉" : "◯", this);
    icon->setObjectName("iconTextBig");
    if (active)
      icon->setStyleSheet("color: blue;");
    QLabel *lbl = new QLabel(name, this);
    QLabel *stat = new QLabel(active ? "PRESSED" : "RELEASED", this);
    if (active)
      stat->setStyleSheet("background-color: blue; color: white; font-weight: "
                          "bold; padding: 2px;");
    else
      stat->setStyleSheet("color: gray; font-size: 10px;");
    gL->addWidget(icon, 0, Qt::AlignCenter);
    gL->addWidget(lbl, 0, Qt::AlignCenter);
    gL->addWidget(stat, 0, Qt::AlignCenter);
  };
  // setupGpio(ui->g1, "BTN_01", true);
  // setupGpio(ui->g2, "BTN_02", false);
  // setupGpio(ui->g3, "BTN_03", false);
  // setupGpio(ui->g4, "BTN_04", false);
  
  // Real-time Live Chart (50 points)
  m_liveChart = new ChartWidget(50, 10.0, 50.0, QColor(204, 34, 0), this);
  ui->gpioGrid->addWidget(m_liveChart);
}

void MainWindow::applyStyles() {
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

void MainWindow::onInitializeClicked() {
  if (m_viewModel->isConnected()) {
    m_viewModel->disconnectDevice();
    return;
  }

  QString portName = ui->portCombo->currentText();
  int baud = ui->baudCombo->currentText().toInt();
  
  appendLog("SYS", QString("Attempting to open %1 at %2 bps").arg(portName, QString::number(baud)));
  m_viewModel->connectDevice(portName, baud);
}

void MainWindow::updateTemperature(const QString &temp) {
  ui->tempValue->setText(temp);
  // Extract number for chart
  QString numPart = temp.split(' ').first();
  updateChart(numPart.toDouble());
}

void MainWindow::updateConnectionState(bool connected) {
  ui->initBtn->setText(connected ? "Connected" : "Disconnected");
  ui->initBtn->setProperty("connected", connected);
  ui->initBtn->style()->unpolish(ui->initBtn);
  ui->initBtn->style()->polish(ui->initBtn);
  
  if (connected) {
    ui->dashStatus->setText("STATUS: <span style='color:blue;'>CONNECTED_ACTIVE</span> / UPTIME: 00:00:00");
  } else {
    ui->dashStatus->setText("STATUS: <span style='color:gray;'>DISCONNECTED</span>");
  }
}

void MainWindow::updateLedState(bool active) {
  ui->tgl1_val->setText(active ? "[ ON ]" : "[ OFF ]");
  ui->tgl1_val->setProperty("active", active);
  ui->tgl1_sym->setProperty("active", active);

  ui->tgl1_val->style()->unpolish(ui->tgl1_val);
  ui->tgl1_val->style()->polish(ui->tgl1_val);
  ui->tgl1_sym->style()->unpolish(ui->tgl1_sym);
  ui->tgl1_sym->style()->polish(ui->tgl1_sym);
}

void MainWindow::appendLog(const QString &type, const QString &msg) {
  QString timeStr = QTime::currentTime().toString("hh:mm:ss.zzz");
  ui->textBrowser->append(QString("<span style='color: %1;'>[%2] [%3] %4</span>")
      .arg(getLogColor(type), timeStr, type, msg));
}
// Logic moved to SensorModel and DashboardViewModel

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
  if ((obj == ui->iconSettings || obj == ui->iconPower) &&
      event->type() == QEvent::MouseButtonRelease) {
    onExitRequested();
    return true;
  }
  return QMainWindow::eventFilter(obj, event);
}

void MainWindow::onExitRequested() {
  QMessageBox::StandardButton res = QMessageBox::question(
      this, "프로그램 종료", "프로그램을 종료하시겠습니까?",
      QMessageBox::Yes | QMessageBox::No);
  if (res == QMessageBox::Yes) {
    m_viewModel->setMonitor(false);
    m_viewModel->disconnectDevice();
    this->close();
  }
}

void MainWindow::on_menuLedBlink_clicked() {
  m_viewModel->toggleLed();
}

void MainWindow::on_menuMonitorStart_clicked() {
  m_viewModel->setMonitor(true);
}

void MainWindow::on_menuMonitorStop_clicked() {
  m_viewModel->setMonitor(false);
}

QString MainWindow::getLogColor(const QString &type) {
  bool isDark = (this->property("theme").toString() == "dark");
  if (type == "SYS") return isDark ? "#ffff00" : "#aa6600";
  if (type == "ERR") return "#ff0000";
  if (type == "TX")  return isDark ? "#44aaff" : "#0055ff";
  if (type == "RX")  return isDark ? "#00ffff" : "#008888";
  return isDark ? "#ffffff" : "#000000";
}
void MainWindow::updateChart(double newValue) {
  if (m_chartWidget) m_chartWidget->addData(newValue);
  if (m_liveChart) m_liveChart->addData(newValue);
}
