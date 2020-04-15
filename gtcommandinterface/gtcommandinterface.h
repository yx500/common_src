#ifndef GTCOMMANDINTERFACE_H
#define GTCOMMANDINTERFACE_H

#include <QMap>
#include <QObject>
#include "gtbuffer.h"

class GtCommandInterface : public QObject
{
    Q_OBJECT
public:
    explicit GtCommandInterface(QObject *parent,IGtBufferInterface *buffers);
    void setSRC_ID(QString p){FSRC_ID=p;}
    QString SRC_ID() const {return FSRC_ID;}
    int send_cmd(QString DEST, const QMap<QString,QString> &m);
    int send_cmd(const QMap<QString,QString> &m);
    int accept_cmd(const QMap<QString,QString> &m,QString accept_str);
    bool echo=false;

signals:
    void recv_cmd(QMap<QString,QString> m);
    void recv_accept(QMap<QString,QString> m);
public slots:
    void bufferChanged(GtBuffer *B);
protected:
    IGtBufferInterface *buffers;
    QString FSRC_ID="";
    QString newUID();
    QList<QString> l_sended_tu;
    GtBuffer _B0;
    GtBuffer _B2;
    QList<QString> l_sended_rtu;
    QMap<QString,QString> str2map(QString &S);
    QString map2str(QMap<QString,QString> &m);
};

#endif // GTCOMMANDINTERFACE_H
