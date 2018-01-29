#ifndef SerialPort_H
#define SerialPort_H

#include <QSerialPort>
#include <QJsonArray>

class SerialPort : public QSerialPort
{
    Q_OBJECT
public:
    Q_INVOKABLE SerialPort(QObject *parent = nullptr);

    Q_INVOKABLE bool isOpen() { return QSerialPort::isOpen() ;}
    Q_INVOKABLE bool isReadable() { return QSerialPort::isReadable() ;}
    Q_INVOKABLE bool isWritable() { return QSerialPort::isWritable() ;}
    Q_INVOKABLE bool open(OpenMode mode) { return QSerialPort::open(mode) ;}
    Q_INVOKABLE void close() { QSerialPort::close() ;}

    Q_INVOKABLE qint64 bytesAvailable() { return QSerialPort::bytesAvailable() ;}
    Q_INVOKABLE qint64 bytesToWrite() { return QSerialPort::bytesToWrite() ;}

    Q_INVOKABLE bool setBaudRate(qint32 baudRate, Directions directions = AllDirections) { return QSerialPort::setBaudRate(baudRate, directions) ;}
    Q_INVOKABLE qint32 baudRate(Directions directions = AllDirections) const { return QSerialPort::baudRate(directions) ;}

    Q_INVOKABLE QByteArray readAll() { return QSerialPort::readAll() ;}
    Q_INVOKABLE qint64 write(const QByteArray &data) { return QSerialPort::write(data) ;}

    Q_INVOKABLE QJsonArray portsList() ;
};

#endif // SerialPort_H
