#include "server.h"
#include <QDebug>

Server::Server(QObject *parent) : QObject(parent)
{
}

Server::~Server()
{
}

//1
//TCP连接建立，监听，显示地址和端口
bool Server::init()
{
    mTcpServer = new QTcpServer(this);

    connect(mTcpServer, &QTcpServer::newConnection, this, &Server::tcpNewConnection);

    if (!mTcpServer->listen(QHostAddress::AnyIPv4, 1723)) {
        qDebug().noquote() << mTcpServer->serverError() << mTcpServer->errorString();
        return false;
    }
    qInfo().noquote() << "tcpListen： " << mTcpServer->serverAddress().toString() + ":" + QString::number(mTcpServer->serverPort());

    return true;
}

//3333
//从nextpending中获取到一个TCP，然后调用此函数获取TCP的端口号和地址
QString Server::tcpAddr(QTcpSocket *tcp)
{
    return tcp->peerAddress().toString() + ":" + QString::number(tcp->peerPort());
}





//55
void Server::tcpBytesWritten(qint64 bytes)
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender()); //返回signal的发送者
    qDebug().noquote() << tcpAddr(tcp) << "bytes" << bytes;
    LinkHash::iterator itr = mLinkHash.find(tcp); //根据tcp 找到link包
    Link *link = itr.value();
    tcpSend(link);//发送该包，具体函数在底下
}
//666
void Server::tcpConnected()
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    qWarning().noquote() << tcpAddr(tcp);
    LinkHash::iterator itr = mLinkHash.find(tcp);
    Link *link = itr.value();
    tcpSend(link);
}
//777777
void Server::tcpDisconnected()
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    qWarning().noquote() << tcpAddr(tcp);
}
//999999
void Server::tcpError(QAbstractSocket::SocketError error)
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    qDebug().noquote() << tcpAddr(tcp) << error;
}



//44444444444444444
//初始化TCP连接函数
void Server::tcpInit(QTcpSocket *tcp)
{
    connect(tcp, &QTcpSocket::bytesWritten, this, &Server::tcpBytesWritten);
    connect(tcp, &QTcpSocket::connected, this, &Server::tcpConnected);
    connect(tcp, &QTcpSocket::disconnected, this, &Server::tcpDisconnected);//断开连接后，显示断开的连接地址
    connect(tcp, &QTcpSocket::disconnected, tcp, &QTcpSocket::deleteLater);//断开连接
    connect(tcp, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(tcpError(QAbstractSocket::SocketError))); // duplicated error() method fail compiling
    connect(tcp, &QTcpSocket::readyRead, this, &Server::tcpReadyRead);
    connect(tcp, &QTcpSocket::stateChanged, this, &Server::tcpStateChanged);
}


//2222222222222222222
void Server::tcpNewConnection()
{//取下一个在等待连接的请求
    while (mTcpServer->hasPendingConnections()) {
        QTcpSocket *tcp = mTcpServer->nextPendingConnection();
        qInfo().noquote() << tcpAddr(tcp); //获取该tcp请求的地址和port
        tcpInit(tcp); //初始化该TCP连接
        Link *link = new Link;
        link->tcp = tcp;
        link->peer = new Link;
        link->peer->tcp = new QTcpSocket(this); //下游服务器请求建立连接 中继点连接到远程服务器
        tcpInit(link->peer->tcp);
        link->peer->tcp->connectToHost("172.16.97.129", 2723);
        link->peer->peer = link; //回环链表
        mLinkHash.insert(tcp, link);
        mLinkHash.insert(link->peer->tcp, link->peer);
    }
}

//10 10 10 10
//中继点可以接受了信号函数
void Server::tcpReadyRead()
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    LinkHash::iterator itr = mLinkHash.find(tcp);
    Link *link = itr.value();
    Link *peer = link->peer;//link 中的下一个tcp
    peer->dataTx.append(tcp->readAll());
    qDebug().noquote() << tcpAddr(peer->tcp) << "tx" << peer->dataTx.size();
    tcpSend(peer);
}

//8888
//发送tcp
void Server::tcpSend(Link *link)
{
    if (QAbstractSocket::ConnectedState == link->tcp->state()) {
        qint64 sent = link->tcp->write(link->dataTx);//将发送数据buffer写入TCP中
        if (0 < sent) {
            link->dataTx.remove(0, sent);//清空buffer
            qDebug().noquote() << tcpAddr(link->tcp) << "tx" << link->dataTx.size();
        }
    }
}


//11 11 11
//删除tcp连接
void Server::tcpStateChanged(QAbstractSocket::SocketState state)
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    qDebug().noquote() << tcpAddr(tcp) << state;
    if (QAbstractSocket::UnconnectedState!=state) {
        return;
    }
    LinkHash::iterator itr = mLinkHash.find(tcp);
    Link *link = itr.value();
    mLinkHash.erase(itr);
    if (Link *peer = link->peer) {
        peer->peer = NULL;
        peer->tcp->abort();
    }
    delete link;
}
