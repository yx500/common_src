#ifndef QVARINATHASHPROPERTYMANAGER_H
#define QVARINATHASHPROPERTYMANAGER_H

#include "qtvariantproperty.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"

class QVarinatHashPropertyManager : public QtVariantPropertyManager
{
    Q_OBJECT
public:
    explicit QVarinatHashPropertyManager(QObject *parent = nullptr);
    virtual ~QVarinatHashPropertyManager(){}

    virtual QVariant value(const QtProperty *property) const;
    virtual int valueType(int propertyType) const;
    virtual bool isPropertyTypeSupported(int propertyType) const;

    QString valueText(const QtProperty *property) const;

public slots:
    virtual void setValue(QtProperty *property, const QVariant &val);
protected:
    virtual void initializeProperty(QtProperty *property);
    virtual void uninitializeProperty(QtProperty *property);
private slots:
private:

    QMap<const QtProperty *, QVariantHash> propertyToData;
};


#endif // QVARINATHASHPROPERTYMANAGER_H
