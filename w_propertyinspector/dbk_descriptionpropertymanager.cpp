
#include "dbk_descriptionpropertymanager.h"
#include <QMetaObject>
#include <QMetaProperty>


DBK_DescriptionPropertyManager::DBK_DescriptionPropertyManager(QObject *parent) :
    QtVariantPropertyManager(parent)
{
    connect(this, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                this, SLOT(slotValueChanged(QtProperty *, const QVariant &)));
    connect(this, SIGNAL(propertyDestroyed(QtProperty *)),
            this, SLOT(slotPropertyDestroyed(QtProperty *)));
}

DBK_DescriptionPropertyManager::~DBK_DescriptionPropertyManager()
{

}

QVariant DBK_DescriptionPropertyManager::value(const QtProperty *property) const
{
    if (propertyToData.contains(property)){
        QVariant v;
        v.setValue(propertyToData[property].value);
        return v;
    }
    return QtVariantPropertyManager::value(property);
}

int DBK_DescriptionPropertyManager::valueType(int propertyType) const
{
    if (propertyType == qMetaTypeId<DBK_Description>())
        return qMetaTypeId<DBK_Description>();
    return QtVariantPropertyManager::valueType(propertyType);
}

bool DBK_DescriptionPropertyManager::isPropertyTypeSupported(int propertyType) const
{
    if (propertyType == qMetaTypeId<DBK_Description>())
        return true;
    return QtVariantPropertyManager::isPropertyTypeSupported(propertyType);
}

QString DBK_DescriptionPropertyManager::valueText(const QtProperty *property) const
{
    if (propertyToData.contains(property)) {
        DBK_Description p = propertyToData[property].value;
        QString s=p.toString();

        return s;
    }
    return QtVariantPropertyManager::valueText(property);
}

void DBK_DescriptionPropertyManager::setValue(QtProperty *property, const QVariant &val)
{
    if (propertyToData.contains(property)) {
        if (val.userType() != qMetaTypeId<DBK_Description>())
            return;
        DBK_Description p = val.value<DBK_Description>();
        Data d = propertyToData[property];
        if (d.value == p){
        } else {
            d.value = p;

            if (d.KLC_num )
                d.KLC_num->setValue(p.KLC_num());
            if (d.addr )
                d.addr->setValue(lDBK_addr.keys().indexOf(p.addr()));
            if (d.data_number )
                d.data_number->setValue(lDBK_data_number.keys().indexOf(p.data_number()));


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

void DBK_DescriptionPropertyManager::initializeProperty(QtProperty *property)
{
    if (propertyType(property) == qMetaTypeId<DBK_Description>()) {
        Data d;

        d.value = DBK_Description();

        DBK_DescriptionPropertyManager *that = (DBK_DescriptionPropertyManager *)this;

        d.KLC_num = that->addProperty( QVariant::Int);
        d.KLC_num->setPropertyName(tr("KLC_num"));
        property->addSubProperty(d.KLC_num);
        KLC_numToProperty[d.KLC_num] = property;

        d.addr = that->addProperty( QtVariantPropertyManager::enumTypeId());
        d.addr->setPropertyName(tr("addr"));
        QStringList addr_enumNames;
        lDBK_addr.insert(DBK_addr_nothing,"DBK_addr_nothing");
        lDBK_addr.insert(DBK_addr_350,"DBK_addr_350");
        lDBK_addr.insert(DBK_addr_221,"DBK_addr_221");
        lDBK_addr.insert(DBK_addr_387,"DBK_addr_387");
        lDBK_addr.insert(DBK_addr_124,"DBK_addr_124");
        lDBK_addr.insert(DBK_addr_499,"DBK_addr_499");
        lDBK_addr.insert(DBK_addr_26 ,"DBK_addr_26");
        lDBK_addr.insert(DBK_addr_312,"DBK_addr_312");
        lDBK_addr.insert(DBK_addr_407,"DBK_addr_407");
        lDBK_addr.insert(DBK_addr_310,"DBK_addr_310");
        lDBK_addr.insert(DBK_addr_181,"DBK_addr_181");
        lDBK_addr.insert(DBK_addr_20 ,"DBK_addr_20");
        lDBK_addr.insert(DBK_addr_211,"DBK_addr_211");
        lDBK_addr.insert(DBK_addr_114,"DBK_addr_114");
        lDBK_addr.insert(DBK_addr_497,"DBK_addr_497");
        lDBK_addr.insert(DBK_addr_336,"DBK_addr_336");
        lDBK_addr.insert(DBK_addr_111,"DBK_addr_111");
        lDBK_addr.insert(DBK_addr_206,"DBK_addr_206");
        lDBK_addr.insert(DBK_addr_333,"DBK_addr_333");
        lDBK_addr.insert(DBK_addr_492,"DBK_addr_492");
        lDBK_addr.insert(DBK_addr_299,"DBK_addr_299");
        lDBK_addr.insert(DBK_addr_394,"DBK_addr_394");
        lDBK_addr.insert(DBK_addr_9  ,"DBK_addr_9");
        lDBK_addr.insert(DBK_addr_168,"DBK_addr_168");
        lDBK_addr.insert(DBK_addr_7  ,"DBK_addr_7");
        lDBK_addr.insert(DBK_addr_166,"DBK_addr_166");
        lDBK_addr.insert(DBK_addr_293,"DBK_addr_293");
        lDBK_addr.insert(DBK_addr_388,"DBK_addr_388");
        lDBK_addr.insert(DBK_addr_323,"DBK_addr_323");
        lDBK_addr.insert(DBK_addr_482,"DBK_addr_482");
        lDBK_addr.insert(DBK_addr_97 ,"DBK_addr_97 ");
        lDBK_addr.insert(DBK_addr_192,"DBK_addr_192");
        for (int i=0;i<lDBK_addr.keys().size();i++)
                addr_enumNames.append(lDBK_addr.values().at(i));
        d.addr->setAttribute(QLatin1String("enumNames"), addr_enumNames);
        property->addSubProperty(d.addr);
        addrToProperty[d.addr] = property;

        d.data_number = that->addProperty( QtVariantPropertyManager::enumTypeId());
        d.data_number->setPropertyName(tr("data_number"));
        QStringList data_number_enumNames;
        lDBK_data_number.insert(DBK_data_n1,"DBK_data_n1");
        lDBK_data_number.insert(DBK_data_n2,"DBK_data_n2");
        lDBK_data_number.insert(DBK_data_n3,"DBK_data_n3");
        lDBK_data_number.insert(DBK_data_n4,"DBK_data_n4");
        lDBK_data_number.insert(DBK_data_n5,"DBK_data_n5");
        for (int i=0;i<lDBK_data_number.keys().size();i++)
                data_number_enumNames.append(lDBK_data_number.values().at(i));
        d.data_number->setAttribute(QLatin1String("enumNames"), data_number_enumNames);
        property->addSubProperty(d.data_number);
        data_numberToProperty[d.data_number] = property;



        propertyToData[property] = d;
    }
    QtVariantPropertyManager::initializeProperty(property);
}

void DBK_DescriptionPropertyManager::uninitializeProperty(QtProperty *property)
{
    if (propertyToData.contains(property)) {
        Data d = propertyToData[property];
        if (d.KLC_num)
            KLC_numToProperty.remove(d.KLC_num);
        if (d.addr)
            addrToProperty.remove(d.addr);
        if (d.data_number)
            data_numberToProperty.remove(d.data_number);

        propertyToData.remove(property);
    }
    QtVariantPropertyManager::uninitializeProperty(property);
}

void DBK_DescriptionPropertyManager::slotValueChanged(QtProperty *property, const QVariant &value)
{
    if (KLC_numToProperty.contains(property)) {
        QtProperty *pointProperty = KLC_numToProperty[property];
        QVariant v = this->value(pointProperty);
        DBK_Description p = v.value<DBK_Description>();
        int ev=value.value<int>();
        p.setKLC_num(ev);
        v.setValue(p);
        setValue(pointProperty, v);
    } else if (addrToProperty.contains(property)) {
        QtProperty *pointProperty = addrToProperty[property];
        QVariant v = this->value(pointProperty);
        DBK_Description p = v.value<DBK_Description>();
        int ev=value.value<int>();
        p.setAddr(lDBK_addr.keys().at(ev));
        v.setValue(p);
        setValue(pointProperty, v);
    } else if (data_numberToProperty.contains(property)) {
        QtProperty *pointProperty = data_numberToProperty[property];
        QVariant v = this->value(pointProperty);
        DBK_Description p = v.value<DBK_Description>();
        int ev=value.value<int>();
        p.setData_number(lDBK_data_number.keys().at(ev));
        v.setValue(p);
        setValue(pointProperty, v);
    }
}

void DBK_DescriptionPropertyManager::slotPropertyDestroyed(QtProperty *property)
{
    if (KLC_numToProperty.contains(property)) {
        QtProperty *pointProperty = KLC_numToProperty[property];
        propertyToData[pointProperty].KLC_num= 0;
        KLC_numToProperty.remove(property);
    }
    if (addrToProperty.contains(property)) {
        QtProperty *pointProperty = addrToProperty[property];
        propertyToData[pointProperty].addr= 0;
        addrToProperty.remove(property);
    }
    if (data_numberToProperty.contains(property)) {
        QtProperty *pointProperty = data_numberToProperty[property];
        propertyToData[pointProperty].data_number= 0;
        data_numberToProperty.remove(property);
    }
}
