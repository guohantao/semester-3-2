#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class Server : public QObject
{
    Q_OBJECT
public:
    Server(QObject *parent = 0);
    ~Server();
    bool init(); //监听连接；

private slots:
    void tcpConnected();
    void tcpDisconnected();
    void tcpNewConnection();
    void tcpStateChanged(QAbstractSocket::SocketState state);
    void tcpError(QAbstractSocket::SocketError error);
    void tcpReadyRead();
    void tcpBytesWeitten(qint64 bytes);
private:
    QTcpServer *myTcpServer;
    struct Link {
        QByteArray dataRx;
        QByteArray dataTx;
        Link *peer;
        QTcpSocket *tcp;
    };
    typedef QHash<QTcpSocket*,Link*> LinkHash;
    QString tcpAddr (QTcpSocket *tcp);
    void tcpinit(QTcpSocket *tcp);
    void tcpsend(Link *link);
    LinkHash mLinkHash;


};

#endif // SERVER_H
