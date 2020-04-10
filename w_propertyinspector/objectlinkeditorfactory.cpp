#include "objectlinkeditorfactory.h"

#include "objectlinkpropertymanager.h"
#include "objectlink.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include <QVariant>
#include <QIcon>
#include <QDate>
#include <QDebug>
#include <QLineEdit>
#include "baseobjecttools.h"
#include "baseobject.h"


ObjectLinkEditorFactory::ObjectLinkEditorFactory(QObject *parent) :
    QtVariantEditorFactory(parent)
{
    editor=0;
    editorProperty=0;
    editorManager=0;
}
ObjectLinkEditorFactory::~ObjectLinkEditorFactory()
{
    editorProperty=0;
}

void ObjectLinkEditorFactory::connectPropertyManager(QtVariantPropertyManager *manager)
{
    editorManager=qobject_cast<ObjectLinkPropertyManager*>(manager);
    connect(manager, SIGNAL(valueChanged(QtProperty *, QVariant)),
            this, SLOT(slotPropertyChanged(QtProperty *, QVariant)));
}

QWidget *ObjectLinkEditorFactory::createEditor(QtVariantPropertyManager *manager, QtProperty *property, QWidget *parent)
{
    editorProperty=property;
    editorManager=qobject_cast<ObjectLinkPropertyManager*>(manager);
    editor = new QLineEdit(parent);
    ObjectLink p = manager->value(property).value<ObjectLink>();
    editor->setText(p.toString());
    editor->setReadOnly(false);

    connect(editor, SIGNAL(textEdited(QString)), this, SLOT(slotSetValue(QString)));
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

void ObjectLinkEditorFactory::disconnectPropertyManager(QtVariantPropertyManager *manager)
{
    editorManager=nullptr;
    editorProperty=nullptr;
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, double)),
               this, SLOT(slotPropertyChanged(QtProperty *, double)));
}

void ObjectLinkEditorFactory::slotPropertyChanged(QtProperty *property, QVariant value)
{
    Q_UNUSED(property)
    if (!editor) return;
    if (editor->text() != value.toString()) {
            editor->blockSignals(true);
            ObjectLink p = value.value<ObjectLink>();
            editor->setText(p.toString());
            editor->blockSignals(false);
    }
}

void ObjectLinkEditorFactory::slotSetValue(QString value)
{
    //QObject *object = sender();
    if (!editorManager) return;
    if (!editorProperty) return;

    ObjectLink p;
    p.clear();
    if (value.isEmpty()){
        p.setInNotUse(true);
    } else {
        quint64 id=str2objectId(value);
//        BaseObject *B=superFindObjectById(root4id,id);
//        if (B) p.linkObj(B); else
            p.clear();
            p.storedFid=id;
    }
    QVariant v;
    v.setValue(p);
    editorManager->setValue(editorProperty, v);
}

void ObjectLinkEditorFactory::slotEditorDestroyed(QObject *ed)
{
    if ((ed==editor)&&(editor!=0)){
        //disconnect(editor, SIGNAL(valueChanged(double)), this, SLOT(slotSetValue(double)));
        disconnect(editor, SIGNAL(destroyed(QObject *)),
                    this, SLOT(slotEditorDestroyed(QObject *)));
        editor=0;
    }
}

