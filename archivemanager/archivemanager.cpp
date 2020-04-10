#include "archivemanager.h"

#include <qdir.h>
#include <qdebug.h>



ArchiveManager::ArchiveManager(QObject *parent) : IArchiveReader(parent)
{
    FcurrentRecord=0;
    gtBuffers=nullptr;
}

ArchiveManager::~ArchiveManager()
{
}

void ArchiveManager::addArchiveReader(IArchiveReader *r)
{
    vReaders.push_back(r);
}

void ArchiveManager::setGtBuffers(GtBuffers *gtBuffers)
{
    this->gtBuffers=gtBuffers;
}

QList<IArchiveReader::TArchiveSrc> ArchiveManager::findArchiveSrc(QString path)
{
    QList<TArchiveSrc> l;
    foreach (IArchiveReader *r, vReaders) {
        QList<TArchiveSrc> l1=r->findArchiveSrc(path);
        l.append(l1);
    }
    return l;
}

bool ArchiveManager::setArchiveSrc(IArchiveReader::TArchiveSrc asrc)
{
    activeReader=asrc.reader;
    connect(activeReader,SIGNAL(progress(int)),this,SLOT(reader_progress(int)));
    activeReader->clear();
    activeReader->setGtBuffers(gtBuffers);
    activeReader->setArchiveSrc(asrc);
    return setRecord(0);

}


QDateTime ArchiveManager::beginTime()
{
    return activeReader->beginTime();
}

QDateTime ArchiveManager::endTime()
{
    return activeReader->endTime();
}

int ArchiveManager::recordsCount()
{
    return activeReader->recordsCount();
}

QList<GtBuffer *> ArchiveManager::gtBuffersRecord(int index_pos)
{
    return activeReader->gtBuffersRecord(index_pos);
}

bool ArchiveManager::setRecord(int index_pos)
{
    if (FcurrentRecord!=index_pos){
        FcurrentRecord=index_pos;
        QList<GtBuffer*> l=activeReader->gtBuffersRecord(index_pos);
        foreach (GtBuffer * gtBuffer, l) {
            gtBuffer->emitBufferChanged();
        }
        emit positionChaged(index_pos);
        emit buffersChanged(l);
        return true;
    }
    return false;
}

QDateTime ArchiveManager::record2time(int npos) const
{
    return activeReader->record2time(npos);
}

int ArchiveManager::time2record(QDateTime &t) const
{
    return activeReader->time2record(t);
}

QDateTime ArchiveManager::currentTime() const
{
    return record2time(FcurrentRecord);
}

bool ArchiveManager::go(int napr)
{
    if (napr==1) return setRecord(FcurrentRecord+1);
    if (napr==-1) return setRecord(FcurrentRecord-1);
    if (napr==11) return setRecord(recordsCount()-1);
    if (napr==-11) return setRecord(0);
    return false;
}


bool ArchiveManager::go_sec(int sec)
{
    if (!currentTime().isValid()) return false;
    QDateTime newTime=currentTime().addSecs(sec);
    int npos=activeReader->time2record(newTime);
    return setRecord(npos);
}

void ArchiveManager::reader_progress(int procent)
{
    emit progress(procent);
}

void ArchiveManager::reader_buffersChanged(QList<GtBuffer *> l)
{
    emit buffersChanged(l);
}




