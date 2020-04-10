


INCLUDEPATH +=$$MVP_ROOT/../_ext/qt-solutions-master/qtpropertybrowser/src \
                $$PWD/


HEADERS += \
    $$PWD/baseobjectcontroller.h \
    $$PWD/objectlinkpropertymanager.h \
    $$PWD/objectlinkeditorfactory.h \
    $$PWD/int64propertymanager.h \
    $$PWD/signaldescriptionpropertymanager.h \
    $$PWD/int64editorfactory.h\
    $$PWD/wdgpropertyinspector.h \
    $$PWD/wdgpropertyesbaseobject.h

SOURCES += \
    $$PWD/baseobjectcontroller.cpp \
    $$PWD/objectlinkpropertymanager.cpp \
    $$PWD/objectlinkeditorfactory.cpp\
    $$PWD/int64propertymanager.cpp \
    $$PWD/signaldescriptionpropertymanager.cpp \
    $$PWD/int64editorfactory.cpp \
    $$PWD/wdgpropertyinspector.cpp \
    $$PWD/wdgpropertyesbaseobject.cpp

FORMS    += $$PWD/wdgpropertyinspector.ui \
            $$PWD/wdgpropertyesbaseobject.ui

LIBPATH=$$MVP_ROOT/libs
CONFIG(debug, debug|release) {
    win32: LIBS += -L$$LIBPATH -lpropertyinspectord1
    unix: LIBS += -L$$LIBPATH -L. -lpropertyinspectord -Wl,-rpath,$$LIBPATH -Wl,-rpath,.

} else {
    win32: LIBS += -L$$LIBPATH -lpropertyinspector1
    unix: LIBS += -L$$LIBPATH -L. -lpropertyinspector -Wl,-rpath,$$LIBPATH -Wl,-rpath,.

}
