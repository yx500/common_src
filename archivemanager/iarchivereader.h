#ifndef IARCHIVEREADER_H
#define IARCHIVEREADER_H

#include <QObject>
#include <QDateTime>
#include "gtbuffers.h"



class IArchiveReader : public QObject
{
    Q_OBJECT
public:

    struct TArchiveSrc{
        IArchiveReader * reader;
        QDateTime T1,T2;
        QString params;
        bool	operator == (const TArchiveSrc &other) const {
            return (reader==other.reader &&
                  T1==other.T1 && T2==other.T2 && params==other.params);
        }
    };

    explicit IArchiveReader(QObject *parent = nullptr):QObject(parent){}
    virtual ~IArchiveReader(){}

    virtual void setGtBuffers(GtBuffers*gtBuffers)=0;

    virtual QList<TArchiveSrc> findArchiveSrc(QString path)=0;

    virtual bool setArchiveSrc(TArchiveSrc asrc)=0;

    virtual QDateTime beginTime()=0;
    virtual QDateTime endTime()=0;
    virtual int recordsCount()=0;
    virtual QList<GtBuffer*> gtBuffersRecord(int index_pos)=0;
    virtual QDateTime record2time(int npos) const=0;
    virtual int time2record(QDateTime &t) const=0;

    virtual void clear(){}

signals:
    void progress(int procent);

public slots:
};

#endif // IARCHIVEREADER_H
