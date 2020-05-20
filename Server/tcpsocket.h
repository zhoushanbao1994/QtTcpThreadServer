#ifndef TCPSOCKET_H
#define TCPSOCKET_H

#include <QTcpSocket>
#include <QQueue>
#include <QFutureWatcher>
#include <QByteArray>

#include <QTime>


class TcpSocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TcpSocket(qintptr socketDescriptor, QObject *parent = 0);
    ~TcpSocket();

    // 用来处理数据的函数
    //QByteArray handleData(QByteArray data,const QHostAddress & ip, qint16 port);

signals:
    void readData_signal(const qintptr socketDescriptor, const QHostAddress &, const quint16,const QByteArray &);
    //NOTE:断开连接的用户信息，此信号必须发出！线程管理类根据信号计数的
    void sockDisConnect(const qintptr socketDescriptor, const QHostAddress &,const quint16, QThread *);
public slots:
    //发送信号的槽
    void sentDataSlot(const qintptr socketDescriptor, const QByteArray &);
    // 断开连接
    void disConTcp(const qintptr socketDescriptor);

protected slots:
    void readData_slot();//接收数据

protected:
    QQueue<QByteArray> m_datas;             // 提供队列的通用容器
private:
    qintptr m_socketID;                     // 套接字描述符
    QMetaObject::Connection m_dis;          // 信号与槽的连接状态，连接时赋值，断开时根据值断开
};

#endif // TCPSOCKET_H
