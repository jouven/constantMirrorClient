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
        , const downloadInfo_s &downloadInfo_par_con
        , const bool deleteThenCopy_par_con, QObject *parent)
    : QSslSocket(parent)
    , downloadInfo_pri(downloadInfo_par_con)
    , deleteThenCopy_pri(deleteThenCopy_par_con)
{
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
            //QOUT_TS("(downloadClient_c::ctor() error) his->deleteLater()" << endl);
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
    //QOUT_TS("(downloadClient_c::successfulConnection_f) client connected" << endl);
#endif

#ifdef DEBUGJOUVEN
    //QOUT_TS("(downloadClient_c::successfulConnection_f) client connected state" << endl);
    //qDebug() << "this->peerAddress()" << this->peerAddress() << endl;
    //qDebug() << "this->peerPort()" << this->peerPort() << endl;
#endif
    //check how is the file locally and send a request with the filename fullpath
    if (not downloadInfo_pri.source_pub.isEmpty())
    {
        QFileInfo destinationFileInfoTmp(downloadInfo_pri.destination_pub);
        if (destinationFileInfoTmp.exists() and destinationFileInfoTmp.isFile())
        {
            if (deleteThenCopy_pri)
            {
                if (QFile::remove(destinationFileInfoTmp.absoluteFilePath()))
                {
                    //remove successful
                }
                else
                {
                    QOUT_TS("Failed to remove local file " << destinationFileInfoTmp.absoluteFilePath() << " before downloading it from server" << endl);
                    this->disconnectFromHost();
                    return;
                }
            }
            else
            {
                if (QFile::rename(destinationFileInfoTmp.absoluteFilePath(), destinationFileInfoTmp.absoluteFilePath() + ".old"))
                {
                    //rename successful
                }
                else
                {
                    //#ifdef DEBUGJOUVEN
                    QOUT_TS("Failed to rename local file " << destinationFileInfoTmp.absoluteFilePath() << " before downloading it from server" << endl);
                    this->disconnectFromHost();
                    return;
                    //#endif
                }
            }
        }
        else
        //doesn't exist locally, try to create its parent path and leave it ready so it can be created
        //and written
        {
            if (destinationFileInfoTmp.isDir())
            {
//#ifdef DEBUGJOUVEN
                QOUT_TS("Download error, destination is folder " << destinationFileInfoTmp.absoluteFilePath() << endl);
//#endif
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
//#ifdef DEBUGJOUVEN
                    QOUT_TS("Download error, failed to create parent Path " << destinationParentTmp.absolutePath() << endl);
                    this->disconnectFromHost();
                    return;
//#endif
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
            //QOUT_TS("(downloadClient_c::successfulConnection_f) sourceFileFullPath_pri " << sourceFileFullPath_pri << endl);
#endif
            byteArrayTmp.append(downloadInfo_pri.source_pub);
            this->write(byteArrayTmp.data(), byteArrayTmp.size());
            //this flush is alright because else the client might wait before sending more stuff when what's required
            //is for the server to receive the sourceFile so it can start sending it to the client
            if (this->flush())
            {
#ifdef DEBUGJOUVEN
                //QOUT_TS("(downloadClient_c::successfulConnection_f) write flush true" << endl);
#endif
            }
            else
            {
#ifdef DEBUGJOUVEN
                //QOUT_TS("(downloadClient_c::successfulConnection_f) write flush false" << endl);
#endif
            }
        }
        else
        {
//#ifdef DEBUGJOUVEN
            QOUT_TS("Download error, failed to open/create " << destinationFileInfoTmp.absoluteFilePath() << endl);
            this->disconnectFromHost();
//#endif
        }
    }
    else
    {
//#ifdef DEBUGJOUVEN
        QOUT_TS("Download error, source/remote file name is empty" << endl);
        this->disconnectFromHost();
//#endif
    }
}

void downloadClient_c::newRead_f()
{
#ifdef DEBUGJOUVEN
    //QOUT_TS("(downloadClient_c::newRead_f) socket newRead" << endl);
#endif
    if (firstRead_pri)
    {
        firstRead_pri = false;
        QByteArray arrayOfOneTmp(this->read(1));
        char firstChar(arrayOfOneTmp.at(0));
        if (firstChar != '0')
        {
            bool isknownErrorTmp(false);
            if (firstChar == '1')
            {
                isknownErrorTmp = true;
                QOUT_TS("Download file not found in the server " << downloadInfo_pri.source_pub << endl);
            }
            if (firstChar == '2')
            {
                isknownErrorTmp = true;
                QOUT_TS("Download file not found in the server (it existed previously) " << downloadInfo_pri.source_pub << endl);
            }
            if (firstChar == '3')
            {
                isknownErrorTmp = true;
                QOUT_TS("File reading error in the server " << downloadInfo_pri.source_pub << endl);
            }

            if (not isknownErrorTmp)
            {
                QOUT_TS("Download error, unknown error, first byte value " << firstChar << endl);
            }
#ifdef DEBUGJOUVEN
            //QOUT_TS("(downloadClient_c::newRead_f()) firstChar " << arrayOfOneTmp << endl);
#endif
            this->disconnectFromHost();
        }
        else
        {
            //success
        }
    }

    auto writeSize(file_pri.write(this->readAll()));
#ifdef DEBUGJOUVEN
    //QOUT_TS("(downloadClient_c::newRead_f()) this->readAll() writeSize " << writeSize << endl);
#endif

}

void downloadClient_c::finishFile_f()
{
#ifdef DEBUGJOUVEN
    //QOUT_TS("(downloadClient_c::finishFile_f) " << endl);
#endif
    if (file_pri.size() == downloadInfo_pri.size_pub)
    {
        if (not deleteThenCopy_pri)
        {
            //remove the old file
            QFileInfo destinationFileInfoTmp(downloadInfo_pri.destination_pub + ".old");
            if (destinationFileInfoTmp.exists())
            {
                if (QFile::remove(downloadInfo_pri.destination_pub + ".old"))
                {
                    //remove success
                }
                else
                {
                    //#ifdef DEBUGJOUVEN
                    QOUT_TS("Download error, failed to remove " << downloadInfo_pri.destination_pub + ".old after download" << endl);
                    //#endif
                }
            }
            else
            {
                //should this be an error? going to delete the file but it is not there...
            }
        }
        else
        {
            //it was deleted before, so nothing to do here
        }
    }
    else
    {
        QOUT_TS("Download error, file size (file list request) " << downloadInfo_pri.size_pub << " downloaded file size " << file_pri.size() << endl);
    }
}

