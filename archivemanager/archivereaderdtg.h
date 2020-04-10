#ifndef ARCHIVEREADERDTG_H
#define ARCHIVEREADERDTG_H

#include "iarchivereader.h"
#include <QFile>
#include "parsedvector.h"




class ArchiveReaderDTG : public IArchiveReader
{
    Q_OBJECT
public:

    struct TAChanelData{
        int n;
        QFile F;
        GtBuffer *B;
        int npos; // текущая позиция в файле
        TAChanelData(){npos=-1;}
    };


     ArchiveReaderDTG(QObject *parent = nullptr);
    virtual ~ArchiveReaderDTG(){}
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

#endif // ARCHIVEREADERDTG_H
