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
    void tcpConnected();                    //中继点与上游节点建立连接成功，输出到控制台
    void tcpDisconnected();                 //tcp连接断开，输出到控制台
    void tcpNewConnection();                //listen到请求，下游节点向中继点建立连接时，中继点同时向上游服务器请求TCP连接
    void tcpStateChanged(QAbstractSocket::SocketState state);   //TCP状态改变，如果是UnconnectedState，则删除link节点，释放对称的另一组tcp连接
    void tcpError(QAbstractSocket::SocketError error);          //tcp错误
    void tcpReadyRead();                    //中继点读取一方的TCP数据，并将该数据转送到另一方
    void tcpBytesWeitten(qint64 bytes);     //数据写入TCP，并发送，将address 和 包大小输出到控制台
private:
    QTcpServer *myTcpServer;
    struct Link {
        QByteArray dataRx;
        QByteArray dataTx;
        Link *peer;                         //中继点与下游服务器的link
        QTcpSocket *tcp;
    };
    typedef QHash<QTcpSocket*,Link*> LinkHash;
    QString tcpAddr (QTcpSocket *tcp);      //返回该tcp的address
    void tcpinit(QTcpSocket *tcp);          //初始化该TCP连接，将各 singal 与 slot相连
    void tcpsend(Link *link);               //将dataTX写入TCP中，等待发送
    LinkHash mLinkHash;


};

#endif // SERVER_H
