#ifndef SERIAL_DEVICE_H
#define SERIAL_DEVICE_H

#include <QObject>
#include <QSerialPort>
#include <QByteArray>

class SerialDevice : public QObject {
    Q_OBJECT
public:
    explicit SerialDevice(QObject *parent = nullptr);
    virtual ~SerialDevice();

    bool open(const QString &portName, int baudRate);
    void close();
    bool isOpen() const;

    qint64 write(const QByteArray &data);
    void flush();

signals:
    void readyRead(const QByteArray &data);
    void connected();
    void disconnected();
    void errorOccurred(const QString &error);

private slots:
    void onReadyRead();

protected:
    QSerialPort *m_serial;
};

#endif // SERIAL_DEVICE_H
