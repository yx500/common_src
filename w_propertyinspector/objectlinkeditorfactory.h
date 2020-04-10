#ifndef OBJECTLINKEDITORFACTORY_H
#define OBJECTLINKEDITORFACTORY_H


#include "qtpropertybrowser.h"
#include <QVariant>
#include "qtvariantproperty.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"


class QLineEdit;
class ObjectLinkPropertyManager;

class ObjectLinkEditorFactory : public QtVariantEditorFactory
{
    Q_OBJECT
public:
    explicit ObjectLinkEditorFactory(QObject *parent);
    virtual ~ObjectLinkEditorFactory();
protected:
    QLineEdit *editor;
    QtProperty *editorProperty;
    ObjectLinkPropertyManager *editorManager;
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

#endif // OBJECTLINKEDITORFACTORY_H
