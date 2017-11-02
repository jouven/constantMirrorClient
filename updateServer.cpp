#include "updateServer.hpp"
#include "updateServerThread.hpp"
#include "mirrorConfig.hpp"

#include "essentialQtso/essentialQt.hpp"

#include <QHostAddress>
#include <QHostInfo>
#include <QTcpSocket>


updateServer_c::updateServer_c(
        const QHostAddress &address_par_con
        , const quint16 port_par_con
        , QObject *parent_par)
    : QTcpServer(parent_par)
{
    if (this->listen(address_par_con, port_par_con))
    {
#ifdef DEBUGJOUVEN
        QOUT_TS("(updateServer_c::updateServer_c) listen successfull" << endl);
        QOUT_TS("(updateServer_c::updateServer_c) address_par_con.toString() " << address_par_con.toString() << endl);
        QOUT_TS("(updateServer_c::updateServer_c) port_par_con " << port_par_con << endl);
#endif
    }
    else
    {
        QOUT_TS("Failed to setup update server, address " << address_par_con.toString() << " port " << port_par_con << endl);
    }
}


void updateServer_c::incomingConnection(qintptr socketDescriptor)
{
    updateServerThread_c* updateServerThreadTmp = new updateServerThread_c(
                socketDescriptor
                , this
    );

    connect(updateServerThreadTmp, &QThread::finished, [updateServerThreadTmp]
    {
        //port (should be the file request server one) sent by the "source"

        QByteArray portByteArrayTmp(updateServerThreadTmp->destinationByteArray_f());
        quint16 portReadFromUpdateRequestTmp(portByteArrayTmp.toInt());
        QHostAddress peerAddressTmp(updateServerThreadTmp->peerAddress_f());
#ifdef DEBUGJOUVEN
        //QOUT_TS("(updateServerThreadTmp, &QThread::finished) peerAddressTmp " << peerAddressTmp.toString()<< endl);
        //QOUT_TS("(updateServerThreadTmp, &QThread::finished) peerAddressTmp " << peerAddressTmp.toIPv4Address()<< endl);
        //QOUT_TS("(updateServerThreadTmp, &QThread::finished) portReadFromUpdateRequestTmp " << portReadFromUpdateRequestTmp << endl);
#endif
        mirrorConfig_ext.setRemoteHasUpdated_f(peerAddressTmp, portReadFromUpdateRequestTmp);
    });
    connect(updateServerThreadTmp, &QThread::finished, updateServerThreadTmp, &QThread::deleteLater);
    updateServerThreadTmp->start();
}

