/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Solutions component.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QMetaObject>
#include <QMetaProperty>
#include <QVBoxLayout>
#include <QScrollArea>
#include "baseobjectcontroller.h"
#include "qtvariantproperty.h"
#include "qtgroupboxpropertybrowser.h"
#include "qttreepropertybrowser.h"
#include "qtpropertybrowser.h"

#include "int64propertymanager.h"
#include "objectlinkpropertymanager.h"
#include "signaldescriptionpropertymanager.h"
#include "qvarinathashpropertymanager.h"
#include "qvarianthasheditorfactory.h"

#include "objectlinkeditorfactory.h"
#include "int64editorfactory.h"
#include "mvp_system.h"
#include "baseobjecttools.h"

class BaseObjectControllerPrivate
{
    BaseObjectController *q_ptr;
    Q_DECLARE_PUBLIC(BaseObjectController)
public:
    void addManager(QtVariantPropertyManager *manager, QtVariantEditorFactory *abstractFactory);

    void addClassProperties(const QMetaObject *metaObject);
    void addProperties();
    void addClassDynamicProperties(const QObject *O);
    //void updateClassProperties(const QMetaObject *metaObject, bool recursive);
    void updateProperties();
    void saveExpandedState();
    void restoreExpandedState();
    void slotValueChanged(QtProperty *property, const QVariant &value);
    int enumToInt(const QMetaEnum &metaEnum, int enumValue) const;
    int intToEnum(const QMetaEnum &metaEnum, int intValue) const;
    int flagToInt(const QMetaEnum &metaEnum, int flagValue) const;
    int intToFlag(const QMetaEnum &metaEnum, int intValue) const;
    bool isSubValue(int value, int subValue) const;
    bool isPowerOf2(int value) const;

    QList<QPointer<QObject>>     m_objects;
    QStringList                         m_commonPropertyNames;

    QMap<const QMetaObject *, QtProperty *> m_classToProperty;
    QMap<QtProperty *, const QMetaObject *> m_propertyToClass;
    QMap<QtProperty *, int>     m_propertyToIndex;
    QMap<const QMetaObject *, QMap<int, QtVariantProperty *> > m_classToIndexToProperty;

    QMap<QtProperty *, bool>    m_propertyToExpanded;

    QList<QtProperty *>         m_topLevelProperties;

    QtAbstractPropertyBrowser    *m_browser;
    QtVariantPropertyManager *m_manager;
    QtVariantPropertyManager *m_readOnlyManager;


    QMap<QtProperty *, QString>     m_propertyToDPropertyName;
    QMap<QString ,QtProperty *>     m_DPropertyNameToproperty;
    bool onlyState;

    QList<QtVariantProperty *> m_allProperties;

    QtVariantProperty *makeQtVariantProperty(QMetaProperty metaProperty);
    QStringList expandedPropertyeNames;
    bool updateMode;
    QList<QtVariantPropertyManager *> l_Managers;

};

void BaseObjectControllerPrivate::addManager(QtVariantPropertyManager *manager, QtVariantEditorFactory *abstractFactory)
{
    l_Managers.push_back(manager);
    m_browser->setFactoryForManager(qobject_cast<QtVariantPropertyManager*>(manager), abstractFactory);


}


int BaseObjectControllerPrivate::enumToInt(const QMetaEnum &metaEnum, int enumValue) const
{
    QMap<int, int> valueMap; // dont show multiple enum values which have the same values
    int pos = 0;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value)) {
            if (value == enumValue)
                return pos;
            valueMap[value] = pos++;
        }
    }
    return -1;
}

int BaseObjectControllerPrivate::intToEnum(const QMetaEnum &metaEnum, int intValue) const
{
    QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
    QList<int> values;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value)) {
            valueMap[value] = true;
            values.append(value);
        }
    }
    if (intValue >= values.count())
        return -1;
    return values.at(intValue);
}

bool BaseObjectControllerPrivate::isSubValue(int value, int subValue) const
{
    if (value == subValue)
        return true;
    int i = 0;
    while (subValue) {
        if (!(value & (1 << i))) {
            if (subValue & 1)
                return false;
        }
        i++;
        subValue = subValue >> 1;
    }
    return true;
}

bool BaseObjectControllerPrivate::isPowerOf2(int value) const
{
    while (value) {
        if (value & 1) {
            return value == 1;
        }
        value = value >> 1;
    }
    return false;
}

int BaseObjectControllerPrivate::flagToInt(const QMetaEnum &metaEnum, int flagValue) const
{
    if (!flagValue)
        return 0;
    int intValue = 0;
    QMap<int, int> valueMap; // dont show multiple enum values which have the same values
    int pos = 0;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value) && isPowerOf2(value)) {
            if (isSubValue(flagValue, value))
                intValue |= (1 << pos);
            valueMap[value] = pos++;
        }
    }
    return intValue;
}

int BaseObjectControllerPrivate::intToFlag(const QMetaEnum &metaEnum, int intValue) const
{
    QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
    QList<int> values;
    for (int i = 0; i < metaEnum.keyCount(); i++) {
        int value = metaEnum.value(i);
        if (!valueMap.contains(value) && isPowerOf2(value)) {
            valueMap[value] = true;
            values.append(value);
        }
    }
    int flagValue = 0;
    int temp = intValue;
    int i = 0;
    while (temp) {
        if (i >= values.count())
            return -1;
        if (temp & 1)
            flagValue |= values.at(i);
        i++;
        temp = temp >> 1;
    }
    return flagValue;
}

//void BaseObjectControllerPrivate::updateClassProperties(const QMetaObject *metaObject, bool recursive)
//{
//    if (!metaObject)
//        return;

//    if (recursive)
//        updateClassProperties(metaObject->superClass(), recursive);

//    QtProperty *classProperty = m_classToProperty.value(metaObject);
//    if (!classProperty)
//        return;

//    for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++) {
//        QMetaProperty metaProperty = metaObject->property(idx);
//        if (metaProperty.isReadable()) {
//            if (m_classToIndexToProperty.contains(metaObject) && m_classToIndexToProperty[metaObject].contains(idx)) {
//                QtVariantProperty *subProperty = m_classToIndexToProperty[metaObject][idx];
//                if (metaProperty.isEnumType()) {
//                    if (metaProperty.isFlagType())
//                        subProperty->setValue(flagToInt(metaProperty.enumerator(), metaProperty.read(m_object).toInt()));
//                    else
//                        subProperty->setValue(enumToInt(metaProperty.enumerator(), metaProperty.read(m_object).toInt()));
//                } else {
//                    subProperty->setValue(metaProperty.read(m_object));
//                }
//            }
//        }
//    }


//}
void BaseObjectControllerPrivate::updateProperties()
{
    updateMode=true;
    QObject *m_object=0;
    if (m_objects.size()>0) {
        m_object=m_objects.first();
    }
    if (!m_object) return;
    foreach (QtVariantProperty *subProperty, m_allProperties) {
        int idx=m_object->metaObject()->indexOfProperty(qPrintable(subProperty->propertyName()));
        if (idx>=0){
            QMetaProperty metaProperty = m_object->metaObject()->property(idx);
            QVariant val=metaProperty.read(m_object);
            if (metaProperty.isEnumType()) {
                if (metaProperty.isFlagType()){
                    subProperty->setValue(flagToInt(metaProperty.enumerator(), metaProperty.read(m_object).toInt()));
                }
                else
                {
                    subProperty->setValue(enumToInt(metaProperty.enumerator(), metaProperty.read(m_object).toInt()));
                }
            } else {
                //subProperty->propertyManager()->blockSignals(true);
                subProperty->setValue(metaProperty.read(m_object));
                //subProperty->propertyManager()->blockSignals(false);

            }
            // проверяем на разные значения
            QString valstr;
            if (MVP_System::QVariantToQString(val,valstr)){
                foreach (QObject *m_object, m_objects) {
                    int idx2=m_object->metaObject()->indexOfProperty(qPrintable(subProperty->propertyName()));
                    if (idx>=0){
                        QMetaProperty metaProperty2 = m_object->metaObject()->property(idx2);
                        QVariant val1=metaProperty2.read(m_object);
                        QString valstr1;
                        if (MVP_System::QVariantToQString(val1,valstr1)){
                            if (valstr!=valstr1){
                                QtTreePropertyBrowser *tree_browser=qobject_cast<QtTreePropertyBrowser *>(m_browser);
                                if (tree_browser){
                                    QtBrowserItem * classQtBrowserItem= m_browser->items(subProperty).first();
                                    tree_browser->setBackgroundColor(classQtBrowserItem,Qt::yellow);
                                }
                                break;
                            }
                        }
                    }
                }
            }
        }else {
            subProperty->setValue(QVariant());

        }
        //subProperty->propertyManager()->blockSignals(true);
        //subProperty->setValue(value);
        //subProperty->propertyManager()->blockSignals(false);
    }
    updateMode=false;
}
QtVariantProperty *BaseObjectControllerPrivate::makeQtVariantProperty(QMetaProperty metaProperty)
{
    QtVariantProperty *subProperty = 0;
    int type = metaProperty.userType();
    if (!metaProperty.isReadable()) {
        subProperty = m_readOnlyManager->addProperty(QVariant::String, QLatin1String(metaProperty.name()));
        subProperty->setValue(QLatin1String("< Non Readable >"));
    } else if (metaProperty.isEnumType()) {
        if (metaProperty.isFlagType()) {
            subProperty = m_manager->addProperty(QtVariantPropertyManager::flagTypeId(), QLatin1String(metaProperty.name()));
            QMetaEnum metaEnum = metaProperty.enumerator();
            QMap<int, bool> valueMap;
            QStringList flagNames;
            for (int i = 0; i < metaEnum.keyCount(); i++) {
                int value = metaEnum.value(i);
                if (!valueMap.contains(value) && isPowerOf2(value)) {
                    valueMap[value] = true;
                    flagNames.append(QLatin1String(metaEnum.key(i)));
                }
                subProperty->setAttribute(QLatin1String("flagNames"), flagNames);
//                subProperty->setValue(flagToInt(metaEnum, metaProperty.read(m_object).toInt()));
            }
            subProperty->setAttribute(QLatin1String("flagNames"), flagNames);
        } else {
            subProperty = m_manager->addProperty(QtVariantPropertyManager::enumTypeId(), QLatin1String(metaProperty.name()));
            QMetaEnum metaEnum = metaProperty.enumerator();
            QMap<int, bool> valueMap; // dont show multiple enum values which have the same values
            QStringList enumNames;
            for (int i = 0; i < metaEnum.keyCount(); i++) {
                int value = metaEnum.value(i);
                if (!valueMap.contains(value)) {
                    valueMap[value] = true;
                    enumNames.append(QLatin1String(metaEnum.key(i)));
                }
            }
            subProperty->setAttribute(QLatin1String("enumNames"), enumNames);
//            subProperty->propertyManager()->blockSignals(true);
//            subProperty->setValue(enumToInt(metaEnum, metaProperty.read(m_object).toInt()));
//            subProperty->propertyManager()->blockSignals(false);
        }
    } else {
            bool unknownType=true;

            foreach (auto manager, l_Managers) {
                if (manager->isPropertyTypeSupported(type)) {
                    unknownType=false;
                    if (!metaProperty.isWritable())
                        subProperty = m_readOnlyManager->addProperty(type, QLatin1String(metaProperty.name()) + QLatin1String(" (Non Writable)"));
                    if (!metaProperty.isDesignable())
                        subProperty = m_readOnlyManager->addProperty(type, QLatin1String(metaProperty.name()) + QLatin1String(" (Non Designable)"));
                    else
                        subProperty = manager->addProperty(type, QLatin1String(metaProperty.name()));
                    break;
            }
            }

                if (unknownType)
        {
            subProperty = m_readOnlyManager->addProperty(QVariant::String, QLatin1String(metaProperty.name()));
            subProperty->setValue(QLatin1String("< Unknown Type >"));
            subProperty->setEnabled(false);
        }
        if (subProperty){

//            subProperty->propertyManager()->blockSignals(true);
//            subProperty->setValue(metaProperty.read(m_object));
//            subProperty->propertyManager()->blockSignals(false);
        }
    }
    return subProperty;
}

void BaseObjectControllerPrivate::addClassProperties(const QMetaObject *metaObject)
{
    if (metaObject==nullptr)
        return;

    addClassProperties(metaObject->superClass());

    QtProperty *classProperty = m_classToProperty.value(metaObject);
    if (!classProperty) {
        QString className = QLatin1String(metaObject->className());
        classProperty = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), className);
        m_classToProperty[metaObject] = classProperty;
        m_propertyToClass[classProperty] = metaObject;

        for (int idx = metaObject->propertyOffset(); idx < metaObject->propertyCount(); idx++) {
            QMetaProperty metaProperty = metaObject->property(idx);
            if (onlyState){
                if (metaProperty.isStored()) continue;
            } else {
                if (!metaProperty.isStored()) continue;
            }
            if (!metaProperty.isDesignable()) continue;
            QtVariantProperty *subProperty = makeQtVariantProperty(metaProperty);



            classProperty->addSubProperty(subProperty);
            m_propertyToIndex[subProperty] = idx;
            m_classToIndexToProperty[metaObject][idx] = subProperty;
            m_allProperties.push_back(subProperty);
        }


    } else {
        //updateClassProperties(metaObject, false);
        updateProperties();
    }
    if (classProperty->subProperties().size()>0) {
        m_topLevelProperties.append(classProperty);
        QtBrowserItem * classQtBrowserItem= m_browser->addProperty(classProperty);
        QtTreePropertyBrowser *tree_browser=qobject_cast<QtTreePropertyBrowser *>(m_browser);

        if (tree_browser){
            foreach (QtBrowserItem *I, classQtBrowserItem->children()) {
          //      tree_browser->setBackgroundColor(I,tree_browser->backgroundColor(I));
                tree_browser->setExpanded(I,false);
            }
        }
    }


}

void BaseObjectControllerPrivate::addProperties()
{
    if (m_objects.size()==0)
        return;

    //QString className = "---";
    //QtProperty *classProperty = m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), className);

    updateMode=true;
    QMap<QString,QtProperty*> m_groups;
    const QMetaObject *metaObject=m_objects.first()->metaObject();
    foreach (QString propertyName, m_commonPropertyNames) {
        int idx=metaObject->indexOfProperty(qPrintable(propertyName));
        if (idx>=0){
            QMetaProperty metaProperty = metaObject->property(idx);
            if (onlyState){
                if (metaProperty.isStored()) continue;
            } else {
                if (!metaProperty.isStored()) continue;
            }
            if (!metaProperty.isDesignable()) continue;
            QtVariantProperty *subProperty =nullptr;
            if (metaProperty.userType()==QMetaType::QVariant){
                QVariant v=m_objects.first()->property(qPrintable(propertyName));
                int type=v.type();
                subProperty = m_manager->addProperty(type, propertyName);
                subProperty->setValue(v);
            } else {
             subProperty=makeQtVariantProperty(metaProperty);
            }
            QString property_group=MVP_ObjectFactory::instance()->property_group(metaObject,metaProperty.name());
            subProperty->setPropertyRusName(MVP_ObjectFactory::instance()->property_rusname(metaObject,metaProperty.name()));
            //subProperty->setPropertyName(MVP_ObjectFactory::instance()->property_rusname(metaObject,metaProperty.name()));

            QString property_tooltip=QString("%1\r\n[%2]").arg(MVP_ObjectFactory::instance()->property_toolTip(metaObject,metaProperty.name())).arg(subProperty->propertyName());
            subProperty->setToolTip(property_tooltip);

            double singleStep=MVP_ObjectFactory::instance()->property_singleStep(metaObject,metaProperty.name());
            if (singleStep>0){
                subProperty->setAttribute(QLatin1String("singleStep"), singleStep);
                int decimalsAttribute=MVP_ObjectFactory::instance()->property_decimalsAttribute(metaObject,metaProperty.name());
                subProperty->setAttribute(QLatin1String("decimalsAttribute"), decimalsAttribute);

            }

            m_allProperties.push_back(subProperty);
            QtBrowserItem * classQtBrowserItem=nullptr;
            if (!property_group.isEmpty()){
                QtProperty *classProperty;
                if (m_groups.contains(property_group)) {
                    classProperty=m_groups[property_group];
                } else {
                    classProperty=m_manager->addProperty(QtVariantPropertyManager::groupTypeId(), property_group);
                    m_groups[property_group]=classProperty;
                    m_topLevelProperties.append(classProperty);
                    classQtBrowserItem= m_browser->addProperty(classProperty);
                }
                classProperty->addSubProperty(subProperty);
            } else {
                m_topLevelProperties.append(subProperty);
                classQtBrowserItem= m_browser->addProperty(subProperty);
            }

            if (classQtBrowserItem){
                QtTreePropertyBrowser *tree_browser=qobject_cast<QtTreePropertyBrowser *>(m_browser);
                if (tree_browser){
                    tree_browser->setRootIsDecorated(true);
                    tree_browser->setExpanded(classQtBrowserItem,false);
                }
            }
        }

    }
    if (m_objects.first()->dynamicPropertyNames().size()>0)
        addClassDynamicProperties(m_objects.first());
    updateMode=false;

}



void BaseObjectControllerPrivate::addClassDynamicProperties(const QObject *O)
{
    if (!O)
        return;

    QList<QByteArray> ln=O->dynamicPropertyNames();
    if (ln.size()==0) return;
    for (int i=0;i<ln.size();i++){
            QByteArray pn=ln[i];
            QtVariantProperty *subProperty = nullptr;
            QVariant v=O->property(pn);
            QString properyName=QString(pn);
            int type=v.type();
            subProperty = m_manager->addProperty(type, pn);
            subProperty->setValue(v);
            m_propertyToDPropertyName[subProperty] = properyName;
            m_DPropertyNameToproperty[properyName] = subProperty;
            m_topLevelProperties.append(subProperty);
            m_browser->addProperty(subProperty);
    }

}

QStringList listExpandedPropertyeNames(QtTreePropertyBrowser *tree_browser,QtBrowserItem *I)
{
    QStringList l;
    if (I->children().size()>0){
        if (tree_browser->isExpanded(I)) l.push_back(I->property()->propertyName());
        foreach(QtBrowserItem *sI,I->children()){
            QStringList l1=listExpandedPropertyeNames(tree_browser,sI);
            for (int i=0;i<l1.size();i++) l.push_back(l1[i]);
        }
    }
    return l;
}

void BaseObjectControllerPrivate::saveExpandedState()
{
    expandedPropertyeNames.clear();
    QtTreePropertyBrowser *tree_browser=qobject_cast<QtTreePropertyBrowser *>(m_browser);
    if (tree_browser){
        foreach(QtBrowserItem *I,tree_browser->topLevelItems()){
            QStringList l1=listExpandedPropertyeNames(tree_browser,I);
            for (int i=0;i<l1.size();i++) expandedPropertyeNames.push_back(l1[i]);
        }
    }
}
void setExpandedPropertyeNames(QtTreePropertyBrowser *tree_browser,QtBrowserItem *I,QString propertyName)
{
    if (I->children().size()==0) return;
    if (propertyName.isEmpty()) {
        tree_browser->setExpanded(I,false);
    } else {
        if (I->property()->propertyName()==propertyName) tree_browser->setExpanded(I,true);
    }
    foreach(QtBrowserItem *sI,I->children()){
        setExpandedPropertyeNames(tree_browser,sI,propertyName);
    }
}


void BaseObjectControllerPrivate::restoreExpandedState()
{
    QtTreePropertyBrowser *tree_browser=qobject_cast<QtTreePropertyBrowser *>(m_browser);
    if (tree_browser){
        foreach(QtBrowserItem *I,tree_browser->topLevelItems()){
            setExpandedPropertyeNames(tree_browser,I,QString());
            foreach(QString propertyName,expandedPropertyeNames){
                setExpandedPropertyeNames(tree_browser,I,propertyName);
            }
        }
    }
}

void BaseObjectControllerPrivate::slotValueChanged(QtProperty *property, const QVariant &value)
{
    if (updateMode) return;
    foreach (QObject *m_object, m_objects) {


        if (m_propertyToDPropertyName.contains(property)){
            QString properyName=m_propertyToDPropertyName[property];
            m_object->setProperty(qPrintable(properyName),value);
            continue;
        }

        const QMetaObject *metaObject = m_object->metaObject();
        int idx=m_object->metaObject()->indexOfProperty(qPrintable(property->propertyName()));
        if (idx>=0){
            QMetaProperty metaProperty = metaObject->property(idx);
            if (metaProperty.isEnumType()) {
                if (metaProperty.isFlagType())
                    metaProperty.write(m_object, intToFlag(metaProperty.enumerator(), value.toInt()));
                else
                    metaProperty.write(m_object, intToEnum(metaProperty.enumerator(), value.toInt()));
            } else {
                metaProperty.write(m_object, value);
            }
            //emit q_ptr->propertyChange(m_object);
        }
    }
    ///updateClassProperties(metaObject, true);
}

///////////////////

BaseObjectController::BaseObjectController(QWidget *parent)
    : QWidget(parent)
{
    d_ptr = new BaseObjectControllerPrivate;
    d_ptr->onlyState=false;
    d_ptr->q_ptr = this;

    //d_ptr->m_object = 0;
/*
    QScrollArea *scroll = new QScrollArea(this);
    scroll->setWidgetResizable(true);

    d_ptr->m_browser = new QtGroupBoxPropertyBrowser(this);
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(scroll);
    scroll->setWidget(d_ptr->m_browser);
*/
    QtTreePropertyBrowser *browser = new QtTreePropertyBrowser(this);
    browser->setRootIsDecorated(false);
    d_ptr->m_browser = browser;
    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->addWidget(d_ptr->m_browser);

    d_ptr->m_readOnlyManager = new QtVariantPropertyManager(this);
    d_ptr->m_manager = new QtVariantPropertyManager(this);
    QtVariantEditorFactory *factory = new QtVariantEditorFactory(this);
    d_ptr->addManager(d_ptr->m_manager,factory);
    d_ptr->addManager(new Int64PropertyManager(this),new Int64EditorFactory(this));
    d_ptr->addManager(new ObjectLinkPropertyManager,new ObjectLinkEditorFactory(this));
    d_ptr->addManager(new SignalDescriptionPropertyManager(this),factory);
    d_ptr->addManager(new QVarinatHashPropertyManager,new QVariantHashEditorFactory(this));

    foreach (auto manager, d_ptr->l_Managers ) {
        connect(manager, SIGNAL(valueChanged(QtProperty *, const QVariant &)),
                    this, SLOT(slotValueChanged(QtProperty *, const QVariant &)));
    }
}

BaseObjectController::~BaseObjectController()
{
    delete d_ptr;
}


void BaseObjectController::setObjects(QObjectList l)
{
    QList<QPointer<QObject>>     m_newobjects;
    foreach (auto o, l) {
        m_newobjects.push_back(QPointer<QObject>(o));
    }
    if (d_ptr->m_objects!=m_newobjects){
        foreach (QObject *o, d_ptr->m_objects) {
            o->disconnect(this);
        }
        if (d_ptr->m_objects.size()!=0) d_ptr->saveExpandedState();
        d_ptr->m_objects=m_newobjects;
        d_ptr->m_commonPropertyNames.clear();
        // список общих свойств
        QStringList commonPropertyes;
        if (l.size()>0) commonPropertyes=listPropertyes(l.first());
        for (int i=1;i<l.size();i++){
            QStringList l1=listPropertyes(l.at(i));
            foreach (QString s, commonPropertyes) {
                if (l1.indexOf(s)<0) commonPropertyes.removeOne(s);
            }
        }
        QListIterator<QtProperty *> it(d_ptr->m_topLevelProperties);
        while (it.hasNext()) {
            d_ptr->m_browser->removeProperty(it.next());
        }
        d_ptr->m_topLevelProperties.clear();

        d_ptr->m_propertyToDPropertyName.clear();
        d_ptr->m_DPropertyNameToproperty.clear();

        //    d_ptr->m_manager->clear();
        //    d_ptr->m_readOnlyManager->clear();
        //    d_ptr->m_quint64_manager->clear();
        //    d_ptr->m_ObjectLink_manager->clear();
        //    d_ptr->m_SignalDescription_manager->clear();

        d_ptr->m_classToIndexToProperty.clear();
        d_ptr->m_classToProperty.clear();
        d_ptr->m_propertyToClass.clear();
        d_ptr->m_propertyToIndex.clear();

        blockSignals(true);


        d_ptr->m_allProperties.clear();
        d_ptr->m_commonPropertyNames=commonPropertyes;
//        if (d_ptr->onlyState) {
//            if (d_ptr->m_objects.size()!=0) {
//                BaseObject *B=qobject_cast<BaseObject *>(d_ptr->m_objects.first());
//                d_ptr->addClassDynamicProperties(&B->STATES);
//            }
//        } else {
//            d_ptr->addProperties();
//        }
        d_ptr->addProperties();
        d_ptr->updateProperties();
        d_ptr->restoreExpandedState();
        foreach (QObject *o, l) {
            BaseObject *b=qobject_cast<BaseObject*>(o);
            if (b){
                connect(b,&BaseObject::propertyChanged,this,&BaseObjectController::propertyChanged);
                connect(b,&BaseObject::stateChanged,this,&BaseObjectController::propertyChanged);
            }
        }


        blockSignals(false);
    }
}

QObjectList BaseObjectController::objects() const
{
    QObjectList l;
    foreach (auto p, d_ptr->m_objects) {
        if (!p.isNull()) l.push_back(p.data());
    }

    return l;
}

void BaseObjectController::setOnlyState(bool p)
{
    d_ptr->onlyState=p;
}

int BaseObjectController::topLevelPropertiesCount()
{
    return  d_ptr->m_topLevelProperties.size();
}



void BaseObjectController::propertyChanged(QObject *object)
{
    if (d_ptr->m_objects.indexOf(object)>=0){
            d_ptr->updateProperties();
        }
}

#include "moc_baseobjectcontroller.cpp"
