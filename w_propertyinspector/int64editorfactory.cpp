#include "int64editorfactory.h"


#include "int64propertymanager.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"
#include <QVariant>
#include <QIcon>
#include <QDate>
#include <QLineEdit>



Int64EditorFactory::Int64EditorFactory(QObject *parent) :
    QtVariantEditorFactory(parent)
{
    editor=0;
    editorProperty=0;
    editorManager=0;
}

Int64EditorFactory::~Int64EditorFactory()
{
    editorProperty=0;
}
void Int64EditorFactory::connectPropertyManager(QtVariantPropertyManager *manager)
{
    editorManager=qobject_cast<Int64PropertyManager*>(manager);
    connect(manager, SIGNAL(valueChanged(QtProperty *, QVariant)),
                this, SLOT(slotPropertyChanged(QtProperty *, QVariant)));
}

QWidget *Int64EditorFactory::createEditor(QtVariantPropertyManager *manager, QtProperty *property, QWidget *parent)
{
    editorProperty=property;
    editorManager=qobject_cast<Int64PropertyManager*>(manager);
    editor = new QLineEdit(parent);
    quint64 p = manager->value(property).toULongLong();
    editor->setText(QString("%1").arg(p));
    editor->setReadOnly(true);

    //connect(editor, SIGNAL(valueChanged(double)), this, SLOT(slotSetValue(double)));
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

void Int64EditorFactory::disconnectPropertyManager(QtVariantPropertyManager *manager)
{
    editorManager=0;
    editorProperty=0;
    disconnect(manager, SIGNAL(valueChanged(QtProperty *, double)),
                this, SLOT(slotPropertyChanged(QtProperty *, double)));

}

void Int64EditorFactory::slotPropertyChanged(QtProperty *property, QVariant value)
{
    if (!editor) return;
//    if (editor->text() != value.toString()) {
//            editor->blockSignals(true);
//            ObjectId p = value.value<ObjectId>();
//            editor->setValue(p.num());
//            editor->blockSignals(false);
//        }
}

void Int64EditorFactory::slotSetValue(double value)
{
    //QObject *object = sender();
    if (!editorManager) return;
    if (!editorProperty) return;
    quint64 p;
    p=value;
    QVariant v;
    v.setValue(p);
    editorManager->setValue(editorProperty, v);
}

void Int64EditorFactory::slotEditorDestroyed(QObject *ed)
{
    if ((ed==editor)&&(editor!=0)){
        //disconnect(editor, SIGNAL(valueChanged(double)), this, SLOT(slotSetValue(double)));
        disconnect(editor, SIGNAL(destroyed(QObject *)),
                    this, SLOT(slotEditorDestroyed(QObject *)));
        editor=0;
    }
}
