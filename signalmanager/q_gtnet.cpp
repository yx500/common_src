/*
 *  Прием / передача датаграмм по сети
 *
 *
*/

#include "q_gtnet.h"

#include <utility>


static QHostAddress _maddr("224.168.123.4");
static quint16 _port=12347;//12350

GtNet::GtNet(QObject *parent) :
    QThread(parent)
{
#ifdef Q_OS_WIN
    timerrReadDatagrams=nullptr;
#endif
    socketRead=nullptr;
    socketSend=nullptr;
    Open( _maddr, _port );
    moveToThread(this);

}

GtNet::GtNet(QObject *parent, const QHostAddress& groupAddress, quint16 port):
    QThread(parent)

{
#ifdef Q_OS_WIN
    timerrReadDatagrams=nullptr;
#endif
    socketRead=nullptr;
    socketSend=nullptr;
    Open( groupAddress, port );
    moveToThread(this);
}

GtNet::~GtNet()
{
    delete socketRead;
    delete socketSend;
}

void GtNet::run()
{
    socketRead = new QUdpSocket(this);

#ifdef Q_OS_WIN
    if (timerrReadDatagrams) delete timerrReadDatagrams;
    timerrReadDatagrams=new QTimer(this);
    connect(timerrReadDatagrams,SIGNAL(timeout()),this,SLOT(readDatagrams()));
    timerrReadDatagrams->start(10);
    qDebug() << "GtNet by timer " << timerrReadDatagrams->interval() << "ms" << endl;
#endif

    //QHostAddress haddr("192.168.1.47");
    //if ( ! msocket->bind(haddr/*QHostAddress::AnyIPv4*/, port, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress) )
    if ( ! socketRead->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress) )
        qCritical("GtNet::Open error: %s",qPrintable(socketRead->errorString()));
    else {
        socketRead->joinMulticastGroup(groupAddress);
        qDebug()
                << "GtNet group address" << groupAddress.toString() << ":" << port
                << " on " << socketRead->localAddress().toString()<<":" << socketRead->localPort()
                << endl;
#ifndef Q_OS_WIN
        connect(socketRead, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
        qDebug() << "GtNet by slot " << endl;
#endif
        connect(socketRead, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socket_error(QAbstractSocket::SocketError)));
        connect(socketRead, SIGNAL(disconnected()), this, SLOT(socket_disconnected()));


    }
    socketSend = new QUdpSocket(this);
    if ( ! socketSend->bind(QHostAddress::AnyIPv4, 0, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress) )
        qCritical("GtNet::Open error: %s",qPrintable(socketSend->errorString()));
    exec();
}
void GtNet::Open(QHostAddress groupAddress, quint16 port)
{
    this->port=port;
    this->groupAddress=std::move(groupAddress);
}

void GtNet::readDatagrams()
{
    QByteArray dat;
    QHostAddress sender;
    quint16 senderPort;
    while (socketRead->hasPendingDatagrams()) {
        size_t sz = socketRead->pendingDatagramSize();
        dat.resize(sz);
        socketRead->readDatagram(dat.data(), dat.size(), &sender, &senderPort);
        static const QMetaMethod valueChangedSignal = QMetaMethod::fromSignal(&GtNet::dtgrmRecivedText);
        if (isSignalConnected(valueChangedSignal)) {
            QString str;
            if (utf8)
                str = QString::fromUtf8( dat.data() ); else
                str = QString::fromLocal8Bit(dat.data() );
            emit dtgrmRecivedText(str,sender, senderPort);
        } else {
            auto  *dtgrm = reinterpret_cast<TDatagram2*>( dat.data() );
            if(sz == ( dtgrm->Size + TDatagram2_HEADER_LN ) ){
                memset(&_dtgrm,0,sizeof(_dtgrm));
                memcpy(&_dtgrm,dtgrm,sz);
                emit dtgrmRecived( _dtgrm,sender, senderPort );
            }
        }
    }
}

void GtNet::socket_disconnected()
{
    qWarning() << "GtNet::socket_disconnected()"<< endl;
    Open( groupAddress, port );
}

void GtNet::socket_error(QAbstractSocket::SocketError)
{
    qCritical("GtNet::socket_error: %s",qPrintable(socketRead->errorString()));
    Open( groupAddress, port );
}


size_t GtNet::send(const TDatagram2& dtgrm)
{
    return socketSend->writeDatagram( (const char*)&dtgrm, dtgrm.Size + TDatagram2_HEADER_LN, groupAddress, port );
}

size_t GtNet::sendTo(const TDatagram2& dtgrm,const QHostAddress& addressTo,quint16 portTo)
{
    return socketSend->writeDatagram( (const char*)&dtgrm, dtgrm.Size + TDatagram2_HEADER_LN, addressTo, portTo );
}


void GtNet::dtgrmDataSend(const TDatagram2& Data)
{
    send(Data);
}

