#include "tcpsocket.h"
#include <QtConcurrent/QtConcurrent>
#include <QHostAddress>
#include <QDebug>

TcpSocket::TcpSocket(qintptr socketDescriptor, QObject *parent) : //构造函数在主线程执行，lambda在子线程
    QTcpSocket(parent),
    m_socketID(socketDescriptor)
{
    // 套接字描述符
    this->setSocketDescriptor(socketDescriptor);

    // 关联读数据
    connect(this,&TcpSocket::readyRead,this,&TcpSocket::readData_slot);
    // 关联用户断开
    m_dis = connect(this,&TcpSocket::disconnected,
        [&](){
            qDebug() << QThread::currentThread() << QThread::currentThreadId() << this->peerAddress().toString() << this->peerPort() << "disconnect" ;
            emit sockDisConnect(m_socketID,this->peerAddress(),this->peerPort(),QThread::currentThread());//发送断开连接的用户信息
            this->deleteLater();
        });

    qDebug() << QThread::currentThread()
             //<< QThread::currentThreadId()
             << socketDescriptor
             << this->peerAddress().toString()
             << this->peerPort()
             << "new connect";
}

TcpSocket::~TcpSocket()
{
}


void TcpSocket::sentDataSlot(const qintptr socketDescriptor, const QByteArray &data)
{
    if(socketDescriptor == m_socketID) {
        qDebug() << "-----------------------------------------";
        write(data);
    }
    else {
        qDebug() << "++++++++++++++++++++++++++++++++++++++++++";
    }
}

void TcpSocket::disConTcp(const qintptr socketDescriptor)
{
    if (socketDescriptor == m_socketID)
    {
        this->disconnectFromHost();
    }
    else if (socketDescriptor == -1) //-1为全部断开
    {
        disconnect(m_dis); //先断开连接的信号槽，防止二次析构
        this->disconnectFromHost();
        this->deleteLater();
    }
}

void TcpSocket::readData_slot()
{
    qDebug() << QThread::currentThread() << __FILE__ << __LINE__ << __FUNCTION__;
    QByteArray ba = this->readAll();

    emit readData_signal(m_socketID, this->peerAddress(), this->peerPort(), ba);
}

//QByteArray TcpSocket::handleData(QByteArray data, const QHostAddress &ip, qint16 port)
//{
//    qDebug() << QThread::currentThread() << QThread::currentThreadId() << this->peerAddress().toString() << this->peerPort();
//    QTime time;
//    time.start();
//
//    QElapsedTimer tm;
//    tm.start();
//    while(tm.elapsed() < 100)
//    {}
//    data = "IP:" + ip.toString().toUtf8() + ", Port:" + QByteArray::number(port) + ", Data:" + data + ", Time:" + QTime::currentTime().toString().toUtf8();
//    return data;
//}

