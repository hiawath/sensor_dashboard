#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTextBrowser>
#include <QList>
#include <QFrame>
#include <QWidget>
#include <QPainter>
#include <QPainterPath>
#include <QPen>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

typedef enum {
    ID_SYS_HEARTBEAT    = 0,
    ID_SYS_UPTIME       = 1,
    ID_SYS_TEMP         = 2,
    ID_SYS_VREF         = 3,
    ID_ENV_TEMP         = 10,
    ID_ENV_HUMI         = 11,
    ID_ENV_PRESS        = 12,
    ID_ENV_LIGHT        = 13,
    ID_IN_BUTTON_1      = 30,
    ID_IN_BUTTON_2      = 31,
    ID_IN_SW_DIP        = 32,
    ID_IN_ENC_POS       = 33,
    ID_OUT_LED_STATE    = 50,
    ID_OUT_MOTOR_SPEED  = 51,
    ID_OUT_RELAY        = 52,
    ID_IMU_ACCEL_X      = 70,
    ID_IMU_ACCEL_Y      = 71,
    ID_IMU_ACCEL_Z      = 72,
    ID_IMU_GYRO_X       = 73,
    ID_ALARM_CRITICAL   = 100,
    ID_ALARM_WARN       = 101
} SensorID;

typedef enum {
    TYPE_UINT8   = 0,
    TYPE_INT32   = 1,
    TYPE_FLOAT   = 2,
    TYPE_BOOL    = 3,
    TYPE_STRING  = 4
} DataType;

class ChartWidget : public QWidget {
public:
    ChartWidget(int maxPoints = 600, double minY = 10.0, double maxY = 50.0, QColor color = QColor(0, 34, 204), QWidget *parent = nullptr) 
        : QWidget(parent), m_maxPoints(maxPoints), m_minY(minY), m_maxY(maxY), m_lineColor(color) {
        setMinimumHeight(100);
    }

    void addData(double value) {
        m_data.append(value);
        if (m_data.size() > m_maxPoints) m_data.removeFirst();
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        int w = width();
        int h = height();

        // 배경 그리드
        painter.setPen(QPen(QColor(200, 200, 200, 100), 1, Qt::DashLine));
        for (int i = 0; i <= 4; ++i) {
            int y = h - (i * h / 4);
            painter.drawLine(0, y, w, y);
            double val = m_minY + (m_maxY - m_minY) * (i / 4.0);
            painter.drawText(5, y - 5, QString::number(val, 'f', 1) + "C");
        }

        if (m_data.isEmpty()) return;

        // 라인 그리기
        QPainterPath path;
        double displayCount = qMax(10.0, (double)m_data.size());
        if (m_maxPoints < displayCount) displayCount = m_maxPoints;
        
        double xStep = (double)w / (displayCount - 1);

        for (int i = 0; i < m_data.size(); ++i) {
            double val = qBound(m_minY, m_data[i], m_maxY);
            double x = i * xStep;
            double y = h - ((val - m_minY) / (m_maxY - m_minY) * h);

            if (i == 0) path.moveTo(x, y);
            else path.lineTo(x, y);
        }

        painter.setPen(QPen(m_lineColor, 2));
        painter.drawPath(path);
        
        QPainterPath fillPath = path;
        fillPath.lineTo((m_data.size()-1) * xStep, h);
        fillPath.lineTo(0, h);
        fillPath.closeSubpath();
        
        QLinearGradient gradient(0, 0, 0, h);
        gradient.setColorAt(0, QColor(m_lineColor.red(), m_lineColor.green(), m_lineColor.blue(), 80));
        gradient.setColorAt(1, QColor(m_lineColor.red(), m_lineColor.green(), m_lineColor.blue(), 0));
        painter.fillPath(fillPath, gradient);
    }

private:
    QList<double> m_data;
    int m_maxPoints;
    double m_minY, m_maxY;
    QColor m_lineColor;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject *obj, QEvent *event) override;

private slots:
    void onInitializeClicked();
    void readData();
    void onExitRequested();

    void on_menuLedBlink_clicked();
    void on_menuMonitorStart_clicked();
    void on_menuMonitorStop_clicked();

private:
    void applyStyles();
    void setupDynamicUI();
    void parseProtocol(const QString &data);
    void monitor_Off();
    QString getLogColor(const QString &type);
    void updateChart(double newValue);

    Ui::MainWindow *ui;
    QSerialPort *serial;
    QByteArray m_serialBuffer;
    ChartWidget *m_chartWidget;
    ChartWidget *m_liveChart;
};
#endif // MAINWINDOW_H
