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
    bool init();

private slots:
    void tcpBytesWritten(qint64 bytes);
    void tcpConnected();
    void tcpDisconnected();
    void tcpError(QAbstractSocket::SocketError error);
    void tcpNewConnection();
    void tcpReadyRead();
    void tcpStateChanged(QAbstractSocket::SocketState state);

private:
    struct Link {
        QByteArray dataRx;
        QByteArray dataTx;
        Link *peer;
        QTcpSocket *tcp;
    };
    typedef QHash<QTcpSocket*, Link*> LinkHash;
    QString tcpAddr(QTcpSocket *tcp);
    void tcpInit(QTcpSocket *tcp);
    void tcpSend(Link *link);
    LinkHash mLinkHash;
    QTcpServer *mTcpServer;
};

#endif // SERVER_H

