#very strange thing
include(../macsCommon/macsCommon.pri)
INCLUDEPATH+=$${SHARED_DIR}/vartypes

win32 {
  #add libs
  LIBS += -lws2_32
  LIBS += -L$${MATLAB_DIR}/lib/win$${BIT}/microsoft/ -llibeng \
          -L$${MATLAB_DIR}/lib/win$${BIT}/microsoft/ -llibmat \
          -L$${MATLAB_DIR}/lib/win$${BIT}/microsoft/ -llibmx
}

QT       += core gui
QT       += network


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LARCmaCS
TEMPLATE = app


#directories of sources of the vision client
INCLUDEPATH += \
  $${SHARED_DIR}/net \
  $${SHARED_DIR}/util \
  $${SHARED_DIR}/rfprotocol \
  $${MATLAB_DIR}/include \

SOURCES +=  \
  $${SHARED_DIR}/net/netraw.cpp \
  $${SHARED_DIR}/net/robocup_ssl_client.cpp \

SOURCES += \
    $$PWD/main.cpp \
    $$PWD/larcmacs.cpp \
    $$PWD/fieldScene.cpp \
    $$PWD/sceneView.cpp \
    $$PWD/receiver.cpp \
    $$PWD/mainAlg.cpp \
    $$PWD/mlData.cpp \
    $$PWD/connector.cpp \
    $$PWD/remotecontrol.cpp \
    $$PWD/message.cpp \
    $$PWD/ipdialog.cpp \
    $$PWD/client.cpp \
    $$PWD/reference.cpp \


HEADERS  += \
  $${SHARED_DIR}/net/netraw.h \
  $${SHARED_DIR}/net/robocup_ssl_client.h \
  $${SHARED_DIR}/net/Pipe.h \
  $${SHARED_DIR}/util/timer.h \
  $${SHARED_DIR}/util/field.h \
  $${SHARED_DIR}/util/field_default_constants.h \
  $${SHARED_DIR}/rfprotocol/rfprotocol.h \
    $$PWD/larcmacs.h \
    $$PWD/fieldScene.h \
    $$PWD/sceneView.h \
    $$PWD/packetSSL.h \
    $$PWD/receiver.h \
    $$PWD/mainAlg.h \
    $$PWD/mlData.h \
    $$PWD/connector.h \
    $$PWD/remotecontrol.h \
    $$PWD/message.h \
    $$PWD/ipdialog.h \
    $$PWD/client.h \
    $$PWD/reference.h \

FORMS += \
    $$PWD/larcmacs.ui \
    $$PWD/remotecontrol.ui \
    $$PWD/ipdialog.ui \
    $$PWD/reference.ui \
