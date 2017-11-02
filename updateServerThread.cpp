#include "updateServerThread.hpp"

#include "updateServerSocket.hpp"

#include "essentialQtso/essentialQt.hpp"

updateServerThread_c::updateServerThread_c(const qintptr socketDescriptor_par_con
        , QObject *parent_par) :
    threadedFunction_c(parent_par)
    , socketDescriptor_pri(socketDescriptor_par_con)
{
}

void updateServerThread_c::run()
{
#ifdef DEBUGJOUVEN
    //QOUT_TS("updateServerThread_c::run()" << endl);
#endif
    updateServerSocket_c* updateServerSocketTmp = new updateServerSocket_c(socketDescriptor_pri);
    connect(updateServerSocketTmp, &QTcpSocket::disconnected, [this, updateServerSocketTmp]()
    {
        if (not updateServerSocketTmp->destinationByteArray_f().isEmpty())
        {
            destinationByteArray_pri = updateServerSocketTmp->destinationByteArray_f();
        }
        else
        {
//#ifdef DEBUGJOUVEN
            QOUT_TS("Update server, client didn't send any data, which server updated can't be determined" << endl);
//#endif
        }
        //this one should be impossible to else but who knows
        if (not updateServerSocketTmp->peerAddress_f().isNull())
        {
            peerAddress_pri = updateServerSocketTmp->peerAddress_f();
        }
        else
        {
//#ifdef DEBUGJOUVEN
            QOUT_TS("Update server, client address is null, which server updated can't be determined" << endl);
//#endif
        }
    });

    connect(updateServerSocketTmp, &QTcpSocket::destroyed, this, &QThread::quit);

    exec();
}

QByteArray updateServerThread_c::destinationByteArray_f() const
{
    return destinationByteArray_pri;
}
QHostAddress updateServerThread_c::peerAddress_f() const
{
    return peerAddress_pri;
}
