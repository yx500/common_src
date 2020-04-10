#ifndef W_SIGNALSOURCE_H
#define W_SIGNALSOURCE_H

#include <QComboBox>
#include "signalsmanager.h"

class w_SignalSource : public QComboBox
{
    Q_OBJECT
public:
    explicit w_SignalSource(QWidget *parent,SignalsManager *signalsManager);

signals:

public slots:
    void cbSMSS_currentIndexChanged(int index);
    void signalSourceChanged(int ss);

protected:
    SignalsManager *signalsManager;
};

#endif // W_SIGNALSOURCE_H
