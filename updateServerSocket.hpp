#ifndef CMC_UPDATESERVERSOCKET_HPP
#define CMC_UPDATESERVERSOCKET_HPP

#include <QTcpSocket>
#include <QByteArray>
#include <QHostAddress>
#include <QSslSocket>

class updateServerSocket_c : public QSslSocket
{
    Q_OBJECT

    QHostAddress peerAddress_pri;
    QByteArray destinationByteArray_pri;
public:
    explicit updateServerSocket_c(
            const qintptr socketDescriptor_par_con
            , QObject* parent_par = nullptr
    );

    QHostAddress peerAddress_f() const;
    QByteArray destinationByteArray_f() const;

private Q_SLOTS:
    void readyRead_f();
    void disconnected_f();
};

#endif // CMC_UPDATESERVERSOCKET_HPP
