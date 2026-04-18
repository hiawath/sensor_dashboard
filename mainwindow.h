#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QTextBrowser>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:

    void on_btnRead_clicked();
    void readData();

private:
    Ui::MainWindow *ui;
    QSerialPort *serial;
    QTextBrowser *textBrowser;

};
#endif // MAINWINDOW_H
