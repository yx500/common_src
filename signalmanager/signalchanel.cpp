/*
 *  Работа с именованным канало данных
 *
*/

#include "signalchanel.h"
#include <QFileInfo>
#include <QDebug>
#include "signaldescription.h"


SignalChanel::SignalChanel(QObject *parent,GtBuffer *B) :
    GtBuffer(parent)
{
    this->B=B;
    connect(B,SIGNAL(bufferChanged(GtBuffer *)),this,SLOT(B_bufferChanged(GtBuffer *)));
}

SignalChanel::~SignalChanel()
{
}




bool SignalChanel::getSignal1bit(quint16 chanelOffset)
{
    const quint8 *TSBUF;
    uint sz=TGtBuffer_MAX_DATA;
    TSBUF=(const quint8 *)B->A.data();
    // тут эмуляция старого пакета ТС 8 бит, со смещением начала на 1 байт
    if (B->type==1){
        chanelOffset++;
        sz=TGtBuffer_MAX_DATA-1;
    }
    uint n_im=chanelOffset;
    uint nbyte = n_im/8;
    uint nbit = n_im%8;
    if (nbyte<sz){
        quint8 r=( (TSBUF[nbyte]>>nbit) & 0x01 ) ? 1 : 0;
        return r;
    }
    return false;
}


qint64 SignalChanel::getSignal1bitTms(quint16 chanelOffset)
{
    if (chanelOffset>=TGtBuffer_MAX_DATA*8) return -1;
    if (!FSignalsChangeTime.T[chanelOffset].isValid()) FSignalsChangeTime.T[chanelOffset]=QDateTime::currentDateTime();
    return FSignalsChangeTime.T[chanelOffset].msecsTo(QDateTime::currentDateTime());
}

bool SignalChanel::setSignal1bit(quint16 chanelOffset, bool val)
{
    quint8 *TSBUF;
    int sz=TGtBuffer_MAX_DATA;
    TSBUF=(quint8 *)B->A.data();
    // тут эмуляция старого пакета ТС 8 бит, со смещением начала на 1 байт
    if (B->type==1){
        chanelOffset++;
        sz=TGtBuffer_MAX_DATA-1;

    }
    quint16 n_im=chanelOffset;
    quint16 nbyte = n_im/8;
    quint8 nbit = n_im%8;
    if (nbyte>=sz) return false;
    SignalDescription::setbit8(&TSBUF[nbyte],nbit,val);
    return true;
}


void SignalChanel::prepareDataChange()
{
    A2Cmp=B->A;
}

bool SignalChanel::isDataChanged()
{
    return A2Cmp!=A;
}

void SignalChanel::updateSignalsChangeTime()
{
    QDateTime T=QDateTime::currentDateTime();
        quint8 *TSBUF1;
        quint8 *TSBUF2;
        int sz=TGtBuffer_MAX_DATA;
        int s=0;
        TSBUF1=(quint8 *)B->A.data();
        TSBUF2=(quint8 *)A2Cmp.data();
        if (B->type==1){
            s=1;
            sz=TGtBuffer_MAX_DATA-1;
        for (int nbyte=0;nbyte<sz;nbyte++){
            if (TSBUF1[nbyte+s]!=TSBUF2[nbyte+s]){
                for (int nbit=0;nbit<8;nbit++){
                    quint8 r1=( (TSBUF1[nbyte+s]>>nbit) & 0x01 ) ? 1 : 0;
                    quint8 r2=( (TSBUF2[nbyte+s]>>nbit) & 0x01 ) ? 1 : 0;
                    if(r1!=r2) FSignalsChangeTime.T[nbyte*8+nbit]=T;
                }
            }
        }
    }
}

void SignalChanel::B_bufferChanged(GtBuffer *)
{
    updateSignalsChangeTime();
}








