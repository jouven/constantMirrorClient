#ifndef CMC_UPDATESERVER_HPP
#define CMC_UPDATESERVER_HPP

#include <QTcpServer>
#include <QHostAddress>

class updateServer_c : public QTcpServer
{
    Q_OBJECT

public:
    explicit updateServer_c(
            const QHostAddress &address_par_con = QHostAddress::Any
            , const quint16 port_par_con = 0
            , QObject *parent_par = nullptr);
protected:
    void incomingConnection(qintptr socketDescriptor) override;
};


#endif // UPDATESERVER_HPP
