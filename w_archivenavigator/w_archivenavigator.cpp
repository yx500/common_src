#include "w_archivenavigator.h"
#include "ui_w_archivenavigator.h"

#include <QFileDialog>
#include <QMenu>

#include <QSettings>
#include <QTimer>

#include "archivereaderpks.h"
#include "archivereaderdtg.h"

w_ArchiveNavigator::w_ArchiveNavigator(QWidget *parent,ArchiveManager *archiveManager) :
    QWidget(parent),
    ui(new Ui::w_ArchiveNavigator)
{
    ui->setupUi(this);
    this->archiveManager=archiveManager;
    Fstep=0;
    ui->progressBar->setVisible(false);
    //adjustSize();
    ui->labelChanelChanged->setText("");
    ArchiveReaderPKS *archiveReaderPKS=new ArchiveReaderPKS(archiveManager);
    archiveManager->addArchiveReader(archiveReaderPKS);
    ArchiveReaderDTG *archiveReaderDTG=new ArchiveReaderDTG(archiveManager);
    archiveManager->addArchiveReader(archiveReaderDTG);

    connect(archiveManager,SIGNAL(buffersChanged(QList<GtBuffer*>)),this,SLOT(buffersChanged(QList<GtBuffer*>)));
    connect(archiveManager,SIGNAL(positionChaged(int)),this,SLOT(on_positionChaged(int)));
    connect(archiveManager,SIGNAL(progress(int)),this,SLOT(on_progress(int)));

    timerGo=new QTimer();
    connect(timerGo,&QTimer::timeout,this,&w_ArchiveNavigator::slotGoTimer);

    if (!QCoreApplication::instance()->organizationName().isEmpty()){
        QSettings settings;
        QString ArchiveDirectory=settings.value("ArchiveDirectory").toString();
        loadfound(ArchiveDirectory);
    }
}

w_ArchiveNavigator::~w_ArchiveNavigator()
{
    delete ui;
}

void w_ArchiveNavigator::on_btMenu_clicked()
{
    QMenu *popupMenu=new QMenu(this);
    QAction *Act1 = new QAction(tr("Каталог.."), this);
    Act1->setStatusTip(tr("Указать месторасположение каталога архива"));
    popupMenu->addAction(Act1);
    connect(Act1, SIGNAL(triggered()), this, SLOT(on_directory()));
    popupMenu->popup(ui->btMenu->mapToGlobal(QPoint(ui->btMenu->width(),0)));

    QMenu *subMenu1=new QMenu(popupMenu);
    subMenu1->setTitle("Шаг прокрутки");
    QAction *acts1 = new QAction(tr("Покадровый"), subMenu1);subMenu1->addAction(acts1);
    acts1->setStatusTip(tr("Последовательно каждую запись"));
    QObject::connect(acts1, &QAction::triggered, [=] () {setStep(0);});
    QAction *acts2 = new QAction(tr("1 сек"), subMenu1);subMenu1->addAction(acts2);
    QObject::connect(acts2, &QAction::triggered, [=] () {setStep(1);});
    QAction *acts3 = new QAction(tr("15 сек"), subMenu1);subMenu1->addAction(acts3);
    QObject::connect(acts3, &QAction::triggered, [=] () {setStep(15);});
    QAction *acts4 = new QAction(tr("60  сек"), subMenu1);subMenu1->addAction(acts4);
    QObject::connect(acts4, &QAction::triggered, [=] () {setStep(60);});
    popupMenu->addMenu(subMenu1);

    foreach (QMenu *extMenu, extMenus) {
        popupMenu->addMenu(extMenu);
    }

}

void w_ArchiveNavigator::loadfound(QString ArchiveDirectory)
{
    ui->cbDays->clear();
    lfound.clear();
    if (!ArchiveDirectory.endsWith("/"))
        ArchiveDirectory=ArchiveDirectory+"/";
    lfound=archiveManager->findArchiveSrc(ArchiveDirectory);
    for (int i=0;i<lfound.size();i++)
        ui->cbDays->addItem(lfound[i].T1.toString("MMM dd"));
//    if (lfound.size()==1)
    //        on_cbDays_activated(0);
}

void w_ArchiveNavigator::setStep(int step)
{
    Fstep=step;
}


void w_ArchiveNavigator::on_directory()
{
    QFileDialog dialog(this);
    dialog.setFileMode(QFileDialog::Directory);
    QString ArchiveDirectory;
    if (!QCoreApplication::instance()->organizationName().isEmpty()){
        QSettings settings;
        ArchiveDirectory=settings.value("ArchiveDirectory").toString();
        dialog.setDirectory(ArchiveDirectory);
    }
    if (dialog.exec()){
        QString ArchiveDirectory=dialog.selectedFiles().at(0);
        if (!QCoreApplication::instance()->organizationName().isEmpty()){
            QSettings settings;
            settings.setValue("ArchiveDirectory", ArchiveDirectory);
        }
        loadfound(ArchiveDirectory);
    }
}

void w_ArchiveNavigator::on_progress(int procent)
{
    if (procent<0){
        ui->progressBar->setVisible(false);
        adjustSize();
    } else {
        ui->progressBar->setVisible(true);
        ui->progressBar->setValue(procent);
    }
}

void w_ArchiveNavigator::buffersChanged(QList<GtBuffer *> l)
{

    ui->labelCurrentTime->setText(archiveManager->getBuffers()->maxTimeDataChanged().toString("hh:mm::ss.zzz"));
    if (l.isEmpty())    {
        ui->labelChanelChanged->setText("");

    } else
    if (l.size()==1){
        GtBuffer *B=l.first();
        QString type=QString("%1").arg(B->type);
        QString name=B->name;
        ui->labelChanelChanged->setText(QString("%1 %2").arg(type,name));
    } else {
        ui->labelChanelChanged->setText(QString("%1 прочитано..").arg(l.size()));
        QString S;
        foreach (GtBuffer *B, l) {
            S=S+QString("%1 %2 \r").arg(B->getType()).arg(B->getName());
        }
        setToolTip(S);
    }
}

void w_ArchiveNavigator::slotGoTimer()
{
    if (!archiveManager->go(1)){
        timerGo->stop();
        ui->btGo->setChecked(false);
    }
}

void w_ArchiveNavigator::on_positionChaged(int newpos)
{
    ui->horizontalSlider->blockSignals(true);
    ui->horizontalSlider->setMaximum(archiveManager->recordsCount());
    ui->horizontalSlider->setValue(newpos);
    ui->horizontalSlider->blockSignals(false);

}

void w_ArchiveNavigator::on_btPrev_clicked()
{
    if (Fstep==0) archiveManager->go(-1); else archiveManager->go_sec(-Fstep);
}

void w_ArchiveNavigator::on_btNext_clicked()
{
    if (Fstep==0) archiveManager->go(1); else archiveManager->go_sec(Fstep);
}

void w_ArchiveNavigator::on_btEnd_clicked()
{
    archiveManager->go(11);
}

void w_ArchiveNavigator::on_btBegin_clicked()
{
    archiveManager->go(-11);
}

void w_ArchiveNavigator::on_cbDays_activated(int index)
{
    if (index>=0){
        archiveManager->setArchiveSrc(lfound[index]);
        archiveManager->go(-11);
        ui->btBegin->setToolTip(archiveManager->beginTime().toString("hh:mm:ss"));
        ui->btEnd->setToolTip(archiveManager->endTime().toString("hh:mm:ss"));
    }

}

void w_ArchiveNavigator::on_horizontalSlider_valueChanged(int value)
{
    archiveManager->setRecord(value);
}


void w_ArchiveNavigator::on_btGo_clicked()
{
    if (ui->btGo->isChecked()){
        timerGo->start(1);
    } else {
        timerGo->stop();
    }

}

void w_ArchiveNavigator::stopGo()
{
    ui->btGo->setChecked(false);
    timerGo->stop();
}

void w_ArchiveNavigator::recivedBroadcastCommand(QObject *object, QVariant cmd)
{
    if (cmd.toInt()==101) stopGo();
}
