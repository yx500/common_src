#include "archivereaderpks.h"
#include <qdir.h>
#include "dmpacket2.h"

enum {szPKSrec=512};




ArchiveReaderPKS::ArchiveReaderPKS(QObject *parent) : IArchiveReader(parent)
{

}

void ArchiveReaderPKS::setGtBuffers(GtBuffers *gtBuffers)
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

TAChanelData * ArchiveReaderPKS::chanel(quint16 type, const QString &name)
{
    if (m_TypeToNameToChanel.contains(type))
        if (m_TypeToNameToChanel[type].contains(name))
            return m_TypeToNameToChanel[type][name];

    return nullptr;
}



QList<IArchiveReader::TArchiveSrc> ArchiveReaderPKS::findArchiveSrc(QString path)
{
    QList<TArchiveSrc> l;
    QDir dir(path);
    dir.setFilter(QDir::Files);
    dir.setSorting(QDir::Size | QDir::Reversed);

    QFileInfoList list = dir.entryInfoList();
    for (int i = 0; i < list.size(); ++i) {
        QFileInfo fileInfo = list.at(i);
        QString fileName=fileInfo.fileName();
        QString fileExt=fileInfo.suffix();
        if ((fileExt=="pks2")/*&&(fileName.length()==10)*/){
            //2013-09-18.pks2
            bool ok1,ok2;
            int day=fileName.mid(8,2).toInt(&ok1);
            int month=fileName.mid(5,2).toInt(&ok2);
            //int year=fileName.mid(0,4).toInt(&ok3);
            if ((ok1)&&(ok2)&&(day<=31)&&(month<=12)){
                // пробуем прочитать
                QFile _FPKS2;
                _FPKS2.setFileName(fileInfo.absoluteFilePath());
                if (!_FPKS2.open(QIODevice::ReadOnly)) continue;
                if (!_FPKS2.isReadable()) continue;
                _FPKS2.seek(0);
                TDatagramPacket2 D2;
                if (_FPKS2.read((char*)&D2,szPKSrec)==szPKSrec){
                    QDateTime T1=QDateTime::fromTime_t(D2.time);
                    _FPKS2.seek(_FPKS2.size()-szPKSrec);
                    if (_FPKS2.read((char*)&D2,szPKSrec)==szPKSrec){
                        QDateTime T2=QDateTime::fromTime_t(D2.time);
                        TArchiveSrc asrc;
                        asrc.reader=this;
                        asrc.T1=T1;
                        asrc.T2=T2;
                        asrc.params=fileInfo.absoluteFilePath();
                        l.push_back(asrc);
                    }
                }
            }
        }
    }
    return l;
}


bool ArchiveReaderPKS::loadPKS2(QString fileName)
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


bool ArchiveReaderPKS::setArchiveSrc(TArchiveSrc asrc)
{
    pv.clear();
    //if (asrc.reader!="PKS2") return false;
    if (FPKS2.isOpen()) FPKS2.close();
    return loadPKS2(asrc.params);
}

bool ArchiveReaderPKS::readDatagramPacket2(int npos, GtBuffer *B)
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


QDateTime ArchiveReaderPKS::beginTime()
{
    if (pv.lIndex.size()>0) return pv.lIndex.first();
    return QDateTime();
}

QDateTime ArchiveReaderPKS::endTime()
{
    if (pv.lIndex.size()>0) return pv.lIndex.last();
    return QDateTime();
}

int ArchiveReaderPKS::recordsCount()
{
    return pv.lIndex.size();
}

QList<GtBuffer *> ArchiveReaderPKS::gtBuffersRecord(int index_pos)
{
    QList<GtBuffer*> l;
    if (index_pos>=pv.lIndex.size()) return l;
    if (index_pos<0) return l;

//    QMap<TAChanelData * ,int > kd=pv.srez_data(index_pos);

//    foreach(TAChanelData *ad,kd.keys()){
//         int ad_npos=kd[ad];
//         if (ad->npos!=ad_npos){
//             ad->npos=ad_npos;
//             l.push_back(ad->B);
//             if (ad->npos>=0){
//                 readDatagramPacket2(ad->npos,ad->B);
//                 ad->B->sost=GtBuffer::_alive;
//             } else {
//                 ad->B->clearData();
//             }
//         }
//    }
    auto rl=pv.srez_data_l(index_pos);

    foreach(auto &sr,rl){
         if (sr.k->npos!=sr.data){
             sr.k->npos=sr.data;
             l.push_back(sr.k->B);
             if (sr.k->npos>=0){
                 readDatagramPacket2(sr.k->npos,sr.k->B);
                 sr.k->B->sost=GtBuffer::_alive;
             } else {
                 sr.k->B->clearData();
             }
         }
    }


    return l;
}

//int ArchiveReaderPKS::q2findposIndex(QDateTime &t) const
//{
//    if (lIndex.isEmpty()) return -1;
//    if (t<=lIndex.first()) return 0;
//    if (t>=lIndex.last()) return lIndex.size()-1;


//    int found_npos = -1;
//    int l = 0; // левая граница
//    int r = lIndex.size()-1; // правая граница
//    int mid;
//    while ((l <= r)) {
//        mid = (l + r) / 2; // считываем срединный индекс отрезка [l,r]

//        if (lIndex[mid] == t) {
//            found_npos=mid;
//            while ((lIndex[found_npos] == t) && (found_npos<lIndex.size())) {
//                found_npos++;
//            }
//            break;
//        } //проверяем ключ со серединным элементом
//        if ((mid>0) && (lIndex[mid-1]<t) && (lIndex[mid]>t)) {found_npos=mid-1;break;}
//        if (lIndex[mid] > t) r = mid - 1; // проверяем, какую часть нужно отбросить
//        else l = mid + 1;
//    }
//    return found_npos;
//}

int ArchiveReaderPKS::time2record(QDateTime &t) const
{
    return pv.indexOf(t);
}

void ArchiveReaderPKS::clear()
{
    pv.clear();
    m_TypeToNameToChanel.clear();
    foreach (TAChanelData *ad, vAChanels) {
        delete ad;
    };
    vAChanels.clear();
}


QDateTime ArchiveReaderPKS::record2time(int npos) const
{
    if ((npos>=0)&&(npos<pv.lIndex.size())) return pv.lIndex[npos];
    return QDateTime();
}

