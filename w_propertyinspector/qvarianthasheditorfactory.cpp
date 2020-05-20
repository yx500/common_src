#include "qvarianthasheditorfactory.h"

#include "qtvarianthasheditwidget.h"

#include "mvp_system.h"






QVariantHashEditorFactory::QVariantHashEditorFactory(QObject *parent):QtVariantEditorFactory(parent)
{
    editor=0;
    editorProperty=0;
    editorManager=0;
}

QVariantHashEditorFactory::~QVariantHashEditorFactory()
{
    editorProperty=0;
}

void QVariantHashEditorFactory::connectPropertyManager(QtVariantPropertyManager *manager)
{
    editorManager=qobject_cast<QtVariantPropertyManager*>(manager);
    connect(manager, SIGNAL(valueChanged(QtProperty *, QVariant)),
            this, SLOT(slotPropertyChanged(QtProperty *, QVariant)));
}


QWidget *QVariantHashEditorFactory::createEditor(QtVariantPropertyManager *manager, QtProperty *property, QWidget *parent)
{
    editorProperty=property;
    editorManager=qobject_cast<QtVariantPropertyManager*>(manager);
//    editor = new QLineEdit(parent);
    //ObjectLink p = manager->value(property).value<ObjectLink>();
     editor = new QtVariantHashEditWidget(parent);
    QVariantHash h=manager->value(property).toHash();
    editor->setValue(MVP_System::QVariantHashToQString(h));

    connect(editor, &QtVariantHashEditWidget::valueChanged, this, &QVariantHashEditorFactory::slotSetValue);
    connect(editor, SIGNAL(destroyed(QObject *)),
                this, SLOT(slotEditorDestroyed(QObject *)));
    return editor;
}

void QVariantHashEditorFactory::disconnectPropertyManager(QtVariantPropertyManager *manager)
{
    editorManager=nullptr;
    editorProperty=nullptr;

    disconnect(manager, SIGNAL(valueChanged(QtProperty *, double)),
               this, SLOT(slotPropertyChanged(QtProperty *, double)));
}

void QVariantHashEditorFactory::slotPropertyChanged(QtProperty *property, QVariant value)
{
    Q_UNUSED(property)
    if (!editor) return;
    if (editor->m_text != value.toString()) {
            editor->blockSignals(true);
            QVariantHash h=value.toHash();
            editor->setValue( MVP_System::QVariantHashToQString(h));
            editor->blockSignals(false);
    }
}

void QVariantHashEditorFactory::slotSetValue(QString value)
{
    if (!editorManager) return;
    if (!editorProperty) return;
    QVariantHash p=MVP_System::QStringToQVariantHash(value);

    QVariant v=p;
        editorManager->setValue(editorProperty, v);
}

void QVariantHashEditorFactory::slotEditorDestroyed(QObject *ed)
{
    if ((ed==editor)&&(editor!=0)){
        //disconnect(editor, SIGNAL(valueChanged(double)), this, SLOT(slotSetValue(double)));
        disconnect(editor, SIGNAL(destroyed(QObject *)),
                    this, SLOT(slotEditorDestroyed(QObject *)));
        editor=0;
    }
}
