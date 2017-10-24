#include "updateServerSocket.hpp"

#include "essentialQtso/essentialQt.hpp"

updateServerSocket_c::updateServerSocket_c(const qintptr socketDescriptor_par_con
        , QObject *parent_par) :
    QSslSocket(parent_par)
{

    connect(this, &QTcpSocket::readyRead, this, &updateServerSocket_c::readyRead_f);
    connect(this, &QTcpSocket::disconnected, this, &updateServerSocket_c::disconnected_f);
    connect(this, static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
    [=](QAbstractSocket::SocketError socketError)
    {
#ifdef DEBUGJOUVEN
        QOUT_TS("(updateServerSocket_c::ctor()) socket error address " << this->peerAddress().toString()
                 << " port " << this->peerPort()
                 << " error: " << this->errorString() << endl);
#endif
        if (this->error() != QAbstractSocket::RemoteHostClosedError)
        {
#ifdef DEBUGJOUVEN
            QOUT_TS("(updateServerSocket_c::ctor() error) his->deleteLater()" << endl);
#endif
            this->deleteLater();
        }
    });
    connect(this, static_cast<void(QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors), [=](const QList<QSslError> &errors)
    {
#ifdef DEBUGJOUVEN
        for (const QSslError& errorItem_ite_con : errors)
        {
            QOUT_TS("(updateServerSocket_c::ctor() sslerror) " << errorItem_ite_con.errorString() << endl);
        }
#endif
        //this->deleteLater();
    });
    if(not this->setSocketDescriptor(socketDescriptor_par_con))
    {
        Q_EMIT error(this->error());
    }

    if (this->state() == QTcpSocket::SocketState::ConnectedState)
    {
#ifdef DEBUGJOUVEN
        QOUT_TS("(updateServerSocket_c::ctor()) this->state() == QTcpSocket::SocketState::ConnectedState" << endl);
#endif
        peerAddress_pri = this->peerAddress();
    }
    else
    {
#ifdef DEBUGJOUVEN
        QOUT_TS("(updateServerSocket_c::ctor()) NOT peerAddress_pri = this->peerAddress()" << endl);
#endif
    }
    this->startServerEncryption();
}

void updateServerSocket_c::readyRead_f()
{
    destinationByteArray_pri.append(this->readAll());
#ifdef DEBUGJOUVEN
        QOUT_TS("(updateServerSocket_c::readyRead_f()) destinationByteArray_pri " << destinationByteArray_pri << endl);
#endif
}

void updateServerSocket_c::disconnected_f()
{
#ifdef DEBUGJOUVEN
    QOUT_TS("(updateServerSocket_c::disconnected_f()) "  << this->socketDescriptor() << " Disconnected" << endl);
#endif
    this->deleteLater();
}

QByteArray updateServerSocket_c::destinationByteArray_f() const
{
    return destinationByteArray_pri;
}

QHostAddress updateServerSocket_c::peerAddress_f() const
{
    return peerAddress_pri;
}
