#ifndef WDGPROPERTYESBASEOBJECT_H
#define WDGPROPERTYESBASEOBJECT_H

#include <QFrame>

class BaseObjectController;
class MVP_ObjectFactory;
namespace Ui {
class wdgPropertyesBaseObject;
}

class wdgPropertyesBaseObject : public QFrame
{
    Q_OBJECT

public:
    explicit wdgPropertyesBaseObject(QWidget *parent,MVP_ObjectFactory *MVP_OF,QObject *root4id);
    virtual ~wdgPropertyesBaseObject();
    BaseObjectController *theController;

    void setObjects(QObjectList l);
    QObjectList objects() const;

private:
    Ui::wdgPropertyesBaseObject *ui;
    MVP_ObjectFactory *MVP_OF;
};

#endif // WDGPROPERTYESBASEOBJECT_H
