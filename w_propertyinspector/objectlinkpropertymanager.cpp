#include "objectlinkpropertymanager.h"

ObjectLinkPropertyManager::ObjectLinkPropertyManager(QObject *parent) :
    QtVariantPropertyManager(parent)
{
}

QVariant ObjectLinkPropertyManager::value(const QtProperty *property) const
{
    if (propertyToData.contains(property)){
        QVariant v;
        v.setValue(propertyToData[property]);
        return v;
    }
    return QtVariantPropertyManager::value(property);
}

int ObjectLinkPropertyManager::valueType(int propertyType) const
{
    if (propertyType == qMetaTypeId<ObjectLink>())
        return qMetaTypeId<ObjectLink>();
    return QtVariantPropertyManager::valueType(propertyType);
}

bool ObjectLinkPropertyManager::isPropertyTypeSupported(int propertyType) const
{
    if (propertyType == qMetaTypeId<ObjectLink>())
        return true;
    return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
}

QString ObjectLinkPropertyManager::valueText(const QtProperty *property) const
{
    if (propertyToData.contains(property)) {
        ObjectLink p = propertyToData[property];
        QString s=p.toString();
        if (!s.isEmpty()){
            if (p.obj()!=0)
                return p.obj()->objectName();
            return "0 ["+s+"]";
        }
        return s;
    }
    return QtVariantPropertyManager::valueText(property);
}


void ObjectLinkPropertyManager::setValue(QtProperty *property, const QVariant &val)
{
    if (propertyToData.contains(property)) {
        if (val.userType() != qMetaTypeId<ObjectLink>())
            return;
        ObjectLink p = val.value<ObjectLink>();
        ObjectLink d = propertyToData[property];
        if ((d.id()!=p.id())||(d.isNotUse()!=p.isNotUse())||(d.obj()!=p.obj())){
            d=p;
            propertyToData[property] = d;
            QVariant v;
            v.setValue(d);
            emit propertyChanged(property);
            emit valueChanged(property, v);
        }
        return;
    }
    QtVariantPropertyManager::setValue(property, val);
}

void ObjectLinkPropertyManager::initializeProperty(QtProperty *property)
{
    if (propertyType(property) == qMetaTypeId<ObjectLink>()) {
        ObjectLink d;

        propertyToData[property] = d;
    }
    QtVariantPropertyManager::initializeProperty(property);
}

void ObjectLinkPropertyManager::uninitializeProperty(QtProperty *property)
{
    if (propertyToData.contains(property)) {
        ObjectLink d = propertyToData[property];
        propertyToData.remove(property);
    }
    QtVariantPropertyManager::uninitializeProperty(property);
}
