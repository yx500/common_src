#ifndef SIGNALDESCRIPTIONPROPERTYMANAGER_H
#define SIGNALDESCRIPTIONPROPERTYMANAGER_H

#include "qtvariantproperty.h"
#include "signaldescription.h"
class SignalDescriptionPropertyManager : public QtVariantPropertyManager
{
    Q_OBJECT
public:
    explicit SignalDescriptionPropertyManager(QObject *parent = 0);
    virtual ~SignalDescriptionPropertyManager();

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
    void slotValueChanged(QtProperty *property, const QVariant &value);
    void slotPropertyDestroyed(QtProperty *property);
private:
    struct Data {
        SignalDescription value;
        QtVariantProperty *ChanelName;
        QtVariantProperty *ChanelType;
        QtVariantProperty *ChanelOffset;
        QtVariantProperty *isInvers;
        QtVariantProperty *isNotUse;
        QtVariantProperty *StoredSignalName;
        QtVariantProperty *Value;
    };
    QMap<const QtProperty *, Data> propertyToData;
    QMap<const QtProperty *, QtProperty *> ChanelNameToProperty;
    QMap<const QtProperty *, QtProperty *> ChanelTypeToProperty;
    QMap<const QtProperty *, QtProperty *> ChanelOffsetToProperty;
    QMap<const QtProperty *, QtProperty *> isInversToProperty;
    QMap<const QtProperty *, QtProperty *> isNotUseToProperty;
    QMap<const QtProperty *, QtProperty *> StoredSignalNameToProperty;
    QMap<const QtProperty *, QtProperty *> ValueToProperty;
};

#endif // SIGNALDESCRIPTIONPROPERTYMANAGER_H
