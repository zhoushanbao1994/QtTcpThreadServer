#include <QCoreApplication>
#include "tcpserver.h"
#include <QDebug>
#include <QFile>
#include <QDir>
#include <QTextStream>
#include <iostream>
#include <QThread>
#ifndef Q_OS_WIN
#include "eventdispatcher_libev/eventdispatcher_libev.h"
#endif

void customMessageHandler(QtMsgType type, const QMessageLogContext &context,const QString & msg)
{
        QString txt;
        switch (type) {
        //调试信息提示
        case QtDebugMsg:
//            txt = QString("%1: Debug: at:%2,%3 on %4; %5").arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
//                    .arg(context.file).arg(context.line).arg(context.function).arg(msg);
            txt = msg;
                break;

        //一般的warning提示
        case QtWarningMsg:
                txt = QString("%1:Warning: at:%2,%3 on %4; %5").arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
                        .arg(context.file).arg(context.line).arg(context.function).arg(msg);
        break;
        //严重错误提示
        case QtCriticalMsg:
                txt = QString("%1:Critical: at:%2,%3 on %4; %5").arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
                        .arg(context.file).arg(context.line).arg(context.function).arg(msg);
        break;
        //致命错误提示
        case QtFatalMsg:
                txt = QString("%1:Fatal: at:%2,%3 on %4; %5").arg(QTime::currentTime().toString("hh:mm:ss.zzz"))
                        .arg(context.file).arg(context.line).arg(context.function).arg(msg);
                abort();
        }
        QFile outFile(QString("%1/%2.txt").arg(QDir::currentPath()).arg(QDate::currentDate().toString("yyyy-MM-dd")));
        outFile.open(QIODevice::WriteOnly | QIODevice::Append);
        QTextStream ts(&outFile);
        ts << txt << endl;
}


int main(int argc, char *argv[])
{
    //    qInstallMessageHandler(customMessageHandler);
#ifndef Q_OS_WIN
    QCoreApplication::setEventDispatcher(new EventDispatcherLibEv());
#endif
    QCoreApplication a(argc, argv);
    qDebug() << QThread::currentThread() << "程序启动" ;
    TcpServer ser;
    if(ser.listen(QHostAddress::Any,6666)) {
        qDebug() << QThread::currentThread() << "Liston Port 6666 Success";
    }
    else {
        qDebug() << QThread::currentThread() << "Liston Port 6666 Error";
    }

    // 新用户连接
    QObject::connect(&ser, &TcpServer::connectClient,
                     [&](const int id, const QString & addr, const quint16 port)//发送新用户连接信息
    {
        qDebug() << QThread::currentThread() << id << addr << port;
    });
    // 获得用户发过来的数据
    QObject::connect(&ser, &TcpServer::readData,
                     [&](const int id ,const QString &addr , quint16 port, const QByteArray &data)//发送获得用户发过来的数据
    {
        qDebug() << QThread::currentThread() << id << addr << port << data;
    });
    // 断开连接的用户信息
    QObject::connect(&ser, &TcpServer::sockDisConnect,
                     [&](const int id ,const QString &addr , quint16 port)// 断开连接的用户信息
    {
        qDebug() << QThread::currentThread() << id << addr << port;
    });

    return a.exec();
}
