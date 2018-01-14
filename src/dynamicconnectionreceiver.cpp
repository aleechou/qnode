#include "dynamicconnectionreceiver.h"
#include "common.h"

void DynamicConnectionReceiver::slot() {
    this->wrapper->onSignalReceived(this) ;
}

void DynamicConnectionReceiver::slot1(const QVariant & arg1) {
}

void DynamicConnectionReceiver::slot2(QVariant arg1, QVariant arg2) {
    qDebug() << arg1.type() ;
    qDebug() << arg1 << arg2 ;
}

void DynamicConnectionReceiver::slot3(const QVariant & arg1, const QVariant & arg2, const QVariant & arg3) {
}

void DynamicConnectionReceiver::slot4(const QVariant & arg1, const QVariant & arg2, const QVariant & arg3, const QVariant & arg4) {
}

void DynamicConnectionReceiver::slot5(const QVariant & arg1, const QVariant & arg2, const QVariant & arg3, const QVariant & arg4, const QVariant & arg5) {
}
