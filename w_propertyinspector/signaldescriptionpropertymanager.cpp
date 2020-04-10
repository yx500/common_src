#include "signaldescriptionpropertymanager.h"




SignalDescriptionPropertyManager::SignalDescriptionPropertyManager(QObject *parent) :
    QtVariantPropertyManager(parent)
{
    connect(this, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(slotValueChanged(QtProperty *, const QVariant &)));
    connect(this, SIGNAL(propertyDestroyed(QtProperty *)),
            this, SLOT(slotPropertyDestroyed(QtProperty *)));
}

SignalDescriptionPropertyManager::~SignalDescriptionPropertyManager()
{

}

QVariant SignalDescriptionPropertyManager::value(const QtProperty *property) const
{
    if (propertyToData.contains(property)){
        QVariant v;
        v.setValue(propertyToData[property].value);
        return v;
    }
    return QtVariantPropertyManager::value(property);
}

int SignalDescriptionPropertyManager::valueType(int propertyType) const
{
    if (propertyType == qMetaTypeId<SignalDescription>())
        return qMetaTypeId<SignalDescription>();
    return QtVariantPropertyManager::valueType(propertyType);
}

bool SignalDescriptionPropertyManager::isPropertyTypeSupported(int propertyType) const
{
    if (propertyType == qMetaTypeId<SignalDescription>())
        return true;
    return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
}

QString SignalDescriptionPropertyManager::valueText(const QtProperty *property) const
{
    if (propertyToData.contains(property)) {
        SignalDescription p = propertyToData[property].value;
        QString s=p.toString();

        return s;
    }
    return QtVariantPropertyManager::valueText(property);
}

void SignalDescriptionPropertyManager::setValue(QtProperty *property, const QVariant &val)
{
    if (propertyToData.contains(property)) {
        if (val.userType() != qMetaTypeId<SignalDescription>())
            return;
        SignalDescription p = val.value<SignalDescription>();
        Data d = propertyToData[property];
        if (d.value == p){
        } else {
            d.value = p;
            propertyToData[property] = d;
            if (d.ChanelName)
                d.ChanelName-> setValue(p.chanelName());
            if (d.ChanelType)
                d.ChanelType->setValue(p.chanelType());
            if (d.ChanelOffset)
                d.ChanelOffset->setValue(p.chanelOffset());
            if (d.isInvers)
                d.isInvers->setValue(p.isInvers());
            if (d.isNotUse)
                d.isNotUse->setValue(p.isNotUse());
            if (d.StoredSignalName)
                d.StoredSignalName-> setValue(p.storedSignalName());

            if (d.Value)
                d.Value->setValue(p.value_1bit());
            propertyToData[property] = d;
            QVariant v;
            v.setValue(d.value);
            emit propertyChanged(property);
            emit valueChanged(property, v);
        }
        return;
    }
    QtVariantPropertyManager::setValue(property, val);
}

void SignalDescriptionPropertyManager::initializeProperty(QtProperty *property)
{
    if (propertyType(property) == qMetaTypeId<SignalDescription>()) {
        Data d;

        d.value = SignalDescription();

        SignalDescriptionPropertyManager *that = (SignalDescriptionPropertyManager *)this;

        d.ChanelName = that->addProperty(QVariant::String);
        d.ChanelName->setPropertyName(tr("ChanelName"));
        property->addSubProperty(d.ChanelName);
        ChanelNameToProperty[d.ChanelName] = property;

        d.ChanelType = that->addProperty(QVariant::Int);
        d.ChanelType->setPropertyName(tr("ChanelType"));
        property->addSubProperty(d.ChanelType);
        ChanelTypeToProperty[d.ChanelType] = property;

        d.ChanelOffset = that->addProperty(QVariant::Int);
        d.ChanelOffset->setPropertyName(tr("ChanelOffset"));
        property->addSubProperty(d.ChanelOffset);
        ChanelOffsetToProperty[d.ChanelOffset] = property;

        d.isInvers = that->addProperty(QVariant::Bool);
        d.isInvers->setPropertyName(tr("Invers"));
        property->addSubProperty(d.isInvers);
        isInversToProperty[d.isInvers] = property;

        d.isNotUse = that->addProperty(QVariant::Bool);
        d.isNotUse->setPropertyName(tr("isNotUse"));
        property->addSubProperty(d.isNotUse);
        isNotUseToProperty[d.isNotUse] = property;

        d.StoredSignalName = that->addProperty(QVariant::String);
        d.StoredSignalName->setPropertyName(tr("StoredSignalName"));
        property->addSubProperty(d.StoredSignalName);
        StoredSignalNameToProperty[d.StoredSignalName] = property;

        d.Value = that->addProperty( QVariant::Int);
        d.Value->setPropertyName(tr("Value"));
        d.Value->setEnabled(false);
        property->addSubProperty(d.Value);
        d.Value->setAttribute("readOnly",1);
        ValueToProperty[d.Value] = property;


        propertyToData[property] = d;
    }
    QtVariantPropertyManager::initializeProperty(property);
}

void SignalDescriptionPropertyManager::uninitializeProperty(QtProperty *property)
{
    if (propertyToData.contains(property)) {
        Data d = propertyToData[property];
        if (d.ChanelName)
            ChanelNameToProperty.remove(d.ChanelName);
        if (d.ChanelType)
            ChanelTypeToProperty.remove(d.ChanelType);
        if (d.ChanelOffset)
            ChanelOffsetToProperty.remove(d.ChanelOffset);
        if (d.isInvers)
            isInversToProperty.remove(d.isInvers);
        if (d.isNotUse)
            isNotUseToProperty.remove(d.isNotUse);
        if (d.StoredSignalName)
            StoredSignalNameToProperty.remove(d.StoredSignalName);
        if (d.Value)
            ValueToProperty.remove(d.Value);
        propertyToData.remove(property);
    }
    QtVariantPropertyManager::uninitializeProperty(property);
}

void SignalDescriptionPropertyManager::slotValueChanged(QtProperty *property, const QVariant &value)
{
    if (ChanelNameToProperty.contains(property)) {
        QtProperty *pointProperty = ChanelNameToProperty[property];
        Data d = propertyToData[pointProperty];
        //QVariant v = d.value;// this->value(pointProperty);
        QVariant v;v.setValue<SignalDescription>(d.value);
        SignalDescription p = v.value<SignalDescription>();
        p.setChanelName(value.value<QString>());
        v.setValue(p);
        setValue(pointProperty, v);
    } else if (ChanelOffsetToProperty.contains(property)) {
        QtProperty *pointProperty = ChanelOffsetToProperty[property];
        Data d = propertyToData[pointProperty];
        QVariant v;v.setValue<SignalDescription>(d.value);
        SignalDescription p = v.value<SignalDescription>();
        p.setChanelOffset(value.value<int>());
        v.setValue(p);
        setValue(pointProperty, v);
    } else if (ChanelTypeToProperty.contains(property)) {
        QtProperty *pointProperty = ChanelTypeToProperty[property];
        Data d = propertyToData[pointProperty];
        QVariant v;v.setValue<SignalDescription>(d.value);
        SignalDescription p = v.value<SignalDescription>();
        p.setChanelType(value.value<int>());
        v.setValue(p);
        setValue(pointProperty, v);
    }else if (isInversToProperty.contains(property)) {
        QtProperty *pointProperty = isInversToProperty[property];
        Data d = propertyToData[pointProperty];
        QVariant v;v.setValue<SignalDescription>(d.value);
        SignalDescription p = v.value<SignalDescription>();
        p.setIsInvers(value.value<bool>());
        v.setValue(p);
        setValue(pointProperty, v);
    }else if (isNotUseToProperty.contains(property)) {
        QtProperty *pointProperty = isNotUseToProperty[property];
        Data d = propertyToData[pointProperty];
        QVariant v;v.setValue<SignalDescription>(d.value);
        SignalDescription p = v.value<SignalDescription>();
        p.setIsNoUse(value.value<bool>());
        v.setValue(p);
        setValue(pointProperty, v);
    } else if (StoredSignalNameToProperty.contains(property)) {
        QtProperty *pointProperty = StoredSignalNameToProperty[property];
        Data d = propertyToData[pointProperty];
        QVariant v;v.setValue<SignalDescription>(d.value);
        SignalDescription p = v.value<SignalDescription>();
        p.setStoredSignalName(value.value<QString>());
        v.setValue(p);
        setValue(pointProperty, v);
    } /*else if (ValueToProperty.contains(property)) {
        QtProperty *pointProperty = ValueToProperty[property];
        QVariant v = this->value(pointProperty);
        SignalDescription p = v.value<SignalDescription>();
        p.setValueType(value.value<int>());
        v.setValue(p);
        setValue(pointProperty, v);
    }*/
}

void SignalDescriptionPropertyManager::slotPropertyDestroyed(QtProperty *property)
{
    if (ChanelNameToProperty.contains(property)) {
        QtProperty *pointProperty = ChanelNameToProperty[property];
        propertyToData[pointProperty].ChanelName = 0;
        ChanelNameToProperty.remove(property);
    } else if (ChanelOffsetToProperty.contains(property)) {
        QtProperty *pointProperty = ChanelOffsetToProperty[property];
        propertyToData[pointProperty].ChanelOffset = 0;
        ChanelOffsetToProperty.remove(property);
    } else if (isInversToProperty.contains(property)) {
        QtProperty *pointProperty = isInversToProperty[property];
        propertyToData[pointProperty].isInvers = 0;
        isInversToProperty.remove(property);
    } else if (isNotUseToProperty.contains(property)) {
        QtProperty *pointProperty = isNotUseToProperty[property];
        propertyToData[pointProperty].isNotUse = 0;
        isNotUseToProperty.remove(property);
    } else if (StoredSignalNameToProperty.contains(property)) {
        QtProperty *pointProperty = StoredSignalNameToProperty[property];
        propertyToData[pointProperty].StoredSignalName = 0;
        StoredSignalNameToProperty.remove(property);
    } else if (ValueToProperty.contains(property)) {
        QtProperty *pointProperty = ValueToProperty[property];
        propertyToData[pointProperty].Value = 0;
        ValueToProperty.remove(property);
    }
}
