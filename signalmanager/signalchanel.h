
/*
 *  Класс именованный канал данных
 *
*/

#ifndef GtBuffer_H
#define GtBuffer_H

#include <QObject>
#include "gtbuffer.h"

const int TGtBuffer_MAX_DATA = 1008;

class  SignalChanel : public GtBuffer
{
    Q_OBJECT

public:
    GtBuffer *B;

    explicit SignalChanel(QObject *parent,GtBuffer *B);
    virtual ~ SignalChanel();

    bool    getSignal1bit(quint16 chanelOffset);

    qint64 getSignal1bitTms(quint16 chanelOffset);

    bool setSignal1bit(quint16 chanelOffset, bool val);

    void send();
    QTime sendTime;

    void prepareDataChange();   // сохраняет текущий буфер для последующего сравнения
    bool isDataChanged();       // сравнивает сохраненный с текущим
    void updateSignalsChangeTime();


signals:

public slots:
    void B_bufferChanged(GtBuffer *B);
protected:

    QByteArray A2Cmp;
    struct TSignalsChangeTime{
        QDateTime T[TGtBuffer_MAX_DATA*8];
    };
    TSignalsChangeTime FSignalsChangeTime;


};

#endif // GtBuffer_H
