#include "serial_device.h"

SerialDevice::SerialDevice(QObject *parent) : QObject(parent) {
    m_serial = new QSerialPort(this);
    connect(m_serial, &QSerialPort::readyRead, this, &SerialDevice::onReadyRead);
}

SerialDevice::~SerialDevice() {
    if (m_serial->isOpen()) {
        m_serial->close();
    }
}

bool SerialDevice::open(const QString &portName, int baudRate) {
    if (m_serial->isOpen()) {
        m_serial->close();
    }

    m_serial->setPortName(portName);
    m_serial->setBaudRate(baudRate);
    m_serial->setDataBits(QSerialPort::Data8);
    m_serial->setParity(QSerialPort::NoParity);
    m_serial->setStopBits(QSerialPort::OneStop);
    m_serial->setFlowControl(QSerialPort::NoFlowControl);

    if (m_serial->open(QIODevice::ReadWrite)) {
        emit connected();
        return true;
    } else {
        emit errorOccurred(m_serial->errorString());
        return false;
    }
}

void SerialDevice::close() {
    if (m_serial->isOpen()) {
        m_serial->close();
        emit disconnected();
    }
}

bool SerialDevice::isOpen() const {
    return m_serial->isOpen();
}

qint64 SerialDevice::write(const QByteArray &data) {
    return m_serial->write(data);
}

void SerialDevice::flush() {
    m_serial->flush();
}

void SerialDevice::onReadyRead() {
    emit readyRead(m_serial->readAll());
}
