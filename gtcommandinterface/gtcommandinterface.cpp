#include "gtcommandinterface.h"

GtCommandInterface::GtCommandInterface(QObject *parent, IGtBufferInterface *buffers) : QObject(parent)
{
    this->buffers=buffers;
    //генерим случайный ид срц
    FSRC_ID=QString::number(QDateTime::currentDateTime().msecsTo(QDateTime(QDate(2019,01,01),QTime(0,0))) % 1000);
    GtBuffer *B1=buffers->getGtBuffer(100,"gt_tu_0");
    GtBuffer *B2=buffers->getGtBuffer(102,"gt_rtu_0");
    connect(B1,&GtBuffer::bufferChanged,this,&GtCommandInterface::bufferChanged);
    connect(B2,&GtBuffer::bufferChanged,this,&GtCommandInterface::bufferChanged);
}
int GtCommandInterface::send_cmd(QString DEST, const QMap<QString, QString> &m)
{
    QMap<QString, QString> _m=m;
    _m["DEST"]=DEST;
    send_cmd(_m);
    return 0;
}

int GtCommandInterface::send_cmd(const QMap<QString, QString> &m)
{
    QMap<QString, QString> _m=m;
    _m["UID"]=newUID();
    _m["SRC"]=FSRC_ID;
    QStringList sl;
    sl.push_back(QString("%1=%2").arg("DEST").arg(_m["DEST"]));
    sl.push_back(QString("%1=%2").arg("SRC").arg(_m["SRC"]));
    sl.push_back(QString("%1=%2").arg("UID").arg(_m["UID"]));

    sl.push_back(QString("%1=%2").arg("UID").arg(newUID()));
    foreach (QString key, m.keys()) {
        if ((key=="DEST")||(key=="SRC")||(key=="UID")) continue;
        sl.push_back(QString("%1=%2").arg(key).arg(m[key]));
    }
    QString S=sl.join(";");

    _B0.clearData();
    _B0.setType(100);
    _B0.setName("gt_tu_0");
    _B0.A=S.toUtf8();
    if (_B0.A.size()>buffers->maxSizeData()) return -1;
    buffers->sendGtBuffer(&_B0);
    l_sended_tu.push_front(S);
    if (l_sended_tu.size()>100) l_sended_tu.pop_back();
    return 0;
}

int GtCommandInterface::accept_cmd(const QMap<QString, QString> &m, QString accept_str)
{
    QMap<QString, QString> _m=m;
    _m["ACCEPT_SRC"]=FSRC_ID;
    _m["ACCEPT_TXT"]=accept_str;
    QString S=map2str(_m);
    _B2.clearData();
    _B2.setType(100);
    _B2.setName("gt_rtu_0");
    _B2.A=S.toUtf8();
    if (_B2.A.size()>buffers->maxSizeData()) return -1;
    buffers->sendGtBuffer(&_B2);
    l_sended_rtu.push_front(S);
    if (l_sended_rtu.size()>100) l_sended_rtu.pop_back();
    return 0;

}

void GtCommandInterface::bufferChanged(GtBuffer *B)
{
    if (B->type==100){
        QString S=QString::fromUtf8(B->A);
        if (!echo)
        {
            if (l_sended_tu.indexOf(S)>=0) return;
        };
        QMap<QString,QString> m=str2map(S);
        emit recv_cmd(m);
    }
    if (B->type==102){
        QString S=QString::fromUtf8(B->A);
        if (!echo)
        {
            if (l_sended_rtu.indexOf(S)>=0) return;
        };
        QMap<QString,QString> m=str2map(S);
        emit recv_accept(m);
    }
}

QString GtCommandInterface::newUID()
{
    return QString::number(QDateTime::currentDateTime().msecsTo(QDateTime(QDate(2019,01,01),QTime(0,0))));

}

QMap<QString, QString> GtCommandInterface::str2map( QString S)
{
    //QString S2=S.replace("###",";");
    QStringList ls=S.split(";");
    QMap<QString,QString> m;
    foreach (auto &s, ls) {

        QStringList ls1=s.split("=");
        if (ls1.size()==2){
            m.insert(ls1[0],ls1[1]);
        }
    }
    return m;
}

QString GtCommandInterface::map2str(QMap<QString, QString> &m)
{
    QStringList sl;
    foreach (QString key, m.keys()) {
        sl.push_back(QString("%1=%2").arg(key).arg(m[key]));
    }
    return sl.join(";");
}

