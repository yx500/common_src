#ifndef GTBUFFERS_UDP_D2_H
#define GTBUFFERS_UDP_D2_H

#include "gtbuffers.h"

#include <QThread>
#include <QtNetwork>
#include "dmpacket2.h"

/*
 *  Прием / передача датаграмм по сети
*/

class GtNetB :  public QThread
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
    explicit GtNetB(QObject *parent, GtBuffers *BB);
    virtual ~GtNetB() override;
    void run() override ;
    size_t send(const TDatagram2& dtgrm);

signals:
    void changeBuffer(GtBuffer *B,TDatagram2 dtgrm);

public slots:
    //void dtgrmDataSend(const TDatagram2& Data);
    void bufferSend(const GtBuffer *B);
    void checkLiveStatus();


private:
    QUdpSocket *socketRead;
    QUdpSocket *socketSend;
    TDatagram2  _dtgrm;
    GtBuffers *BB;
    QTimer *checkLiveStatusTimer;

#ifdef Q_OS_WIN
    QTimer * timerrReadDatagrams;
#endif


};



class GtBuffers_UDP_D2 : public GtBuffers
{
    Q_OBJECT
public:
    GtBuffers_UDP_D2(QObject *parent=Q_NULLPTR);
    virtual ~GtBuffers_UDP_D2() override;
    int sendData(int type,const QString &name,const QByteArray &A);
    int sendGtBuffer(const GtBuffer *B) override;
    int maxSizeData() override {return TDatagram2_DATA_LN;}

public slots:
    void bufferChanged(GtBuffer *B,TDatagram2 dtgrm);

signals:
void bufferSend(const GtBuffer *B);

protected:
    GtNetB *gtNetB;

};

#endif // GTBUFFERS_UDP_D2_H
