#include "SerialPort.h"
#include <QSerialPortInfo>
#include <QJsonObject>


QJsonArray SerialPort::portsList(){

    const auto serialPortInfos = QSerialPortInfo::availablePorts();

    const QString blankString = "N/A";
    QString description;
    QString manufacturer;
    QString serialNumber;

    QJsonArray list ;

    for (const QSerialPortInfo &serialPortInfo : serialPortInfos) {

        description = serialPortInfo.description();
        manufacturer = serialPortInfo.manufacturer();
        serialNumber = serialPortInfo.serialNumber();

        QJsonObject portInfo ;
        portInfo.insert("Port", serialPortInfo.portName()) ;
        portInfo.insert("Location", serialPortInfo.systemLocation()) ;
        portInfo.insert("Description", !description.isEmpty() ? description : blankString) ;
        portInfo.insert("Manufacturer", !manufacturer.isEmpty() ? manufacturer : blankString) ;
        portInfo.insert("SerialNumber", !serialNumber.isEmpty() ? serialNumber : blankString);
        portInfo.insert("VendorIdentifier", (serialPortInfo.hasVendorIdentifier()
                                                        ? QByteArray::number(serialPortInfo.vendorIdentifier(), 16)
                                                        : blankString)) ;
        portInfo.insert("ProductIdentifier", (serialPortInfo.hasProductIdentifier()
                                              ? QByteArray::number(serialPortInfo.productIdentifier(), 16)
                                              : blankString)) ;
        portInfo.insert("Busy", serialPortInfo.isBusy()) ;

        list.append(portInfo);
    }

    return list ;
}
