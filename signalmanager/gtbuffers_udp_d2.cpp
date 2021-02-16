#include "gtbuffers_udp_d2.h"


#include <utility>

QAtomicInteger<int> emit_counter;


static QHostAddress _maddr("224.168.123.4");
static quint16 _port=12347;//12350

GtNetB::GtNetB(QObject *parent,GtBuffers *BB) :
    QThread(parent)
{
#ifdef Q_OS_WIN
    timerrReadDatagrams=nullptr;
#endif
    this->BB=BB;
    emit_counter=0;
    socketRead=nullptr;
    socketSend=nullptr;
    Open( _maddr, _port );
    moveToThread(this);

}

GtNetB::~GtNetB()
{
    delete socketRead;
    delete socketSend;
}

void GtNetB::run()
{
    socketRead = new QUdpSocket(this);

#ifdef Q_OS_WIN
    if (timerrReadDatagrams) delete timerrReadDatagrams;
    timerrReadDatagrams=new QTimer(this);
    connect(timerrReadDatagrams,SIGNAL(timeout()),this,SLOT(readDatagrams()));
    timerrReadDatagrams->start(10);
    qDebug() << "GtNetB by timer " << timerrReadDatagrams->interval() << "ms" << endl;
#endif

    if ( ! socketRead->bind(QHostAddress::AnyIPv4, port, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress) )
        qCritical("GtNetB::Open error: %s",qPrintable(socketRead->errorString()));
    else {
        socketRead->joinMulticastGroup(groupAddress);
        qDebug()
                << "GtNetB group address" << groupAddress.toString() << ":" << port
                << " on " << socketRead->localAddress().toString()<<":" << socketRead->localPort()
                << endl;
#ifndef Q_OS_WIN
        connect(socketRead, SIGNAL(readyRead()), this, SLOT(readDatagrams()));
        qDebug() << "GtNetB by slot " << endl;
#endif
        connect(socketRead, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socket_error(QAbstractSocket::SocketError)));
        connect(socketRead, SIGNAL(disconnected()), this, SLOT(socket_disconnected()));


    }
    socketSend = new QUdpSocket(this);
    if ( ! socketSend->bind(QHostAddress::AnyIPv4, 0, QUdpSocket::ReuseAddressHint | QUdpSocket::ShareAddress) )
        qCritical("GtNetB::Open error: %s",qPrintable(socketSend->errorString()));


    checkLiveStatusTimer=new QTimer(this);
    connect(checkLiveStatusTimer, SIGNAL(timeout()), this, SLOT(checkLiveStatus()));
    checkLiveStatusTimer->start(1000);

    exec();
}
void GtNetB::Open(QHostAddress groupAddress, quint16 port)
{
    this->port=port;
    this->groupAddress=std::move(groupAddress);
}

void GtNetB::readDatagrams()
{
    QHostAddress sender;
    quint16 senderPort;
    while (socketRead->hasPendingDatagrams()) {
        auto sz = socketRead->pendingDatagramSize();
        if ((sz>=(int)TDatagram2_HEADER_LN)&&(sz<=(int)TDatagram2_SZ)){
            memset(&_dtgrm,0,sizeof(_dtgrm));
            socketRead->readDatagram((char*)&_dtgrm, sz, &sender, &senderPort);
            if(sz == (int)(_dtgrm.Size + TDatagram2_HEADER_LN ) ){
                if (BB!=nullptr){
                    GtBuffer *D=BB->getBufferEx(_dtgrm.Type,_dtgrm.Name);
                    if ((D!=nullptr)&& (!D->static_mode)&&(!D->shared_mem)){
                        D->timeDataRecived=QDateTime::currentDateTime();
                        // тут бы блокировку от чтения поставить
                        if ((D->A.size()!=_dtgrm.Size) || (D->tu_style)|| (memcmp(D->A.data(),&_dtgrm.Data,_dtgrm.Size)!=0)) {
                            emit_counter++;
                            emit changeBuffer(D,_dtgrm);
                        }
                    }

                }
            }
        } else {
            QByteArray dat;
            dat.resize(sz);
            socketRead->readDatagram(dat.data(), dat.size(), &sender, &senderPort);
        }
    }

}

void GtNetB::socket_disconnected()
{
    qWarning() << "GtNetB::socket_disconnected()"<< endl;
    Open( groupAddress, port );
}

void GtNetB::socket_error(QAbstractSocket::SocketError)
{
    qCritical("GtNetB::socket_error: %s",qPrintable(socketRead->errorString()));
    Open( groupAddress, port );
}


size_t GtNetB::send(const TDatagram2& dtgrm)
{
    return socketSend->writeDatagram( (const char*)&dtgrm, dtgrm.Size + TDatagram2_HEADER_LN, groupAddress, port );
}

void GtNetB::bufferSend(TDatagram2 dtgrm)
{
    send(dtgrm);
}



//void GtNetB::dtgrmDataSend(const TDatagram2& Data)
//{
//    send(Data);
//}

void GtNetB::checkLiveStatus()
{
    if (BB==nullptr) return;
    QDateTime curTime=QDateTime::currentDateTime();
    foreach (GtBuffer *B, BB->allBuffers()) {
        int sost=GtBuffer::_alive;
        if ((B->msecPeriodLive>0)&&(!B->static_mode)){
            if (B->timeDataRecived.isValid()){
                if ((B->timeDataRecived.msecsTo(curTime)>B->msecPeriodLive)){
                    sost=GtBuffer::_error;
                }
            }
        }
        if (sost!=B->sost) {
            B->sost=sost;
        }
    }
}




GtBuffers_UDP_D2::GtBuffers_UDP_D2(QObject *parent)
    :GtBuffers(parent)
{
    qRegisterMetaType<TDatagram2>("TDatagram2");
    gtNetB =new GtNetB(nullptr,this);
    connect(gtNetB,&GtNetB::changeBuffer,this,&GtBuffers_UDP_D2::bufferChanged);
    connect(this,&GtBuffers_UDP_D2::bufferSend,gtNetB,&GtNetB::bufferSend);
    //initSockets();
    gtNetB->start();
    gtNetB->setPriority(QThread::HighPriority);
    QThread::msleep(100);

    //    static quint16 _port_text=12350;
    //    gtNetText =new GtNet(nullptr,_maddr,_port_text);
    //    connect(gtNetText,SIGNAL(dtgrmRecivedText(QString,QHostAddress,quint16)),this,SLOT(dtgrmRecivedText(QString,QHostAddress,quint16)));
    //    connect(gtNetText, &GtNet::finished, gtNetText, &QObject::deleteLater);
    //    gtNetText->start();
}

GtBuffers_UDP_D2::~GtBuffers_UDP_D2()
{
    if (gtNetB!=nullptr) {
        gtNetB->quit();
        //if (gtNetText) gtNetText->quit();
        QThread::msleep(1000);
        //delete gtNetB;
        //delete gtNetText;
    }
}

int GtBuffers_UDP_D2::sendData(int type, const QString &name, const QByteArray &A)
{
    TDatagram2 Data;
    Data.Clear();
    Data.setName(name.toLocal8Bit().data());
    Data.Type=type;
    Data.Size=A.size();
    if (Data.Size>sizeof(Data.Data)) Data.Size=sizeof(Data.Data);
    Data.setData(A.data(),A.size());
    emit bufferSend(Data);
    return 0;
}

int GtBuffers_UDP_D2::sendGtBuffer(const GtBuffer *B)
{
    if (slaveMode){
        auto name=B->name;
        if (!name.isEmpty())name[1]='_';
        return sendData(B->type, name,B->A);
    }else {
        return sendData(B->type, B->name,B->A);
    }
}

int GtBuffers_UDP_D2::get_emit_counter() const
{
    if (gtNetB!=nullptr) return emit_counter;
    return 0;
}

void GtBuffers_UDP_D2::bufferChanged(GtBuffer *B, TDatagram2 dtgrm)
{

    if (gtNetB!=nullptr) emit_counter--;
    B->tick++;
    B->timeDataChanged=B->timeDataRecived;
    if (B->A.size()!=dtgrm.Size) {
        B->A.clear();
        B->A.resize(dtgrm.Size);
    }
    memcpy(B->A.data(),&dtgrm.Data,dtgrm.Size);
    // тут бы блокировку от чтения снять
    B->emitBufferChanged();
}



