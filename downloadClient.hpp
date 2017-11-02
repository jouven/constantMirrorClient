#ifndef CMC_DOWNLOADCLIENT_HPP
#define CMC_DOWNLOADCLIENT_HPP

#include "mirrorConfig.hpp"

#include <QTcpSocket>
#include <QFile>
#include <QHostAddress>
#include <QSslSocket>

class downloadClient_c : public QSslSocket
{
    Q_OBJECT

   downloadInfo_s downloadInfo_pri;

    QFile file_pri;
    bool firstRead_pri = true;
    bool deleteThenCopy_pri = false;

public:
    explicit downloadClient_c(
            const QHostAddress& address_par_con
            , const quint16 port_par_con
            , const downloadInfo_s& downloadInfo_par_con
            , const bool deleteThenCopy_par_con
            , QObject *parent = nullptr);
private Q_SLOTS:
    void successfulConnection_f();
    void newRead_f();
    void finishFile_f();
};

#endif // CMC_DOWNLOADCLIENT_HPP
