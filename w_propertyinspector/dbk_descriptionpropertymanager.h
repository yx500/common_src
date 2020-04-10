#ifndef DBK_DESCRIPTIONPROPERTYMANAGER_H
#define DBK_DESCRIPTIONPROPERTYMANAGER_H

#include "qtvariantproperty.h"
#include "dbk_description.h"
class DBK_DescriptionPropertyManager : public QtVariantPropertyManager
{
     Q_OBJECT
public:

    explicit DBK_DescriptionPropertyManager(QObject *parent = 0);
    virtual ~DBK_DescriptionPropertyManager();

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
        DBK_Description value;
        QtVariantProperty *KLC_num;
        QtVariantProperty *addr;
        QtVariantProperty *data_number;
    };
    QMap<const QtProperty *, Data> propertyToData;
    QMap<const QtProperty *, QtProperty *> KLC_numToProperty;
    QMap<const QtProperty *, QtProperty *> addrToProperty;
    QMap<const QtProperty *, QtProperty *> data_numberToProperty;
    QMap<TDBK_addr, QString> lDBK_addr;
    QMap<int, QString> lDBK_data_number;
};
#endif // DBK_DESCRIPTIONPROPERTYMANAGER_H
