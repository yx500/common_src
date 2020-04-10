/*
 * Список каналов данных
*/


#ifndef SIGNALSMANAGER_H
#define SIGNALSMANAGER_H

#include <QMap>
#include <QTimer>
#include <QObject>
#include "QDate"
#include <QVector>

#include "gtbuffers.h"
//#include "signaldescription.h"
#include "signalchanel.h"

class SignalsManager : public IGtBufferInterface
{
    Q_OBJECT
public:

    enum {ssReal=0,ssArchive=1,ssTest=2};

    GtBuffers *BB;
    GtBuffers *XB[3];


    explicit SignalsManager(QObject *parent = nullptr);

    virtual ~SignalsManager() override;
    GtBuffer *getGtBuffer(int type,const QString &name)override;
    int sendGtBuffer(const GtBuffer *B)override;
    int maxSizeData()override;


    GtBuffer* registrateGtBuffer(int type, const QString &name);
    SignalChanel *getSignalChanel(int chanelType, const QString &chanelName);

    int  signalSource(){return FSignalSource;}
    void setSignalSource(int p);

    QDateTime currentDateTime() const;

    void sendData(int type, const QString &name, const char *data, int sz);
    void sendData(int type, const QString &name, const QByteArray &A);

    QList<GtBuffers *> getXB() const;


signals:

    void bufferChanged(GtBuffer *B);
    void signalSourceChanged(int ss);

public slots:
    void bufferXBChanged(GtBuffer *B);
    void bufferBBChanged(GtBuffer *B);
    void buffersListChanged(GtBuffers *gtBuffers);
    void slot_bufferSend(const GtBuffer *B);

protected:
    int FSignalSource;
    QVector<SignalChanel*> vSignalChanel;



};

extern SignalsManager SM;



#endif // SIGNALSMANAGER_H
