#include "server.h"
#include <QDebug>

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
        qInfo().noquote() << tcpAddr(tcp);
        tcpinit(tcp);
        Link *link = new Link;
        link->tcp = tcp;
        link->peer = new Link;
        link->peer->peer = link;
        link->peer->tcp = new QTcpSocket(this);
        tcpinit(link->peer->tcp);
        link->peer->tcp->connectToHost("172.16.97.129", 2723);
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
        if(send > 0)
        {
            link ->dataTx.remove(0,send);
            qDebug().noquote() << tcpAddr(link->tcp) << "tx" << link->dataTx.size();
        }
    }
}

void Server::tcpConnected()
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    qWarning().noquote() << tcpAddr(tcp);
}

void Server::tcpDisconnected()
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    qWarning().noquote() << tcpAddr(tcp);

}
void Server::tcpError(QAbstractSocket::SocketError error)
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    qDebug().noquote() << tcpAddr(tcp) << error;
}

void Server::tcpStateChanged(QAbstractSocket::SocketState state)
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    qDebug().noquote() << tcpAddr(tcp) << state;

    if(QAbstractSocket::UnconnectedState != state)
    {
        return;
    }
    LinkHash::iterator itr = mLinkHash.find(tcp);
    Link *link = itr.value();

    if(Link *peer = link->peer)
    {
        LinkHash::iterator itr_next = mLinkHash.find(link ->peer->tcp);
        mLinkHash.erase(itr_next);
        peer->peer = NULL;
        peer -> tcp ->abort();
    }
    mLinkHash.erase(itr);
    delete link;
}

void Server::tcpBytesWeitten(qint64 bytes)
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    qDebug().noquote() << tcpAddr(tcp) << "bytes" << bytes;
    LinkHash::iterator itr = mLinkHash.find(tcp);
    Link *link = itr.value();
    tcpsend(link);

}

void Server::tcpReadyRead()
{
    QTcpSocket *tcp = qobject_cast<QTcpSocket*>(sender());
    LinkHash::iterator itr = mLinkHash.find(tcp);
    Link* link = itr.value();
    link->peer->dataRx.append(link->tcp->readAll());
    qDebug().noquote() << tcpAddr(link->peer->tcp) << "tx" << link->peer->dataTx.size();
    tcpsend(link->peer);
}
