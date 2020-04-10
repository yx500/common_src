#ifndef INT64PROPERTYMANAGER_H
#define INT64PROPERTYMANAGER_H



#include "qtvariantproperty.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"

class Int64PropertyManager : public QtVariantPropertyManager
{
    Q_OBJECT
public:
    explicit Int64PropertyManager(QObject *parent = 0);
    virtual ~Int64PropertyManager();

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

    QMap<const QtProperty *, quint64> propertyToData;
};

#endif // INT64PROPERTYMANAGER_H
