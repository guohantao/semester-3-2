#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include<QMutex>

class Server : public QObject
{
    Q_OBJECT
public:
    Server(QObject *parent = 0);
    ~Server();
    bool init(); //监听连接；



private slots:
    void tcpConnected();                    //上游服务器节点 与 中继点建立连接成功，输出到控制台
    void tcpDisconnected();                 //tcp连接断开，输出到控制台
    void tcpNewConnection();                //listen到请求，下游xp节点 向 中继点建立连接时，中继点同时向上游服务器请求TCP连接
    void tcpStateChanged(QAbstractSocket::SocketState state);   //TCP状态改变，如果是UnconnectedState，则删除link节点，释放对称的另一组tcp连接
    void tcpError(QAbstractSocket::SocketError error);          //tcp错误
    void tcpReadyRead();                    //中继点读取一方的TCP数据，并将该数据转送到另一方
    void tcpBytesWeitten(qint64 bytes);     //数据写入TCP，并发送，将address 和 包大小输出到控制台
private:
    QTcpServer *myTcpServer;
    struct Link {
        quint16 callIdRx;
        quint16 callIdTx;
        QByteArray dataRx;
        QByteArray dataTx;
        Link *peer;                         //中继点 与 上游服务器的link
        QTcpSocket *tcp;
    };

    typedef QHash<QTcpSocket*,Link*> LinkHash;
    typedef QHash<QString , Link*> LinkHashGre;
    LinkHash gLinkHash;
    LinkHashGre gLinkHashGre;
    QMutex gLinkHashMutex;
    LinkHash mLinkHash;

    QString tcpAddr (QTcpSocket *tcp);      //返回该tcp的address
    void tcpinit(QTcpSocket *tcp);          //初始化该TCP连接，将各 singal 与 slot相连
    void tcpsend(Link *link);               //将dataTX写入TCP中，等待发送

    void pptpRecv(Link *link);
    void pptpRecv_outCallReply(Link *link);
    void pptpRecv_outCallRequest(Link *link);



};

enum{  //长度
    pptpLen_msgLen                = 2,
    pptpLen_startCtrlConnRequest  = 156,
    pptpLen_startCtrlConnReply    = 156,
    pptpLen_outCallRequest        = 168,
    pptpLen_outCallReply          = 32,
    pptpLen_inCallRequest         = 220,
    pptpLen_inCallReply           = 24,
    pptpLen_inCallConnected       = 28,
    pptpLen_callClearRequest      = 16,
    pptpLen_callDisconnectNotify  = 148,
    pptpLen_wanErrorNotify        = 40,
    pptpLen_setLinkInfo           = 24
};

enum{ //pptp报文包中的位置
    pptpPos_pptpMsgType                 = 2,
    pptpPos_ctrlMsgType                 = 8,
    pptpPos_outCallRequest_callId       = 12,
    pptpPos_outCallRequest_callSn       = 14,
    pptpPos_outCallReply_callId         = 12,
    pptpPos_outCallReply_peerCallId    = 14,
    pptpPos_inCallRequest_callId        = 12,
    pptpPos_inCallRequest_callSn        = 14,
    pptpPos_inCallReply_callId          = 12,
    pptpPos_inCallReply_peerCallId      = 14,
    pptpPos_inCallConnected_peerCallId  = 12,
    pptpPos_callClearRequest_callId     = 12,
    pptpPos_callDisconnectNotify_callId = 12,
    pptpPos_wanErrorNotify_callId       = 12,
    pptpPos_setLinkInfo_peerCallId      = 12
};

enum {
    pptpType_startCtrlConnRequest = 1,
    pptpType_startCtrlConnReply   = 2,
    pptpType_stopCtrlConnRequest  = 3,
    pptpType_stopCtrlConnReply    = 4,
    pptpType_echoRequest          = 5,
    pptpType_echoReply            = 6,
    pptpType_outCallRequest       = 7,
    pptpType_outCallReply         = 8,
    pptpType_inCallRequest        = 9,
    pptpType_inCallReqly          = 10,
    pptpType_inCallConnected      = 11,
    pptpType_callClearRequest     = 12,
    pptpType_callDisconnectNotify = 13,
    pptpType_wanErrorNotify       = 14,
    pptpType_setLinkInfo          = 15


};

#endif // SERVER_H
