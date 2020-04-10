#include "w_signalsource.h"

w_SignalSource::w_SignalSource(QWidget *parent,SignalsManager *signalsManager) : QComboBox(parent)
{
    this->signalsManager=signalsManager;
    addItem("Сеть");
    addItem("Архив");
    addItem("Тест");
    setCurrentIndex(SM.signalSource());
    connect(this,SIGNAL(currentIndexChanged(int)),this,SLOT(cbSMSS_currentIndexChanged(int)));
    connect(signalsManager,&SignalsManager::signalSourceChanged,this,&w_SignalSource::signalSourceChanged);
}
void w_SignalSource::cbSMSS_currentIndexChanged(int index)
{
    signalsManager->setSignalSource(index);
}

void w_SignalSource::signalSourceChanged(int ss)
{
    blockSignals(true);
    setCurrentIndex(ss);
    blockSignals(false);

}
