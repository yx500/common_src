#ifndef OBJECTLINKPROPERTYMANAGER_H
#define OBJECTLINKPROPERTYMANAGER_H

#include "qtvariantproperty.h"
#include "qteditorfactory.h"
#include "qttreepropertybrowser.h"
#include "objectlink.h"


class ObjectLinkPropertyManager : public QtVariantPropertyManager
{
    Q_OBJECT
public:
    explicit ObjectLinkPropertyManager(QObject *parent = 0);
    virtual ~ObjectLinkPropertyManager(){}

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

    QMap<const QtProperty *, ObjectLink> propertyToData;
};

#endif // OBJECTLINKPROPERTYMANAGER_H
