#include "signaldescriptiontextinterface.h"
#include <QDir>
#include <QTextStream>
#include <QFile>

SignalDescriptionTextInterface SDTI;
SignalDescriptionTextInterface::SignalDescriptionTextInterface(QObject *parent) : QObject(parent)
{

}

SignalDescriptionTextInterface::~SignalDescriptionTextInterface()
{
    foreach (ChanelTextData *textData, lTextData) {
        delete  textData;
    }
    lTextData.clear();

}

void SignalDescriptionTextInterface::addSearchDir(const QString &dir)
{
    if (lSearchDirs.indexOf(dir)<0)  lSearchDirs.push_back(dir);
}

SignalDescriptionTextInterface::ChanelTextData* SignalDescriptionTextInterface::addChanel(const int chanelType, const QString &chanelName, QString fileName)
{
    foreach (SignalDescriptionTextInterface::ChanelTextData*d, lTextData) {
        if ((d->ChanelType==chanelType)&&(d->ChanelName==chanelName)) return d;
    }
    SignalDescriptionTextInterface::ChanelTextData *d=new SignalDescriptionTextInterface::ChanelTextData;
    d->ChanelType=chanelType;
    d->ChanelName=chanelName;
    lTextData.push_back(d);

    if (fileName.isEmpty()){
        if (chanelType==1){
            fileName=findFile(chanelName);
        } else {
            fileName=findFile(QString("%1_%2").arg(chanelType).arg(chanelName));
        }
    }

    if (!fileName.isEmpty()){
        QFileInfo fi=QFileInfo(fileName);
        if (fi.exists()){
            if ((fi.suffix()=="CSV")||(fi.suffix()=="csv")) d->loadCSV(fileName);else
                if ((fi.suffix()=="TXT")||(fi.suffix()=="txt")) d->loadTXT(fileName);

        }
    }
    return d;
}

bool SignalDescriptionTextInterface::saveChanel(const int chanelType, const QString &chanelName, QString fileName)
{
    foreach (SignalDescriptionTextInterface::ChanelTextData*d, lTextData) {
        if ((d->ChanelType==chanelType)&&(d->ChanelName==chanelName)) {
            return d->saveCSV(fileName);
        }
    }
    return false;
}



QString SignalDescriptionTextInterface::findFile(const QString &filelName)
{
    QString txtFN=QString("%1.TXT").arg(filelName.toUpper());
    QString csvFN=QString("%1.CSV").arg(filelName.toUpper());
    QDir qdir;
    QStringList filters;
    filters << "*.txt" << "*.csv" <<  "*.TXT" << "*.CSV";
    qdir.setNameFilters(filters);
    foreach (QString dirname, lSearchDirs) {
        qdir.setPath(dirname);
        if (qdir.exists()){
            QFileInfoList lfi=qdir.entryInfoList(filters,QDir::Files);
            foreach (QFileInfo fi, lfi) {
                QString FN=fi.fileName().toUpper();
                if (FN==txtFN) {return fi.absoluteFilePath();}
                if (FN==csvFN) {return fi.absoluteFilePath();}
            }
        }
    }
    return "";
}



QString SignalDescriptionTextInterface::getText(const int chanelType, const QString &chanelName, const int chanelOffset, int role)
{
    foreach (ChanelTextData *chanelTextData, lTextData) {
        if ((chanelTextData->ChanelType==chanelType) && (chanelTextData->ChanelName==chanelName)){
            if ((chanelOffset<chanelTextData->lSignalText.size())){
                const SignalTextData &sd=chanelTextData->lSignalText.at(chanelOffset);

                switch (role){
                case signalName: return sd.data[signalName];
                case signalFullName: return sd.data[signalFullName];
                case signalGroupName: return sd.data[signalGroupName];
                case signalFileName: return chanelTextData->fileName;
                }
            }
        }
    }
    return "";
}

void SignalDescriptionTextInterface::setText(const int chanelType, const QString &chanelName, const int chanelOffset, int role, QString text)
{
    foreach (ChanelTextData *chanelTextData, lTextData) {
        if ((chanelTextData->ChanelType==chanelType) && (chanelTextData->ChanelName==chanelName)){
            chanelTextData->setText(chanelOffset, role, text);
        }
    }
}

void SignalDescriptionTextInterface::ChanelTextData::setText(const int chanelOffset, int role, QString text)
{
    if ((chanelOffset>=lSignalText.size())){
        int cntadd=chanelOffset-lSignalText.size()+1;
        for (int in=0;in<cntadd;in++) lSignalText.push_back(SignalTextData());
    }
    SignalTextData &sd=lSignalText[chanelOffset];
    sd.data[role]=text;
}


int SignalDescriptionTextInterface::getTextCount(const int chanelType, const QString &chanelName) const
{
    foreach (ChanelTextData *chanelTextData, lTextData) {
        if ((chanelTextData->ChanelType==chanelType) && (chanelTextData->ChanelName==chanelName))
            return chanelTextData->lSignalText.size();
    }
    return 0;
}


bool SignalDescriptionTextInterface::ChanelTextData::loadTXT(const QString &fileName)
{
    QStringList ltxt;
    QFile txtfile(fileName);
    if (txtfile.open(QFile::ReadOnly | QFile::Truncate)) {

        QTextStream textStream(&txtfile);
        textStream.setCodec("Windows-1251");
        while (true)
        {
            QString line = textStream.readLine();
            if (line.isNull())
                break;
            else
                ltxt.append(line);
        }
        txtfile.close();
    }
    // с 3ий!
    int Ftxt_signals_in_group=ltxt.size()-3;
    if (Ftxt_signals_in_group<=0) return false;

    this->fileName=fileName;
    lSignalText.clear();
    SignalTextData sd;
    QStringList line1;
    line1=ltxt[3].split("\t");
    int groupCount=line1.size();
    for (int g=0;g<groupCount;g++){
        for (int i=0;i<Ftxt_signals_in_group;i++){
            for (int j=0;j<3;j++) sd.data[j]=QString();
            lSignalText.push_back(sd);
        }
    }
    for (int i=0;i<Ftxt_signals_in_group;i++){
        line1=ltxt[i+3].split("\t");
        for (int g=0;g<line1.size();g++)
            lSignalText[g*Ftxt_signals_in_group+i].data[0]=line1[g];

    }
    return true;
}
bool SignalDescriptionTextInterface::ChanelTextData::loadCSV(const QString &fileName)
{
    QStringList ltxt;
    QFile txtfile(fileName);
    if (txtfile.open(QFile::ReadOnly | QFile::Truncate)) {
        QTextStream textStream(&txtfile);
        textStream.setCodec("Windows-1251");
        while (true)
        {
            QString line = textStream.readLine();
            if (line.isNull())
                break;
            else
                ltxt.append(line);
        }
        txtfile.close();
    }
    // с 1ий!
    QStringList line1;

    this->fileName=fileName;
    lSignalText.clear();
    SignalTextData sd;
    SignalTextData sd0;

    for(int i=1;i<ltxt.count();i++){
        line1=ltxt[i].split(",");
        if (line1.count()<3) continue;
        QString sn=line1[0];
        int nn=sn.toInt();
        if ((nn>0) &&(nn<1000)) {
            if (nn>=lSignalText.size()){
                int cntadd=nn-lSignalText.size();
                for (int in=0;in<cntadd;in++) lSignalText.push_back(sd0);
            }
            for (int j=0;j<3;j++) sd.data[j]=QString();
            sd.data[signalName]=line1[3];
            sd.data[signalFullName]=line1[2];
            lSignalText[nn-1]=sd;
        }
    }
    return true;
}

bool SignalDescriptionTextInterface::ChanelTextData::saveCSV(QString fileName)
{
    if (fileName==""){
        fileName=QString("%1_%2.csv").arg(ChanelType).arg(ChanelName);
    }
    QStringList sl;
    sl.push_back("п/п,тип,обозначение,имя");
    for (int i=0;i<lSignalText.size() ;i++){
        sl.push_back(QString("%1,,%2,%3").arg(i+1).arg(lSignalText[i].data[signalFullName]).arg(lSignalText[i].data[signalName]));
    }
    // write data
    QFile fOut(fileName);
    if (fOut.open(QFile::WriteOnly | QFile::Text)) {
        QTextStream s(&fOut);
        for (int i = 0; i < sl.size(); ++i)
            s << sl.at(i) << '\n';
        fOut.close();
        return true;
    }
    return false;

}


QVector<SignalDescription> SignalDescriptionTextInterface::FindSignals(QString tsName) const
{
    QVector<SignalDescription> vf;
    foreach (ChanelTextData *chanelTextData, lTextData) {
        for (int i=0;i<chanelTextData->lSignalText.size() ;i++){
            if (chanelTextData->lSignalText[i].data[signalName]==tsName){
                SignalDescription s;
                s.setChanelName(chanelTextData->ChanelName);
                s.setChanelType(chanelTextData->ChanelType);
                s.setChanelOffset(i);
                s.setStoredSignalName(tsName);
                vf.push_back(s);
            }
        }
    }
    return vf;
}
