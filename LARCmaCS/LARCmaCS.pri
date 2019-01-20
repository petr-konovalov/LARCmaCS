# Copyright 2019 Irina Khonakhbeeva

# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at

# http://www.apache.org/licenses/LICENSE-2.0

# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

#very strange thing
include(../macsCommon/macsCommon.pri)
INCLUDEPATH += $${SHARED_DIR}/vartypes

win32 {
  #add libs
  LIBS += -lws2_32
  LIBS += -L$${MATLAB_DIR}/lib/win$${BIT}/microsoft/ -llibeng \
                  -L$${MATLAB_DIR}/lib/win$${BIT}/microsoft/ -llibmat \
                  -L$${MATLAB_DIR}/lib/win$${BIT}/microsoft/ -llibmx
}

QT += core gui network

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
	$$PWD/sceneViewWorker.cpp \
	$$PWD/robot.cpp \
	$$PWD/receiverWorker.cpp \
	$$PWD/mainAlgWorker.cpp \
	$$PWD/connectorWorker.cpp \
	$$PWD/packetSSL.cpp \


HEADERS  += \
  $${SHARED_DIR}/net/netraw.h \
  $${SHARED_DIR}/net/robocup_ssl_client.h \
  $${SHARED_DIR}/net/Pipe.h \
  $${SHARED_DIR}/util/timer.h \
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
	$$PWD/sceneViewWorker.h \
	$$PWD/receiverWorker.h \
	$$PWD/robot.h \
	$$PWD/mainAlgWorker.h \
	$$PWD/connectorWorker.h \

FORMS += \
	$$PWD/larcmacs.ui \
	$$PWD/remotecontrol.ui \
	$$PWD/ipdialog.ui \
	$$PWD/reference.ui \
