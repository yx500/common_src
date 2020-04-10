#ifndef WDGPROPERTYINSPECTOR_H
#define WDGPROPERTYINSPECTOR_H

#include <QDockWidget>
#include "wdgpropertyesbaseobject.h"
class ObjectController;
namespace Ui {
class wdgPropertyInspector;
}

class wdgPropertyInspector : public QDockWidget
{
    Q_OBJECT

public:
    wdgPropertyesBaseObject *wdgModelController;
    wdgPropertyesBaseObject *wdgVisualController;

    explicit wdgPropertyInspector(QWidget *parent, bool onlyState=false);
    virtual ~wdgPropertyInspector();

    void setObjects(QObjectList l);

private:
    Ui::wdgPropertyInspector *ui;

public slots:
    void activeObjectsChanged(QObjectList l);
    void objectDeleting(QObject *O);

};

#endif // WDGPROPERTYINSPECTOR_H
