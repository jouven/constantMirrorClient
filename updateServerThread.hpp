#ifndef CMC_UPDATESERVERTHREAD_HPP
#define CMC_UPDATESERVERTHREAD_HPP

#include "threadedFunctionQtso/threadedFunctionQt.hpp"

#include <QByteArray>
#include <QHostAddress>

class updateServerThread_c : public threadedFunction_c
{
    Q_OBJECT

    QHostAddress peerAddress_pri;
    qintptr socketDescriptor_pri = 0;
    QByteArray destinationByteArray_pri;

    void run() override;
public:
    explicit updateServerThread_c(
            const qintptr socketDescriptor_par_con
            , QObject * parent_par = nullptr
    );

    QHostAddress peerAddress_f() const;
    QByteArray destinationByteArray_f() const;
};

#endif // CMC_UPDATESERVERTHREAD_HPP
