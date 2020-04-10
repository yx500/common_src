#ifndef SIGNALDESCRIPTIONTEXTINTERFACE_H
#define SIGNALDESCRIPTIONTEXTINTERFACE_H

/*
 * интерфейс для получения наименования сигнала
 * */

#include <QObject>
#include <QMap>
#include <QVector>
#include "signaldescription.h"


class SignalDescriptionTextInterface : public QObject
{
    Q_OBJECT
public:
    enum SignalTextRole {signalName=0,signalFullName=1,signalGroupName=2,signalFileName=3};
    explicit SignalDescriptionTextInterface(QObject *parent = nullptr);
    ~SignalDescriptionTextInterface();

    void addSearchDir(const QString &dir);

    bool addChanel(const int chanelType, const QString &chanelName, QString fileName="");

    QString getText(const int chanelType,const QString &chanelName,const int chanelOffset,int role=0);
    void setText(const int chanelType,const QString &chanelName,const int chanelOffset,int role,QString text);
    int getTextCount(const int chanelType,const QString &chanelName) const;
    QVector<SignalDescription> FindSignals(QString tsName) const;


signals:

public slots:
protected:
    QStringList lSearchDirs;
    struct SignalTextData{
        SignalTextData(){for (int j=0;j<3;j++) data[j]=QString();}
        QString data[3];
    };

    struct ChanelTextData{
        QString ChanelName;
        int ChanelType;
        QString fileName;
        QList<SignalTextData> lSignalText;
        bool loadTXT(const QString &fileName);
        bool loadCSV(const QString &fileName);
        bool saveCSV(const QString &fileName);

    };
    QList<ChanelTextData*> lTextData;


    QString findFile(const QString &filelName);
};
extern SignalDescriptionTextInterface SDTI;
#endif // SIGNALDESCRIPTIONTEXTINTERFACE_H
