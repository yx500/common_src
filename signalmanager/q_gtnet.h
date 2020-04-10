

#ifndef GtNet_H
#define GtNet_H

#include <QObject>
#include <QThread>
#include <QtNetwork>
#include "dmpacket2.h"

/*
 *  Прием / передача датаграмм по сети
*/

class GtNet :  public QThread
{
    Q_OBJECT

    void Open(QHostAddress groupAddress, quint16 port);
    QHostAddress groupAddress;
    quint16 port{};


private slots:
    void readDatagrams();
    void socket_disconnected();
    void socket_error(QAbstractSocket::SocketError);

public:
    explicit GtNet(QObject *parent = nullptr);
    explicit GtNet(QObject *parent,const QHostAddress& groupAddress, quint16 port);
    virtual ~GtNet() override;
    void run() override ;
    size_t send(const TDatagram2& dtgrm);
    size_t sendTo(const TDatagram2& dtgrm,const QHostAddress& addressTo,quint16 portTo);
    bool utf8=true;

signals:
    void dtgrmRecived(const TDatagram2& Data,const QHostAddress &sender,const quint16  &senderPort);
    void dtgrmRecivedText(const QString &s,const QHostAddress &sender,const quint16  &senderPort);

public slots:
    void dtgrmDataSend(const TDatagram2& Data);

private:
    QUdpSocket *socketRead;
    QUdpSocket *socketSend;
    TDatagram2  _dtgrm;
#ifdef Q_OS_WIN
    QTimer * timerrReadDatagrams;
#endif


};

#endif // GtNet_H
