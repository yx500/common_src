#include "qvarinathashpropertymanager.h"
#include "mvp_system.h"

QVarinatHashPropertyManager::QVarinatHashPropertyManager(QObject *parent) : QtVariantPropertyManager(parent)
{

}
QVariant QVarinatHashPropertyManager::value(const QtProperty *property) const
{
    if (propertyToData.contains(property)){
        QVariant v;
        v.setValue(propertyToData[property]);
        return v;
    }
    return QtVariantPropertyManager::value(property);
}

int QVarinatHashPropertyManager::valueType(int propertyType) const
{
    if (propertyType == QVariant::Hash)
        return QVariant::Hash;
    return QtVariantPropertyManager::valueType(propertyType);
}

bool QVarinatHashPropertyManager::isPropertyTypeSupported(int propertyType) const
{
    if (propertyType == QVariant::Hash)
        return true;
    return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
}


QString QVarinatHashPropertyManager::valueText(const QtProperty *property) const
{
    if (propertyToData.contains(property)) {
        QVariantHash p = propertyToData[property];
        QString s=MVP_System::QVariantHashToQString(p);
        return s;
    }
    return QtVariantPropertyManager::valueText(property);
}


void QVarinatHashPropertyManager::setValue(QtProperty *property, const QVariant &val)
{
    if (propertyToData.contains(property)) {
        if (val.userType() != QVariant::Hash)
            return;
        QVariantHash p = val.toHash();
        QVariantHash d = propertyToData[property];
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

void QVarinatHashPropertyManager::initializeProperty(QtProperty *property)
{
    if (propertyType(property) ==QVariant::Hash) {
        QVariantHash d;

        propertyToData[property] = d;
    }
    QtVariantPropertyManager::initializeProperty(property);
}

void QVarinatHashPropertyManager::uninitializeProperty(QtProperty *property)
{
    if (propertyToData.contains(property)) {
        QVariantHash d = propertyToData[property];
        propertyToData.remove(property);
    }
    QtVariantPropertyManager::uninitializeProperty(property);
}
