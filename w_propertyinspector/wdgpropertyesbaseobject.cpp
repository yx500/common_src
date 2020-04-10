#include "wdgpropertyesbaseobject.h"
#include "ui_wdgpropertyesbaseobject.h"

#include "mvp_objectfactory.h"
#include "baseobjectcontroller.h"

wdgPropertyesBaseObject::wdgPropertyesBaseObject(QWidget *parent,MVP_ObjectFactory *MVP_OF,QObject *root4id) :
    QFrame(parent),
    ui(new Ui::wdgPropertyesBaseObject)
{
    ui->setupUi(this);
    this->MVP_OF=MVP_OF;
    theController = new BaseObjectController(this);
    QVBoxLayout *verticalLayout=new QVBoxLayout(ui->frameKontroller);
    verticalLayout->addWidget(theController);
    verticalLayout->setContentsMargins(1,1,1,1);
    ui->frameKontroller->setLayout(verticalLayout);
}

wdgPropertyesBaseObject::~wdgPropertyesBaseObject()
{
    delete ui;
}

void wdgPropertyesBaseObject::setObjects(QObjectList l)
{
    if (l!=theController->objects()){
        if (l.size()==1){
            QObject *object=l.first();
            if (MVP_OF!=0) ui->labelClassName->setText(MVP_OF->rusClassName(object)); else
                           ui->labelClassName->setText(object->metaObject()->className());
            ui->labelName->setText(object->objectName());
        } else
        if (l.size()>1){
            ui->labelClassName->setText(QString("%1 объектов").arg(l.size()));
            ui->labelName->setText("");
        } else
        {
            ui->labelClassName->clear();;
            ui->labelName->clear();
        }
        theController->setObjects(l);
    }
}

QObjectList wdgPropertyesBaseObject::objects() const
{
    return theController->objects();
}
