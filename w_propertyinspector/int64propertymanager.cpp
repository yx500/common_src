#include "int64propertymanager.h"


Int64PropertyManager::Int64PropertyManager(QObject *parent) :
    QtVariantPropertyManager(parent)
{
}

Int64PropertyManager::~Int64PropertyManager()
{

}

QVariant Int64PropertyManager::value(const QtProperty *property) const
{
    if (propertyToData.contains(property)){
        QVariant v;
        v.setValue(propertyToData[property]);
        return v;
    }
    return QtVariantPropertyManager::value(property);
}

int Int64PropertyManager::valueType(int propertyType) const
{
    if (propertyType ==QVariant::ULongLong)
        return QVariant::ULongLong;
    return QtVariantPropertyManager::valueType(propertyType);
}

bool Int64PropertyManager::isPropertyTypeSupported(int propertyType) const
{
    if (propertyType == QVariant::ULongLong)
        return true;
    return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
}

QString Int64PropertyManager::valueText(const QtProperty *property) const
{
    if (propertyToData.contains(property)) {
        quint64 p = propertyToData[property];
        QString s= QString("%1").arg(p);
        return s;
    }
    return QtVariantPropertyManager::valueText(property);
}


void Int64PropertyManager::setValue(QtProperty *property, const QVariant &val)
{
    if (propertyToData.contains(property)) {
        if (val.userType() != QVariant::ULongLong)
            return;
        quint64 p = val.toULongLong();
        quint64 d = propertyToData[property];
        if (d!=p){
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

void Int64PropertyManager::initializeProperty(QtProperty *property)
{
    if (propertyType(property) == QVariant::ULongLong) {
        quint64 d=0;

        propertyToData[property] = d;
    }
    QtVariantPropertyManager::initializeProperty(property);
}

void Int64PropertyManager::uninitializeProperty(QtProperty *property)
{
    if (propertyToData.contains(property)) {
        propertyToData.remove(property);
    }
    QtVariantPropertyManager::uninitializeProperty(property);
}
