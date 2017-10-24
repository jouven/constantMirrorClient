#ifndef CMC_DOWNLOADCLIENT_HPP
#define CMC_DOWNLOADCLIENT_HPP

#include <QTcpSocket>
#include <QFile>
#include <QHostAddress>
#include <QSslSocket>

class downloadClient_c : public QSslSocket
{
    Q_OBJECT

    QString sourceFileFullPath_pri;
    //to deal with files where the remote file has a name but locally is saved with another one
    QString destinationFileFullPath_pri;

    QFile file_pri;
    bool firstRead_pri = true;

public:
    explicit downloadClient_c(
            const QHostAddress& address_par_con
            , const quint16 port_par_con
            , const QString& sourceFileFullPath_par_con
            //if it's empty it uses sourceFileFullPath_par_con
            , const QString& destinationFileFullPath_par_con = QString()
            , QObject *parent = nullptr);
private Q_SLOTS:
    void successfulConnection_f();
    void newRead_f();
    void finishFile_f();
};

#endif // CMC_DOWNLOADCLIENT_HPP
