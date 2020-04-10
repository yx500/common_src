#include "archivereaderdtg.h"

#include <qdir.h>
#include "dmpacket2.h"

enum {szPKSrec=512};




ArchiveReaderDTG::ArchiveReaderDTG(QObject *parent) : IArchiveReader(parent)
{

}

void ArchiveReaderDTG::setGtBuffers(GtBuffers *gtBuffers)
{
    clear();
    BB=gtBuffers;
    foreach(GtBuffer *B,BB->allBuffers()){
        TAChanelData *ad=new TAChanelData();
        ad->B=B;
        ad->npos=-1;
        ad->n=vAChanels.size();
        vAChanels.push_back(ad);
        m_TypeToNameToChanel[B->type][B->name]=ad;
    }
}

ArchiveReaderDTG::TAChanelData * ArchiveReaderDTG::chanel(quint16 type, const QString &name)
{
    if (m_TypeToNameToChanel.contains(type))
        if (m_TypeToNameToChanel[type].contains(name))
            return m_TypeToNameToChanel[type][name];

    return nullptr;
}



QList<IArchiveReader::TArchiveSrc> ArchiveReaderDTG::findArchiveSrc(QString path)
{
    QList<TArchiveSrc> l;
    QList<QDate> ld;
    QDir dir(path);
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        QString fileName=fileInfo.fileName();
        QString fileExt=fileInfo.suffix();
        if ((fileExt.toInt()<=31)&&(fileExt.toInt()>=1)&&(fileName.indexOf("dtg_")==0)&&(fileName.length()>=10)){
                    //dtg_001_kom3_ts2.10
                    bool ok1;
                    int day=fileExt.toInt(&ok1);
                    if ((ok1)&&(day<=31)){
                        bool exchanel=false;
                        foreach (TAChanelData *ad, vAChanels) {
                            QString fn=QString("dtg_%1_%2").arg(ad->B->getType(),3,10,QLatin1Char('0')).arg(ad->B->getName());
                            if (fileName==fn){
                                exchanel=true;
                                break;
                            }
                        }
                        if (!exchanel) continue;

                        QDate FD=fileInfo.created().date();
                        TArchiveSrc asrc;
                        asrc.reader=this;
                        asrc.T1=QDateTime(FD);
                        asrc.T2=asrc.T1.addDays(1);
                        asrc.params=fileInfo.absolutePath();

                        if (ld.indexOf(FD)<0) {
                            l.push_back(asrc);
                            ld.push_back(FD);
                        }
                    }
        }
    }
    return l;
}


bool ArchiveReaderDTG::loadPKS2(QString fileName)
{

    FPKS2.setFileName(fileName);
    if (!FPKS2.open(QIODevice::ReadOnly)) return false;
    QFile _FPKS2;
    _FPKS2.setFileName(fileName);
    if (!_FPKS2.open(QIODevice::ReadOnly)) return false;
    if (!_FPKS2.isReadable()) return false;
    int cnt=_FPKS2.size()/szPKSrec;
    _FPKS2.seek(0);
    int procent=0;
    emit progress(0);
    TDatagramPacket2 D2;
    QDateTime T;
    pv.clear();
    pv.lIndex.reserve(cnt);
    for (int file_pos=0;file_pos<cnt;file_pos++){
        //_FPKS2.seek(i*szPKSrec);
        if (_FPKS2.read((char*)&D2,szPKSrec)==szPKSrec){
            TAChanelData *ad=chanel(D2.Dtgrm.Type,D2.Dtgrm.Name);
            if (ad==nullptr) continue;
            T=QDateTime::fromTime_t(D2.time);
            int ms=D2.msec %1000;
            T=T.addMSecs(ms);
            pv.add(T,ad,file_pos);
            int newprocent=100*file_pos/cnt;
            if (procent!=newprocent){
                procent=newprocent;
                emit progress(procent);
            }
        }

    }
    emit progress(-1);
    _FPKS2.close();
    return true;
}


bool ArchiveReaderDTG::setArchiveSrc(TArchiveSrc asrc)
{
    pv.clear();
    //if (asrc.reader!="PKS2") return false;
    if (FPKS2.isOpen()) FPKS2.close();
    return loadPKS2(asrc.params);
}

bool ArchiveReaderDTG::readDatagramPacket2(int npos, GtBuffer *B)
{

    if ((FPKS2.isOpen())&&(FPKS2.isReadable())){
        // файл уже открыт
        qint64 filepos=npos*szPKSrec;
        if (FPKS2.seek(filepos)){
            uint readsz=szPKSrec;
            TDatagramPacket2 D2;
            memset(D2.Dtgrm.Data,0,sizeof(D2.Dtgrm.Data));
            if (FPKS2.read((char*)&D2,readsz)==readsz){
                //B.type=D2.Dtgrm.Type;
                if (B->name!=D2.Dtgrm.Name){
                    throw "Что то не так отсортировали";
                };
                B->tick=D2.tick;
                B->timeDataRecived=QDateTime::fromTime_t(D2.time);
                int ms=D2.msec %1000;
                B->timeDataRecived=B->timeDataRecived.addMSecs(ms);
                B->timeDataChanged=B->timeDataRecived;
                B->A.resize(D2.Dtgrm.Size);
                memcpy(B->A.data(),&D2.Dtgrm.Data,D2.Dtgrm.Size);
                return true;
            }
        }
    }
    return false;
}


QDateTime ArchiveReaderDTG::beginTime()
{
    if (pv.lIndex.size()>0) return pv.lIndex.first();
    return QDateTime();
}

QDateTime ArchiveReaderDTG::endTime()
{
    if (pv.lIndex.size()>0) return pv.lIndex.last();
    return QDateTime();
}

int ArchiveReaderDTG::recordsCount()
{
    return pv.lIndex.size();
}

QList<GtBuffer *> ArchiveReaderDTG::gtBuffersRecord(int index_pos)
{
    QList<GtBuffer*> l;
    if (index_pos>=pv.lIndex.size()) return l;
    if (index_pos<0) return l;

    QMap<TAChanelData * ,int > kd=pv.srez_data(index_pos);

    foreach(TAChanelData *ad,kd.keys()){
         int ad_npos=kd[ad];
         if (ad->npos!=ad_npos){
             ad->npos=ad_npos;
             l.push_back(ad->B);
             if (ad->npos>=0){
                 readDatagramPacket2(ad->npos,ad->B);
                 ad->B->sost=GtBuffer::_alive;
             } else {
                 ad->B->clearData();
             }
         }
    }

    return l;
}



int ArchiveReaderDTG::time2record(QDateTime &t) const
{
    return pv.indexOf(t);
}

void ArchiveReaderDTG::clear()
{
    pv.clear();
    m_TypeToNameToChanel.clear();
    foreach (TAChanelData *ad, vAChanels) {
        delete ad;
    };
    vAChanels.clear();
}


QDateTime ArchiveReaderDTG::record2time(int npos) const
{
    if ((npos>=0)&&(npos<pv.lIndex.size())) return pv.lIndex[npos];
    return QDateTime();
}

