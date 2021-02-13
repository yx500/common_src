




QT+= network

INCLUDEPATH += $$PWD

HEADERS += \
    $$PWD/gtbuffers_memshared.h \
    $$PWD/signalsmanager.h \
    $$PWD/dmpacket2.h \
    $$PWD/q_gtnet.h \
    $$PWD/signalchanel.h \
    $$PWD/signaldescriptiontextinterface.h \
    $$PWD/gtbuffers_udp_d2.h \
    $$PWD/gtbuffers.h

SOURCES += \
    $$PWD/gtbuffers_memshared.cpp \
    $$PWD/signalsmanager.cpp \
    $$PWD/q_gtnet.cpp \
    $$PWD/signalchanel.cpp \ 
    $$PWD/signaldescriptiontextinterface.cpp \
    $$PWD/gtbuffers_udp_d2.cpp
    
