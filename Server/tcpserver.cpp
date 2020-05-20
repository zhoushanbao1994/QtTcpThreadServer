#include "tcpserver.h"
#include "threadhandle.h"
#include <QMetaType>

TcpServer::TcpServer(QObject *parent,int numConnections) :
    QTcpServer(parent)
{
    // 注册自定义类型
    qRegisterMetaType<QHostAddress>("QHostAddress");
    qRegisterMetaType<qintptr>("qintptr");


    // 最大连接数默认10000
    // 创建Socket容器
    m_tcpClient = new  QHash<int, TcpSocket *>;
    // 设置tcp最大连接数
    setMaxPendingConnections(numConnections);
}

TcpServer::~TcpServer()
{
    //emit this->sentDisConnectSignal(-1);
    delete m_tcpClient;
}

// 重写设置最大连接数函数
void TcpServer::setMaxPendingConnections(int numConnections)
{
    //调用Qtcpsocket函数，设置最大连接数，主要是使maxPendingConnections()依然有效
    this->QTcpServer::setMaxPendingConnections(numConnections);
    m_maxConnections = numConnections;
}

void TcpServer::incomingConnection(qintptr socketDescriptor) //多线程必须在此函数里捕获新连接
{
    // 继承重写此函数后，QTcpServer默认的判断最大连接数失效，自己实现
    if (m_tcpClient->size() > maxPendingConnections()) {        // 当前连接数 > 最大连接数
        QTcpSocket tcp;
        tcp.setSocketDescriptor(socketDescriptor);
        tcp.disconnectFromHost();                   // 关闭连接
        return;
    }

    auto th = ThreadHandle::getClass().getThread();
    auto tcpTemp = new TcpSocket(socketDescriptor);     // 创建TcpSocket连接
    QHostAddress ip =  tcpTemp->peerAddress();          // 获取IP
    qint16 port = tcpTemp->peerPort();                  // 获取端口

    // NOTE:断开连接的处理，从列表移除，并释放断开的Tcpsocket，此槽必须实现，线程管理计数也是考的他
    connect(tcpTemp, &TcpSocket::sockDisConnect, this, &TcpServer::sockDisConnectSlot);
    // 接收数据
    connect(tcpTemp, &TcpSocket::readData_signal, this, &TcpServer::readDataSlot);
    // 主动断开连接
    //connect(this, &TcpServer::sentDisConnectSignal, tcpTemp,&TcpSocket::disConTcp);

    // 发送数据
    connect(this, &TcpServer::sendDataSignal, tcpTemp,&TcpSocket::sentDataSlot);


    tcpTemp->moveToThread(th);//把tcp类移动到新的线程，从线程管理类中获取
    m_tcpClient->insert(socketDescriptor, tcpTemp);//插入到连接信息中

    // 发送信号：有新设备连接（Socket描述符，IP，端口）
    emit connectClientSignal(socketDescriptor, ip, port);

}

// 断开连接的用户信息
void TcpServer::sockDisConnectSlot(const qintptr socketDescriptor, const QHostAddress & ip, quint16 prot, QThread * th)
{
    m_tcpClient->remove(socketDescriptor);//连接管理中移除断开连接的socket
    ThreadHandle::getClass().removeThread(th); //告诉线程管理类那个线程里的连接断开了

    // 发送信号：设备断开连接（Socket描述符，IP，端口）
    emit sockDisConnectSignal(socketDescriptor, ip, prot);
}

// 接收数据
void TcpServer::readDataSlot(const qintptr socketDescriptor, const QHostAddress &ip, const quint16 point,const QByteArray &data)
{
    // 发送获得用户发过来的数据
    emit readDataSignal(socketDescriptor, ip, point, data);
}

//void TcpServer::clearSlot()
//{
//    emit this->sentDisConnectSignal(-1);
//    ThreadHandle::getClass().clear();
//    m_tcpClient->clear();
//}
