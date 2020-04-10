#ifndef ARCHIVEMANAGER_H
#define ARCHIVEMANAGER_H

/*
 * класс обертка для работы с архивами разных типов

*/

#include <QObject>
#include <QDateTime>
#include <QVarLengthArray>
#include <QVector>
#include <QFile>
#include <QMap>

#include "iarchivereader.h"



class ArchiveManager : public IArchiveReader
{
    Q_OBJECT
public:


    explicit ArchiveManager(QObject *parent = nullptr);
    ~ArchiveManager();

    void addArchiveReader(IArchiveReader *r);

    void setGtBuffers(GtBuffers*gtBuffers);
    GtBuffers* getBuffers() const {return gtBuffers;}
    QList<TArchiveSrc> findArchiveSrc(QString path);
    bool setArchiveSrc(TArchiveSrc asrc);

    QDateTime beginTime();
    QDateTime endTime();
    int recordsCount();
    QList<GtBuffer*> gtBuffersRecord(int index_pos);
    bool setRecord(int index_pos);

    QDateTime record2time(int npos)  const;
    int time2record(QDateTime &t) const;



    QDateTime currentTime() const;
    int currenRecord() const {return FcurrentRecord;}

    bool go(int napr);
    bool go_sec(int sec);


protected:

    IArchiveReader * activeReader;
    GtBuffers*gtBuffers;
    QVector<IArchiveReader*>vReaders;
    int FcurrentRecord;

signals:
    void buffersChanged(QList<GtBuffer*> l);
    void positionChaged(int newpos);
public slots:
    void reader_progress(int procent);
    void reader_buffersChanged(QList<GtBuffer*> l);
};

#endif // ARCHIVEMANAGER_H
