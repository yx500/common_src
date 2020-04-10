#include "wdgpropertyinspector.h"
#include "ui_wdgpropertyinspector.h"

#include <QtWidgets/QVBoxLayout>
#include "ieditorinterface.h"
#include "v_model.h"
#include "m_base.h"

#include "baseobjectcontroller.h"
wdgPropertyInspector::wdgPropertyInspector(QWidget *parent, bool onlyState) :
    QDockWidget(parent),
    ui(new Ui::wdgPropertyInspector)
{
    ui->setupUi(this);

    wdgModelController=new wdgPropertyesBaseObject(0,IEditorInterface::instance()->ObjectsFactory(),IEditorInterface::instance()->rootQObject());
    connect(IEditorInterface::instance(),SIGNAL(propertyChanged(QObject*)) ,wdgModelController->theController,SLOT(propertyChanged(QObject*)) );
    connect(wdgModelController->theController,SIGNAL(propertyChange(QObject*)) ,IEditorInterface::instance(),SLOT(broadcast_PropertyChanged(QObject *) ));

    wdgVisualController =new wdgPropertyesBaseObject(0,IEditorInterface::instance()->ObjectsFactory(),IEditorInterface::instance()->rootQObject());
    connect(IEditorInterface::instance(),SIGNAL(propertyChanged(QObject*)) ,wdgVisualController->theController,SLOT(propertyChanged(QObject*)) );
    connect(wdgVisualController->theController,SIGNAL(propertyChange(QObject*)) ,IEditorInterface::instance(),SLOT(broadcast_PropertyChanged(QObject *) ));

    wdgModelController->setContentsMargins(1,1,1,1);
    wdgModelController->theController->setOnlyState(onlyState);
    wdgVisualController->setContentsMargins(1,1,1,1);
    wdgVisualController->theController->setOnlyState(onlyState);

    //QVBoxLayout *verticalLayout1=new QVBoxLayout(ui-> frameModel);
    //verticalLayout1->addWidget(wdgModelController);
    ui->frameModel->layout()->addWidget(wdgModelController);
    //QVBoxLayout *verticalLayout2=new QVBoxLayout(ui-> frameVisual);
    //verticalLayout2->addWidget(wdgVisualController);
    ui->frameVisual->layout()->addWidget(wdgVisualController);

       connect(IEditorInterface::instance(),SIGNAL(objectsListChanged(QObjectList)),this,SLOT(activeObjectsChanged(QObjectList)));
    connect(IEditorInterface::instance(),SIGNAL(activeObjectsChanged(QObjectList)),this,SLOT(activeObjectsChanged(QObjectList)));
    connect(IEditorInterface::instance(),SIGNAL(objectDeleting(QObject*)),this,SLOT(objectDeleting(QObject*)));
}

wdgPropertyInspector::~wdgPropertyInspector()
{
    delete ui;
}



void wdgPropertyInspector::setObjects(QObjectList l)
{

    if (l.size()==0){
        ui->frameModel->setVisible(false);
        ui->frameVisual->setVisible(false);
        wdgModelController->setObjects(l);
        wdgVisualController->setObjects(l);
    }else{
        QObjectList lm;
        QObjectList lv;
        foreach (QObject *O, l) {
            v_Base *V=qobject_cast<v_Base *>(O);
            BaseObject *M=qobject_cast<m_Base *>(O);
            if (V!=0){
                lv.push_back(V);
                v_Model *vm=qobject_cast<v_Model *>(V);
                if (vm) M=vm->modelObject();
                if (M!=0) lm.push_back(M);
            } else {
                lm.push_back(O);
            }
        }
        if (lm.size()>0){
            wdgModelController->setObjects(lm);
            if (wdgModelController->theController->topLevelPropertiesCount()>0)
                ui->frameModel->setVisible(true);

        }else{
            ui->frameModel->setVisible(false);
            wdgModelController->setObjects(lm);
        }
        if (lv.size()>0){
            wdgVisualController->setObjects(lv);
            if (wdgVisualController->theController->topLevelPropertiesCount()>0)
                ui->frameVisual->setVisible(true);
        } else {
            ui->frameVisual->setVisible(false);
            wdgVisualController->setObjects(lv);
        }
    }
}

void wdgPropertyInspector::activeObjectsChanged(QObjectList l)
{
    setObjects(l);
}

void wdgPropertyInspector::objectDeleting(QObject *O)
{
    if ((wdgModelController->objects().contains(O))||
            (wdgVisualController->objects().contains(O))) {
        setObjects(QObjectList());
    }
}
