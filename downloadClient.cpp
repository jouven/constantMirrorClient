#include "downloadClient.hpp"

//#include "fileHashQtso/fileHashQt.hpp"
#include "essentialQtso/essentialQt.hpp"

#include <QHostAddress>
#include <QDir>
#include <QFileInfo>
#include <QSslConfiguration>
#include <QFile>
#include <QSslKey>

downloadClient_c::downloadClient_c(const QHostAddress& address_par_con
        , const quint16 port_par_con
        , const QString& sourceFileFullPath_par_con
        , const QString &destinationFileFullPath_par_con
        , QObject *parent)
    : QSslSocket(parent)
    , sourceFileFullPath_pri(sourceFileFullPath_par_con)
    , destinationFileFullPath_pri(destinationFileFullPath_par_con)
{
    //this->setSslConfiguration(QSslConfiguration::defaultConfiguration());
    //this->setPeerVerifyMode(QSslSocket::VerifyNone);
    //this->setProtocol(QSsl::TlsV1_2);

    if (destinationFileFullPath_pri.isEmpty())
    {
        destinationFileFullPath_pri = sourceFileFullPath_pri;
    }
    connect(this, &QSslSocket::encrypted, this, &downloadClient_c::successfulConnection_f);
    connect(this, &QTcpSocket::readyRead, this, &downloadClient_c::newRead_f);
    connect(this, &QTcpSocket::disconnected, this, &downloadClient_c::finishFile_f);
    connect(this,  static_cast<void(QAbstractSocket::*)(QAbstractSocket::SocketError)>(&QAbstractSocket::error), [=](QAbstractSocket::SocketError socketError)
    {
#ifdef DEBUGJOUVEN
        QOUT_TS("(downloadClient_c::ctor() error) address " << this->peerAddress().toString()
                 << " port " << this->peerPort()
                 << " error: " << this->errorString() << endl);
#endif
        if (this->error() != QAbstractSocket::RemoteHostClosedError)
        {
#ifdef DEBUGJOUVEN
            QOUT_TS("(downloadClient_c::ctor() error) his->deleteLater()" << endl);
#endif
            this->deleteLater();
        }
    });
    connect(this, static_cast<void(QSslSocket::*)(const QList<QSslError> &)>(&QSslSocket::sslErrors), [=](const QList<QSslError> &errors)
    {
#ifdef DEBUGJOUVEN
        for (const QSslError& errorItem_ite_con : errors)
        {
            QOUT_TS("(downloadClient_c::ctor() sslerror) " << errorItem_ite_con.errorString() << endl);
        }
#endif
        //this->deleteLater();
    });
    this->connectToHostEncrypted(address_par_con.toString(), port_par_con);
}

void downloadClient_c::successfulConnection_f()
{
#ifdef DEBUGJOUVEN
    QOUT_TS("(downloadClient_c::successfulConnection_f) client connected" << endl);
#endif

#ifdef DEBUGJOUVEN
    //QOUT_TS("(downloadClient_c::successfulConnection_f) client connected state" << endl);
    //qDebug() << "this->peerAddress()" << this->peerAddress() << endl;
    //qDebug() << "this->peerPort()" << this->peerPort() << endl;
#endif
    //check how is the file locally and send a request with the filename fullpath
    if (not sourceFileFullPath_pri.isEmpty())
    {
        QFileInfo destinationFileInfoTmp(destinationFileFullPath_pri);
        if (destinationFileInfoTmp.exists() and destinationFileInfoTmp.isFile())
        {
            //TODO futur posar es tema de renombrar primer o despues
            if (QFile::rename(destinationFileInfoTmp.absoluteFilePath(), destinationFileInfoTmp.absoluteFilePath() + ".old"))
            {
                //rename successful
            }
            else
            {
#ifdef DEBUGJOUVEN
                QOUT_TS("(downloadClient_c::successfulConnection_f) failed to rename " << destinationFileInfoTmp.absoluteFilePath() << endl);
#endif
            }
        }
        else
        //doesn't exist locally, try to create its parent path and leave it ready so it can be created
        //and written
        {
            if (destinationFileInfoTmp.isDir())
            {
#ifdef DEBUGJOUVEN
                QOUT_TS("(downloadClient_c::successfulConnection_f) destination is folder " << destinationFileInfoTmp.absoluteFilePath() << endl);
#endif
                this->disconnectFromHost();
                return;
            }
            QDir destinationParentTmp(destinationFileInfoTmp.absoluteDir());
            //create parent folders if necessary
            if (not destinationParentTmp.exists())
            {
                if (destinationParentTmp.mkpath("."))
                {
                    //created successful
                }
                else
                {
#ifdef DEBUGJOUVEN
                    QOUT_TS("(downloadClient_c::successfulConnection_f) failed to create parent Path " << destinationParentTmp.absolutePath() << endl);
#endif
                }
            }
            else
            {
                //already exists
            }
        }
        //set the local file ready to be written, it will be created on open
        //and request the file, using source name, remotely
        file_pri.setFileName(destinationFileInfoTmp.absoluteFilePath());
        if (file_pri.open(QIODevice::WriteOnly))
        {
            QByteArray byteArrayTmp;
#ifdef DEBUGJOUVEN
QOUT_TS("(downloadClient_c::successfulConnection_f) sourceFileFullPath_pri " << sourceFileFullPath_pri << endl);
#endif
            byteArrayTmp.append(sourceFileFullPath_pri);
            this->write(byteArrayTmp.data(), byteArrayTmp.size());
            //this flush is alright because else the client might wait to send more stuff when what's required
            //is the server to have the sourceFile so it can start sending it
            if (this->flush())
            {
#ifdef DEBUGJOUVEN
                QOUT_TS("(downloadClient_c::successfulConnection_f) write flush true" << endl);
#endif
            }
            else
            {
#ifdef DEBUGJOUVEN
                QOUT_TS("(downloadClient_c::successfulConnection_f) write flush false" << endl);
#endif
            }
        }
        else
        {
#ifdef DEBUGJOUVEN
            QOUT_TS("(downloadClient_c::successfulConnection_f) failed to open/create " << file_pri.fileName() << endl);
#endif
        }
    }
    else
    {
#ifdef DEBUGJOUVEN
        QOUT_TS("(downloadClient_c::successfulConnection_f) source file name is empty" << endl);
#endif
    }
}

void downloadClient_c::newRead_f()
{
#ifdef DEBUGJOUVEN
    QOUT_TS("(downloadClient_c::newRead_f) socket newRead" << endl);
#endif
    if (firstRead_pri)
    {
        firstRead_pri = false;
        QByteArray arrayOfOneTmp(this->read(1));
        if (arrayOfOneTmp.at(0) != '0')
        {
#ifdef DEBUGJOUVEN
            QOUT_TS("(downloadClient_c::newRead_f()) firstChar " << arrayOfOneTmp << endl);
#endif
            this->disconnectFromHost();
        }
    }

    auto writeSize(file_pri.write(this->readAll()));
#ifdef DEBUGJOUVEN
    QOUT_TS("(downloadClient_c::newRead_f()) this->readAll() writeSize " << writeSize << endl);
#endif

}

void downloadClient_c::finishFile_f()
{
#ifdef DEBUGJOUVEN
    QOUT_TS("(downloadClient_c::finishFile_f) " << endl);
#endif
    //close the downloaded file
    file_pri.close();
    //TODO futur posar es tema de renombrar primer o despues
    //remove the old file
    QFileInfo destinationFileInfoTmp(destinationFileFullPath_pri + ".old");
    if (destinationFileInfoTmp.exists())
    {
        if (QFile::remove(destinationFileFullPath_pri + ".old"))
        {

        }
        else
        {
#ifdef DEBUGJOUVEN
            QOUT_TS("(downloadClient_c::finishFile_f) failed to remove " << destinationFileFullPath_pri + ".old" << endl);
#endif
        }
    }
}

