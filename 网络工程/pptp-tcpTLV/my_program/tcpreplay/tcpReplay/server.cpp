#include "server.h"
#include <QDebug>
#include"decode_encode.cpp"

#define LOCK do{qWarning()<< "lock"; gLinkHashMutex.lock();}while(0)
#define UNLOCK do{qWarning()<<"unlock"; gLinkHashMutex.unlock();}while(0)

Server::Server(QObject *parent) : QObject(parent) {}
Server::~Server() {}

QString Server::tcpAddr(QTcpSocket *tcp)
{
    return tcp-> peerAddress().toString() + ":" + QString::number(tcp ->peerPort());
}

//监听连接；
bool Server::init()
{
    myTcpServer = new QTcpServer(this);
    connect(myTcpServer, &QTcpServer::newConnection, this, &Server::tcpNewConnection);
    if( !myTcpServer -> listen(QHostAddress::AnyIPv4,1723))
    {
        qDebug().noquote() << myTcpServer ->serverError() << myTcpServer->errorString();
        return false;
    }
    qInfo().noquote() << "tcpListen : " << myTcpServer->serverAddress().toString() + ":" + QString::number(myTcpServer->serverPort());
    return true;
}

//当下游主机向中继点建立连接时，中继点自动向上游服务器建立连接 并初始化
void Server::tcpNewConnection()
{
    while(myTcpServer -> hasPendingConnections())
    {
        QTcpSocket *tcp = myTcpServer->nextPendingConnection();
        qInfo().noquote() <<"tcpNewConnection: "<< tcpAddr(tcp);
        tcpinit(tcp);
        Link *link = new Link;
        link->tcp = tcp;
        link->peer = new Link;
        link->peer->tcp = new QTcpSocket(this);
        tcpinit(link->peer->tcp);
        link->peer->tcp->connectToHost("192.168.110.128", 1723);
        link->peer->peer = link;
        mLinkHash.insert(tcp,link);
        mLinkHash.insert(link->peer->tcp,link->peer);
    }
}

void Server::tcpinit(QTcpSocket *tcp)
{
    connect(tcp, &QTcpSocket::bytesWritten, this, &Server::tcpBytesWeitten);
    connect(tcp, &QTcpSocket::readyRead, this, &Server::tcpReadyRead);
    connect(tcp, &QTcpSocket::connected, this, &Server::tcpConnected);
    connect(tcp, &QTcpSocket::stateChanged, this, &Server::tcpStateChanged);
    connect(tcp, &QTcpSocket::disconnected, this, &Server::tcpDisconnected);
    connect(tcp, &QTcpSocket::disconnected, tcp, &QTcpSocket::deleteLater);
    connect(tcp, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(tcpError(QAbstractSocket::SocketError))); // duplicated error() method fail compiling
}

void Server::tcpsend(Link *link)
{
    if(QAbstractSocket::ConnectedState == link->tcp->state())
    {
        qint64 send = link->tcp->write(link->dataTx);
        if(0 < send)
        {
            link ->dataTx.remove(0,send);
            qDebug().noquote() <<"tcpSend: "<< tcpAddr(link->tcp) << "remain:" << link->dataTx.size();
        }
    }
}

void Server::tcpConnected()
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    qWarning().noquote()  <<"tcpConnected: " << tcpAddr(tcp);
    LinkHash::iterator itr = mLinkHash.find(tcp);
    Link *link = itr.value();
    tcpsend(link);
}

void Server::tcpDisconnected()
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    qWarning().noquote() <<"tcpDisconnected:　" << tcpAddr(tcp);

}
void Server::tcpError(QAbstractSocket::SocketError error)
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    qDebug().noquote()  <<"tcpError: " << tcpAddr(tcp) << error;
}

void Server::tcpStateChanged(QAbstractSocket::SocketState state)
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    qDebug().noquote() <<"tcpStateChanged: " << tcpAddr(tcp) << state;

    if(QAbstractSocket::UnconnectedState != state)
    {
        return;
    }
    LinkHash::iterator itr = mLinkHash.find(tcp);
    Link *link = itr.value();
    mLinkHash.erase(itr);
    if(Link *peer = link->peer)
    {
        if(! peer-> tcp)
        {
            LinkHash::iterator itr_next = mLinkHash.find(peer->tcp);
            mLinkHash.erase(itr_next);
        }
        peer->peer = NULL;
        peer -> tcp ->abort();
    }

    delete link;
}

void Server::tcpBytesWeitten(qint64 bytes)
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    qDebug().noquote() <<" -BytesWritten: "<< tcpAddr(tcp) << "bytes" << bytes<<endl;
    LinkHash::iterator itr = mLinkHash.find(tcp);
    Link *link = itr.value();
    tcpsend(link);

}
//--------------------------------------------------------------------------------------------------
void Server::tcpReadyRead()
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    LOCK;
    LinkHash::iterator itr = mLinkHash.find(tcp);
    Link* link = itr.value();
    link->dataRx.append(tcp->readAll()); //中继点处理，将tcp数据放入DataRx
    qDebug().noquote() <<"tcpReadyRead: nextpeerAddr" << tcpAddr(link->peer->tcp) << "peerDatatx" << link->peer->dataTx.size();
    pptpRecv(link);
    UNLOCK;
}


#define PPTP_CALL(type) case pptpType_##type: pptpRecv_##type(link); break

//将中继点收到的包判别类型 ，并转存到下一link的dataTx中
void Server::pptpRecv(Link *link)
{
    Link *peer = link->peer;
    bool peerSend = false; //判断一个包是否接收完毕
    while(pptpLen_msgLen < link->dataRx.size())
    {
        quint16 msglen = rawDecodeUit16(link->dataRx.constData(),0);
        qDebug() << " -msgLen :" << msglen;
        if(link->dataRx.size() < msglen) //一个包没接收完
            break;

        //获取PPTP包中的control类型
        quint16 ctrlMsgType = rawDecodeUit16(link->dataRx.constData(),pptpPos_ctrlMsgType);
        switch(ctrlMsgType)
        {
            PPTP_CALL(outCallRequest);
            PPTP_CALL(outCallReply);
//              后续可能要用的
//            PPTP_CALL(startCtrlConnRequest);
//            PPTP_CALL(startCtrlConnReply);
//            PPTP_CALL(stopCtrlConnrequest);
//            PPTP_CALL(stopCtrlConnReply);
//            PPTP_CALL(echoRequest);
//            PPTP_CALL(echoReply);
//            PPTP_CALL(inCallRequest);
//            PPTP_CALL(inCallReply);
//            PPTP_CALL(inCallConnected);
//            PPTP_CALL(callClearRequest);
//            PPTP_CALL(callDisconnectNotify);
//            PPTP_CALL(wanErrorNotify);
//            PPTP_CALL(setLinkInfo);
        }
        peerSend = true;
        peer -> dataTx.append(link->dataRx.left(msglen));   //将接收的RX放入对称link的DataTx中，等待发送
        link -> dataRx.remove(0,msglen);
    }
    if(peerSend)
    {
        qDebug() << " -pptpRecv: nextpeer：" << tcpAddr(peer ->tcp) << " tx " <<peer->dataTx.size();
        tcpsend(peer);
    }
}

void Server::pptpRecv_outCallRequest(Link *link)
{
    quint16 callId = rawDecodeUit16(link->dataRx.constData() , pptpPos_outCallRequest_callId);
    quint16 callSn = rawDecodeUit16(link->dataRx.constData(), pptpPos_outCallRequest_callSn);
    qDebug() << " -outCallRequest:    callID :" << callId << "   callSn:" << callSn;
    link->callIdRx = callId;    //装发到pptp服务器时，储存客户端CALLID
    Link *peer = link->peer;
    peer->callIdTx = callId;
    //将来对于多台客户机可能会出现相同callID 中继点重命名callID后在这里写入数据包
    rawEncodeUnit16(link->dataRx.data(), pptpPos_outCallRequest_callId, peer->callIdTx); //将设置的CALLID写入数据报文
}

void Server::pptpRecv_outCallReply(Link *link)
{
    quint16 callId = rawDecodeUit16(link->dataRx.constData() , pptpPos_outCallReply_callId);
    quint16 peerCallId = rawDecodeUit16(link->dataRx.constData(), pptpPos_outCallReply_peerCallId);
    qDebug() << " -outCallReply:  callID :" << callId << "   peerCallId:" << peerCallId;
    link->callIdRx = callId;

    if(peerCallId != link->callIdTx)
        qCritical() << " -errPeerCallId:" << peerCallId << " callIdTx:" << link->callIdTx;

    Link *peer = link->peer;
    peer->callIdTx = callId;
    rawEncodeUnit16(link->dataRx.data() , pptpPos_outCallReply_peerCallId, peer->callIdRx);
    //这里的CALLID PEERCALLID 都是来自服务器发来的包里的
    //将服务器IP地址 与“中继点”规定的CALLID对应 来寻找link
    //发GRE包时 服务器 所用的CALLID是中继点规定的
    QString linkGre = link->tcp->peerAddress().toString() + ":" + QString::number(peerCallId);
    gLinkHashGre.insert(linkGre,link);
    //将客户端IP地址 与 “服务器”的 CALLID 对应
    //发GRE包时，客户端 所用的callID是服务器规定的
    QString peerGre = peer->tcp->peerAddress().toString() + ":" + QString::number(callId);
    gLinkHashGre.insert(peerGre,peer);

}
