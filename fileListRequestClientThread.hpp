#ifndef CMC_FILELISTREQUESTTHREAD_HPP
#define CMC_FILELISTREQUESTTHREAD_HPP

#include "threadedFunctionQtso/threadedFunctionQt.hpp"

#include <QByteArray>
#include <QHostAddress>

class fileListRequestClientThread_c : public threadedFunction_c
{
    Q_OBJECT

    QHostAddress address_pri;
    quint16 port_pri = 0;

    QByteArray* destinationByteArrayRef_pri;

    void run() override;
public:
    explicit fileListRequestClientThread_c(
            const QHostAddress& address_par_con
            , const quint16 port_par_con
            , QByteArray* destinationByteArray_par = nullptr
            , QObject * parent_par = nullptr
    );
};

#endif // CMC_FILELISTREQUESTTHREAD_HPP
