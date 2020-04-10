/*
 * Список каналов данных
 *
 *
 *                  R:GtBuffers_UDP_D2  <- GtNetB::readDatagrams() GtNetB
 *
 *  B: GtBuffers  <- bufferRecived(...)
 *
 *                  A:GtBuffers <-[chanelChanged(TDatagramPacket2& D2)] IArchiveReader
 *
 *
 *
 *
 *
*/

#include "signalsmanager.h"
#include "gtbuffers_udp_d2.h"


SignalsManager SM;







SignalsManager::SignalsManager(QObject *parent) :
    IGtBufferInterface(parent)
{
    FSignalSource=-1;
    setObjectName("МЕНЕДЖЕР КАНАЛОВ СИГНАЛОВ");
    qRegisterMetaType<TDatagram2>("TDatagram2");
    qRegisterMetaType<QHostAddress>("QHostAddress");


    //FsignalDescriptionTextInterface=nullptr;

    //static QHostAddress ping_maddr("192.168.1.61");
    //static QHostAddress ping_maddr_To("192.168.1.101");
    //static quint16 _port_ping=12351;
    //GtNet *gtPing =new GtNet(this,ping_maddr_To,_port_ping);
    //connect(gtPing,SIGNAL(dtgrmRecivedTextip(QString,QHostAddress&,quint16&)),this,SLOT(dtgrmRecivedText(QString,QHostAddress&,quint16&)));
    //int sz=gtPing->sendTo(TDatagram2(),ping_maddr_To,12351);
    //int sz=gtPing->sendTo(TDatagram2(),ping_maddr_To,7);

    //    QTcpSocket socket;
    //    socket.connectToHost("192.168.1.61", 13214,QIODevice::NotOpen);
    //    qDebug() << socket.error() << socket.errorString();
    //    if (!socket.waitForConnected(-1)) {
    //        qDebug() << "Not Connected" << socket.error() << socket.errorString();
    //    } else {
    //        qDebug() << "Connected";
    //    }


    //static QHostAddress _maddr("224.168.123.4");
    //static quint16 _port_dtg=12347;//12350

    BB=new GtBuffers(this);
    BB->setObjectName("BB");
    //connect(BB   ,SIGNAL(bufferChanged(GtBuffer *)),this,SLOT(bufferBBChanged(GtBuffer *)));
    connect(BB   ,&GtBuffers::buffersListChanged,this,&SignalsManager::buffersListChanged);

    for (int i=0;i<3;i++) XB[i]=nullptr;
    setSignalSource(ssTest);

}

void SignalsManager::setSignalSource(int p)
{
    if (FSignalSource==p) return;
    FSignalSource=p;

    if (XB[p]==nullptr) {
        if (p==ssReal){
            XB[p]=new GtBuffers_UDP_D2(this);
        } else {
            XB[p]=new GtBuffers(this);
        }

        XB[p]->setObjectName(QString("XB[%1]").arg(p));
    }

    // добавляем все каналы
    foreach (GtBuffer *B, BB->allBuffers()) {
        GtBuffer *BX=XB[p]->getGtBuffer(B->type,B->name);
        BX->msecPeriodLive=B->msecPeriodLive;
    }

    // переключаем сигнал
    for (int i=0;i<3;i++) {
        if (XB[i]!=nullptr){
            foreach (GtBuffer *B, XB[i]->allBuffers()) {
                disconnect(B,&GtBuffer::bufferChanged,this,&SignalsManager::bufferXBChanged);
            }
        }
    }
    // перенаправлякм изменения с источников на главный буфер
    foreach (GtBuffer *B, XB[p]->allBuffers()) {
        connect(B,&GtBuffer::bufferChanged,this,&SignalsManager::bufferXBChanged,Qt::UniqueConnection);
        // зачитываем срез
        bufferXBChanged(B);
    }
    emit signalSourceChanged(p);
}


void SignalsManager::bufferXBChanged(GtBuffer *B)
{
    GtBuffer *B1=BB->getGtBuffer(B->type,B->name);
    // записали в общий
    B1->assign_data(B);
    // сообщили всем
    B1->emitBufferChanged();

}

void SignalsManager::bufferBBChanged(GtBuffer *B)
{
    // обратное в тест можно сделать
    emit bufferChanged(B);
}


QList<GtBuffers *> SignalsManager::getXB() const
{
    QList<GtBuffers *> l;
    for (int i=0;i<3;i++) {
        if (XB[i]!=nullptr) l.push_back(XB[i]);
    }
    return l;
}



SignalsManager::~SignalsManager()
{
    foreach (GtBuffer *c, vSignalChanel) {
        delete c;
    }
    vSignalChanel.clear();

}

GtBuffer *SignalsManager::getGtBuffer(int type, const QString &name)
{
    return BB->getGtBuffer(type,name);
}

int SignalsManager::sendGtBuffer(const GtBuffer *B)
{
    if (XB[ssReal]!=nullptr) return XB[ssReal]->sendGtBuffer(B);
    return -1;
}

int SignalsManager::maxSizeData()
{
    if (XB[FSignalSource]!=nullptr) return XB[FSignalSource]->maxSizeData();
    return 0;

}
void SignalsManager::buffersListChanged(GtBuffers *gtBuffers)
{
    foreach (GtBuffer *B, gtBuffers->allBuffers()) {
        registrateGtBuffer(B->type,B->name);
    }
}


GtBuffer *SignalsManager::registrateGtBuffer(int type,const QString &name)
{
    BB->blockSignals(true);
    for (int i=0;i<3;i++) {
        if (XB[i]!=nullptr) {
            XB[i]->blockSignals(true);
            // добавляем во все
            XB[i]->getGtBuffer(type,name);
        }
    }
    // XB->BB
    if (XB[FSignalSource]!=nullptr){
        GtBuffer *B=XB[FSignalSource]->getGtBuffer(type,name);
        connect(B,&GtBuffer::bufferChanged,this,&SignalsManager::bufferXBChanged,Qt::UniqueConnection);
    }
    GtBuffer *B=BB->getGtBuffer(type,name);
    connect(B,&GtBuffer::bufferChanged,this,&SignalsManager::bufferBBChanged,Qt::UniqueConnection);
    int msecPeriodLive=0;
    // выставляем по умолчанию
    switch (type) {
    case 1:msecPeriodLive=5*1000;break;
    }
    if (msecPeriodLive!=0){
        B->setMsecPeriodLive(msecPeriodLive);
        for (int i=0;i<3;i++) {
            if (XB[i]!=nullptr)  XB[i]->getGtBuffer(type,name)->setMsecPeriodLive(msecPeriodLive);
        }
    }
    BB->blockSignals(false);
    for (int i=0;i<3;i++) {
        if (XB[i]!=nullptr)  XB[i]->blockSignals(false);
    }
    return B;

}

SignalChanel *SignalsManager::getSignalChanel(int chanelType, const QString &chanelName)
{
    foreach(SignalChanel *c,vSignalChanel){
        if ((c->B->type==chanelType)&&(c->B->name==chanelName))
            return c;
    }
    GtBuffer *B=BB->getGtBuffer(chanelType,chanelName);
    SignalChanel *c=new  SignalChanel(this,B);
    return c;
}


QDateTime SignalsManager::currentDateTime() const
{
    QDateTime maxT;
    foreach (GtBuffer* B, BB->allBuffers()){
        if ((!maxT.isValid())||(maxT<B->timeDataRecived)) maxT=B->timeDataRecived;
    }
    return maxT;
}

void SignalsManager::slot_bufferSend(const GtBuffer *B)
{
    sendData(B->type, B->name, B->A);
    // возможно будет несколько источников
}

void SignalsManager::sendData(int type, const QString &name, const QByteArray &A)
{
    // шлю только в реале
    // надо бы слот под ошибки..
    GtBuffer B;
    B.setType(type);
    B.setName(name);
    B.setData(A.data(),A.size());
    sendGtBuffer(&B);
}


void SignalsManager::sendData(int type, const QString &name, const char *data, int sz)
{
    QByteArray A=QByteArray::fromRawData(data,sz);
    sendData(type, name, A);
}


