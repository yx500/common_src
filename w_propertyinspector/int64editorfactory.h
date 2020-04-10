#ifndef INT64EDITORFACTORY_H
#define INT64EDITORFACTORY_H


#include "qtpropertybrowser.h"
#include <QVariant>
#include "qtvariantproperty.h"
#include "qtpropertymanager.h"
#include "qteditorfactory.h"


class QLineEdit;
class Int64PropertyManager;

class Int64EditorFactory : public QtVariantEditorFactory
{
    Q_OBJECT
public:
    explicit Int64EditorFactory(QObject *parent = 0);
    virtual ~Int64EditorFactory();

protected:
    QLineEdit *editor;
    QtProperty *editorProperty;
    Int64PropertyManager *editorManager;
    void connectPropertyManager(QtVariantPropertyManager *manager);
    QWidget *createEditor(QtVariantPropertyManager *manager, QtProperty *property,
                QWidget *parent);
    void disconnectPropertyManager(QtVariantPropertyManager *manager);

signals:

public slots:
    void slotPropertyChanged(QtProperty *property, QVariant value);
    void slotSetValue(double value);
    void slotEditorDestroyed(QObject *ed);

};


#endif // INT64EDITORFACTORY_H
