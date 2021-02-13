#ifndef GTBUFFERS_H
#define GTBUFFERS_H

#include <QObject>
#include <QVector>
#include <QMap>
#include <QDateTime>
#include "gtbuffer.h"


extern QAtomicInteger<int> emit_counter;

class GtBuffers : public IGtBufferInterface
{
    Q_OBJECT
public:
    explicit GtBuffers(QObject *parent = nullptr):IGtBufferInterface(parent){
    }
    virtual ~GtBuffers() override {}


    GtBuffer *getGtBuffer(int type,const QString &name) override
    {

        if (type<0) return nullptr;
        if (m_TypeToNameToBD.contains(type))
            if (m_TypeToNameToBD[type].contains(name))
                return m_TypeToNameToBD[type][name];

        if (name.isEmpty()) return nullptr;
        GtBuffer *D=new GtBuffer(this);
        D->setName(name);
        D->setType(type);
        D->setSizeData(maxSizeData());
        vbd.push_back(D);
        m_TypeToNameToBD[type][name]=D;
        emit buffersListChanged(this);
        return D;
    }
    int sendGtBuffer(const GtBuffer *) override{return 0;}
    int maxSizeData() override {return 1024;}


    bool contains(int type,const QString &name) const
    {
        if (m_TypeToNameToBD.contains(type))
            if (m_TypeToNameToBD[type].contains(name))
                return true;
        return false;
    }

    GtBuffer *getBufferEx(int type,const QString &name)
    {
        if (m_TypeToNameToBD.contains(type))
            if (m_TypeToNameToBD[type].contains(name))
                return m_TypeToNameToBD[type][name];

        return nullptr;
    }


    virtual const QByteArray * getData(int type,const QString &name)
    {
        GtBuffer *D=getGtBuffer(type,name);
        return &D->A;
    }

    virtual int setData(int type,const QString &name,const QByteArray &A)
    {
        GtBuffer *D=getGtBuffer(type,name);
        D->A=A;
        D->timeDataChanged=QDateTime::currentDateTime();
        D->emitBufferChanged();
        return D->A.size();
    }

    QVector<GtBuffer *> allBuffers() const
    {
        return vbd;
    }

    QDateTime maxTimeDataChanged() const
    {
        QDateTime t;
        foreach (GtBuffer *B, vbd) {
            if (!B->dataRecivedTime().isValid()) continue;
            if ((!t.isValid()) || (t<B->dataRecivedTime())) t=B->dataRecivedTime();
        } return  t;
    }

signals:
    void buffersListChanged(GtBuffers *BB);

protected:
    QVector<GtBuffer *> vbd;
    QMap<int, QMap<QString, GtBuffer*> > m_TypeToNameToBD;
    QMap<GtBuffer*,QObject* > m_TypeToNameToO;
private slots:

};

#endif // GTBUFFERS_H
