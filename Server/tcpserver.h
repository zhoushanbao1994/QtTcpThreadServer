#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QTcpServer>
#include <QHash>
#include "tcpsocket.h"


//继承QTCPSERVER以实现多线程TCPscoket的服务器。
//如果socket的信息处理直接处理的话，很多新建的信号和槽是用不到的
class TcpServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit TcpServer(QObject *parent = 0,int numConnections = 10000);
    ~TcpServer();

    void setMaxPendingConnections(int numConnections);//重写设置最大连接数函数

    // 对外的信号
signals:
    // 新用户连接时，发送新用户连接信息
    void connectClientSignal(const qintptr, const QHostAddress & ,const quint16 );
    // 发送获得用户发过来的数据
    void readDataSignal(const qintptr,const QHostAddress &, quint16, const QByteArray &);
    // 断开连接的用户信息
    void sockDisConnectSignal(const qintptr,QHostAddress ,quint16);

    // 对TcpSocket的信号
signals:
    // 向scoket发送消息
    void sendDataSignal(const int, const QByteArray &);
    // 断开特定连接，并释放资源，-1为断开所有。
    //void sentDisConnectSignal(int i);

//public slots:
//    void clearSlot(); //断开所有连接，线程计数器请0

    // 对TcpSocket的槽函数
protected slots:
    // 断开连接的用户信息
    void sockDisConnectSlot(const qintptr socketDescriptor,const QHostAddress & ip, quint16 prot, QThread *th);
    // 接收数据
    void readDataSlot(const qintptr socketDescriptor, const QHostAddress &, const quint16,const QByteArray &);

protected:
    void incomingConnection(qintptr socketDescriptor);//覆盖已获取多线程
private:
    QHash<int,TcpSocket *> * m_tcpClient; //管理连接的map，TcpSocket容器
    int m_maxConnections;                 // 最大连接数

};

#endif // TCPSERVER_H
