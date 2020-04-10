#ifndef W_ARCHIVENAVIGATOR_H
#define W_ARCHIVENAVIGATOR_H

#include <QWidget>
#include <QVariant>

#include "archivemanager.h"


namespace Ui {
class w_ArchiveNavigator;
}
class QMenu;
class ArchiveManager;

class w_ArchiveNavigator : public QWidget
{
    Q_OBJECT

public:
    explicit w_ArchiveNavigator(QWidget *parent,ArchiveManager *archiveManager);
    ~w_ArchiveNavigator();
    ArchiveManager *archiveManager;
    void addMenu(QMenu *m){extMenus.push_back(m);}

private slots:
    void on_btMenu_clicked();
    void on_directory();
    void on_progress(int procent);
    void on_positionChaged(int newpos);
    void on_btPrev_clicked();

    void on_btNext_clicked();

    void on_btEnd_clicked();

    void on_btBegin_clicked();

    void on_cbDays_activated(int index);

    void on_horizontalSlider_valueChanged(int value);

    void buffersChanged(QList<GtBuffer*> l);

    void slotGoTimer();

    void on_btGo_clicked();

    void stopGo();
signals:
    void sendBroadcastCommand(QObject * object,QVariant cmd);
public slots:
    void recivedBroadcastCommand(QObject * object,QVariant cmd);
protected:
    QList<ArchiveManager::TArchiveSrc> lfound;
    int Fstep;
    void loadfound(QString ArchiveDirectory);
    void setStep(int step);
    QTimer *timerGo;
    QList<QMenu*> extMenus;


private:
    Ui::w_ArchiveNavigator *ui;
};

#endif // W_ARCHIVENAVIGATOR_H
