#ifndef GTBUFFERS_MEMSHARED_H
#define GTBUFFERS_MEMSHARED_H

#include <QThread>
#include "gtbuffers.h"
#include <QAtomicInteger>
#include <QTimer>
#include "dmpacket2.h"
#include <QSharedMemory>

/*
 *  Прием / передача датаграмм по сети
*/

class GtNetSharedMem :  public QThread
{
    Q_OBJECT

private slots:
    void readDatagrams();

public:
    explicit GtNetSharedMem(QObject *parent, GtBuffer *D);
    virtual ~GtNetSharedMem() override;
    void run() override ;
    QAtomicInteger<int> emit_counter;
    QSharedMemory* mem;

signals:
    void changeBuffer(GtBuffer *B,TDatagram2 dtgrm);

public slots:
    void checkLiveStatus();

private:
    GtBuffer *D;
    QTimer *checkLiveStatusTimer;
    QTimer * timerrReadDatagrams;
    unsigned int pck_tick=0;

    TDatagram2  _dtgrm;

};

class GtBuffers_MemShared : public QObject
{
    Q_OBJECT
public:
    explicit GtBuffers_MemShared(QObject *parent = nullptr);

signals:

};

#endif // GTBUFFERS_MEMSHARED_H
