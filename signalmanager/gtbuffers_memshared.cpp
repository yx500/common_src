#include "gtbuffers_memshared.h"

GtNetSharedMem::GtNetSharedMem(QObject *parent,GtBuffer *D) :
    QThread(parent)
{

    this->D=D;
    emit_counter=0;
    timerrReadDatagrams=nullptr;
    mem=new QSharedMemory(this);
    mem->setKey(D->name);
    mem->attach(QSharedMemory::ReadOnly);
    if (mem->isAttached())
        moveToThread(this);

}

GtNetSharedMem::~GtNetSharedMem()
{
}

void GtNetSharedMem::run()
{
    if (timerrReadDatagrams) delete timerrReadDatagrams;
    timerrReadDatagrams=new QTimer(this);
    connect(timerrReadDatagrams,SIGNAL(timeout()),this,SLOT(readDatagrams()));
    timerrReadDatagrams->start(1);

    checkLiveStatusTimer=new QTimer(this);
    connect(checkLiveStatusTimer, SIGNAL(timeout()), this, SLOT(checkLiveStatus()));
    checkLiveStatusTimer->start(1000);

    exec();
}


void GtNetSharedMem::readDatagrams()
{

    TDatagramPacket2 *_pck=(TDatagramPacket2 *)(mem->data());
    if ((unsigned int)mem->size()>=sizeof (TDatagramPacket2)){
        if (_pck->tick!=pck_tick){
            mem->lock();
            {
                pck_tick=_pck->tick;
                memcpy(_dtgrm.Data,_pck->Dtgrm.Data,sizeof(_dtgrm.Data));
                _dtgrm.Size=sizeof(_dtgrm.Data);

                if ((D!=nullptr)&& (!D->static_mode)){
                    D->timeDataRecived=QDateTime::currentDateTime();
                    if ((D->A.size()!=_dtgrm.Size) || (memcmp(D->A.data(),&_dtgrm.Data,_dtgrm.Size)!=0)) {
                        emit_counter++;
                        emit changeBuffer(D,_dtgrm);
                    }
                }

            }
            mem->unlock();

        }
    }

}

void GtNetSharedMem::checkLiveStatus()
{
    QDateTime curTime=QDateTime::currentDateTime();
    int sost=GtBuffer::_error;
    if (D->msecPeriodLive>0){
        if (D->timeDataRecived.isValid()){
            if ((D->timeDataRecived.msecsTo(curTime)<=D->msecPeriodLive)){
                sost=GtBuffer::_alive;
            }
        }
    } else sost=GtBuffer::_alive;
    if (sost!=D->sost) {
        D->sost=sost;
    }
}
