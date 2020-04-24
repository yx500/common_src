#ifndef QVARIANTHASHEDITORFACTORY_H
#define QVARIANTHASHEDITORFACTORY_H

#include "qtpropertybrowser.h"
#include <QVariant>
#include "qtvariantproperty.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"


class QtVariantHashEditWidget;

class QVariantHashEditorFactory: public QtVariantEditorFactory
{
    Q_OBJECT
public:
    explicit QVariantHashEditorFactory(QObject *parent);
    virtual ~QVariantHashEditorFactory();
protected:
    QtVariantHashEditWidget *editor;
    QtProperty *editorProperty;
    QtVariantPropertyManager *editorManager;
    void connectPropertyManager(QtVariantPropertyManager *manager);
    QWidget *createEditor(QtVariantPropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtVariantPropertyManager *manager);



signals:

public slots:
    void slotPropertyChanged(QtProperty *property, QVariant value);
    void slotSetValue(QString value);
    void slotEditorDestroyed(QObject *ed);
};

#endif // QVARIANTHASHEDITORFACTORY_H
