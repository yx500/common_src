TEMPLATE=lib
CONFIG += qt dll qtpropertybrowser-buildlib

# Версия библиотеки.
VERSION = 1.0.0
DESTDIR = $$PWD/../../libs

# Если операционная система из семейства unix
unix: {
                CONFIG (debug, debug|release) {
                        # Такое название имеет debug-версия библиотеки
                        TARGET = propertyinspectord
                } else {
                        # А такое release-версия
                        TARGET = propertyinspector
                }
} else {
                #TARGET = $$qtLibraryTarget(propertyinspector)
                CONFIG (debug, debug|release) {
                        # Такое название имеет debug-версия библиотеки
                        TARGET = propertyinspectord
                } else {
                        # А такое release-версия
                        TARGET = propertyinspector
                }
}

#win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release build_all


include(../../../_ext/qt-solutions-master/qtpropertybrowser/src/qtpropertybrowser.pri)

#INCLUDEPATH += $$PWD/../../mvp

##TARGET = $$QTPROPERTYBROWSER_LIBNAME
##DESTDIR = $$QTPROPERTYBROWSER_LIBDIR
#win32 {
#    DLLDESTDIR = $$[QT_INSTALL_BINS]
#    QMAKE_DISTCLEAN += $$[QT_INSTALL_BINS]\\$${QTPROPERTYBROWSER_LIBNAME}.dll
#}
#target.path = $$DESTDIR
#INSTALLS += target

unix {
    target.path = /usr/lib
    INSTALLS += target
}

FORMS +=


