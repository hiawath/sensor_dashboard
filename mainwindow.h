#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTextBrowser>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QFrame>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onInitializeClicked();
    void readData();

private:
    void setupUI();
    void applyStyles();

    // Serial Communication
    QSerialPort *serial;

    // UI Elements
    QTextBrowser *terminalDisplay;
    QComboBox *portCombo;
    QComboBox *baudCombo;
    QPushButton *initBtn;
};

#endif // MAINWINDOW_H
