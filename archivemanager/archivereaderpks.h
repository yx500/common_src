#ifndef ARCHIVEREADERPKS_H
#define ARCHIVEREADERPKS_H

#include "iarchivereader.h"
#include <QFile>
#include "parsedvector.h"


struct TAChanelData{
    int n;
    GtBuffer *B;
    int npos; // текущая позиция в индексе
    TAChanelData(){npos=-1;}
};

class ArchiveReaderPKS : public IArchiveReader
{
    Q_OBJECT
public:
     ArchiveReaderPKS(QObject *parent = nullptr);
     virtual ~ArchiveReaderPKS(){}

     void setGtBuffers(GtBuffers *gtBuffers);

     QList<TArchiveSrc> findArchiveSrc(QString path);
     bool setArchiveSrc(TArchiveSrc asrc);

     QDateTime beginTime();
     QDateTime endTime();
     int recordsCount();
     QList<GtBuffer*> gtBuffersRecord(int index_pos);
     QDateTime record2time(int npos)  const;
     int time2record(QDateTime &t) const;

     void clear();
     bool loadPKS2(QString fileName);

signals:

public slots:

protected:
     GtBuffers *BB;
     QFile FPKS2;
     ParsedVector<QDateTime,TAChanelData*,int> pv;
     QVector<TAChanelData*> vAChanels;
     QMap<int, QMap<QString, TAChanelData*> > m_TypeToNameToChanel;
     TAChanelData * chanel(quint16 type, const QString &name);
     bool readDatagramPacket2(int npos, GtBuffer *B);
};

#endif // ARCHIVEREADERPKS_H
