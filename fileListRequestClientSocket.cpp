#include "fileListRequestClientSocket.hpp"

#include "mirrorConfig.hpp"

#include "essentialQtso/essentialQt.hpp"

fileListRequestClientSocket_c::fileListRequestClientSocket_c(const QHostAddress &address_par_con
        , const quint16 port_par_con
        , QByteArray *destinationByteArrayRef_par
        , QObject *parent_par) :
    QSslSocket(parent_par)
    , destinationByteArrayRef_pri(destinationByteArrayRef_par)

{
    connect(this, &QSslSocket::encrypted, this, &fileListRequestClientSocket_c::connected_f);
    connect(this, &QTcpSocket::readyRead, this, &fileListRequestClientSocket_c::readyRead_f);
    connect(this, &QTcpSocket::disconnected, this, &fileListRequestClientSocket_c::disconnected_f);
    connect(this, static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error),
    [=](QAbstractSocket::SocketError socketError)
    {
#ifdef DEBUGJOUVEN
        QOUT_TS("(fileListRequestClientSocket_c::ctor()) address " << this->peerAddress().toString()
                 << " port " << this->peerPort()
                 << " error: " << this->errorString() << endl);
#endif
        if (this->error() != QAbstractSocket::RemoteHostClosedError)
        {
#ifdef DEBUGJOUVEN
            QOUT_TS("(fileListRequestClientSocket_c::ctor() error) this->deleteLater()" << endl);
#endif
            this->deleteLater();
        }
    });
    connect(this, static_cast<void(QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors), [=](const QList<QSslError> &errors)
    {
#ifdef DEBUGJOUVEN
        for (const QSslError& errorItem_ite_con : errors)
        {
            QOUT_TS("(fileListRequestClientSocket_c::ctor() sslerror) " << errorItem_ite_con.errorString() << endl);
        }
#endif
        //this->deleteLater();
    });
#ifdef DEBUGJOUVEN
    QOUT_TS("(fileListRequestClientSocket_c::fileListRequestClientSocket_c) address_par_con.toString() " << address_par_con.toString() << endl);
    QOUT_TS("(fileListRequestClientSocket_c::fileListRequestClientSocket_c) port_par_con " << port_par_con << endl);
#endif
    this->connectToHostEncrypted(address_par_con.toString(), port_par_con);
}

void fileListRequestClientSocket_c::connected_f()
{
    QByteArray byteArrayTmp;
    byteArrayTmp.append(QString::number(mirrorConfig_ext.updateServerPort_f()));
    this->write(byteArrayTmp.data(), byteArrayTmp.size());
    //valid flush because the server is the one governing the connection
    this->flush();
#ifdef DEBUGJOUVEN
    QOUT_TS("fileListRequestClientSocket_c::connected_f() mirrorConfig_ext.updateServerPort_f() " << mirrorConfig_ext.updateServerPort_f() << endl);
#endif
}

void fileListRequestClientSocket_c::readyRead_f()
{
#ifdef DEBUGJOUVEN
    //QOUT_TS("fileListRequestClientSocket_c::readyRead_f() " << this->bytesAvailable() << endl);
#endif
    destinationByteArrayRef_pri->append(this->readAll());
}

void fileListRequestClientSocket_c::disconnected_f()
{
#ifdef DEBUGJOUVEN
    QOUT_TS("(fileListRequestClientSocket_c::disconnected_f()) " << endl);
#endif
    this->deleteLater();
}

