#ifndef DYNAMICCONNECTIONRECEIVER_H
#define DYNAMICCONNECTIONRECEIVER_H

#include <QObject>
#include <QVariant>
#include "qtobjectwrapper.h"


class DynamicConnectionReceiver: public QObject {
    Q_OBJECT
public:
    explicit DynamicConnectionReceiver(QtObjectWrapper * wrapper):
        wrapper(wrapper), QObject() {}
public slots:
    void slot() ;
    void slot1(const QVariant &) ;
    void slot2(QVariant, QVariant) ;
    void slot3(const QVariant &, const QVariant &, const QVariant &) ;
    void slot4(const QVariant &, const QVariant &, const QVariant &, const QVariant &) ;
    void slot5(const QVariant &, const QVariant &, const QVariant &, const QVariant &, const QVariant &) ;
private :
    QtObjectWrapper * wrapper ;
};

#endif // DYNAMICCONNECTIONRECEIVER_H
