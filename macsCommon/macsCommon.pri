# Created by and for Qt Creator This file was created for editing the project sources only.
# You may attempt to use it for building too, by modifying this file here.

#TARGET = macsCommon.pri

#HEADERS = \
#   $$PWD/capture/capture_generator.h \
#   $$PWD/capture/capturedc1394v2.h \
#   $$PWD/capture/capturefromfile.h \
#   $$PWD/capture/captureinterface.h \
#   $$PWD/capture/captureopencv.h \
#   $$PWD/cmpattern/cmpattern_pattern.h \
#   $$PWD/cmpattern/cmpattern_team.h \
#   $$PWD/cmpattern/cmpattern_teamdetector.h \
#   $$PWD/cmvision/cmvision_histogram.h \
#   $$PWD/cmvision/cmvision_region.h \
#   $$PWD/cmvision/cmvision_threshold.h \
#   $$PWD/gl/glcamera.h \
#   $$PWD/gl/globject.h \
#   $$PWD/net/netraw.h \
#   $$PWD/net/Pipe.h \
#   $$PWD/net/robocup_ssl_client.h \
#   $$PWD/net/robocup_ssl_server.h \
#   $$PWD/rfprotocol/rfprotocol.h \
#   $$PWD/util/affinity_manager.h \
#   $$PWD/util/bbox.h \
#   $$PWD/util/bitflags.h \
#   $$PWD/util/camera_calibration.h \
#   $$PWD/util/colors.h \
#   $$PWD/util/conversions.h \
#   $$PWD/util/field.h \
#   $$PWD/util/field_default_constants.h \
#   $$PWD/util/field_filter.h \
#   $$PWD/util/font.h \
#   $$PWD/util/framecounter.h \
#   $$PWD/util/framelimiter.h \
#   $$PWD/util/geomalgo.h \
#   $$PWD/util/geometry.h \
#   $$PWD/util/global_random.h \
#   $$PWD/util/gvector.h \
#   $$PWD/util/halffield.h \
#   $$PWD/util/image.h \
#   $$PWD/util/image_interface.h \
#   $$PWD/util/image_io.h \
#   $$PWD/util/lut3d.h \
#   $$PWD/util/nkdtree.h \
#   $$PWD/util/nvector.h \
#   $$PWD/util/pixelloc.h \
#   $$PWD/util/pose.h \
#   $$PWD/util/qgetopt.h \
#   $$PWD/util/quaternion.h \
#   $$PWD/util/random.h \
#   $$PWD/util/range.h \
#   $$PWD/util/rawimage.h \
#   $$PWD/util/ringbuffer.h \
#   $$PWD/util/sobel.h \
#   $$PWD/util/texture.h \
#   $$PWD/util/timer.h \
#   $$PWD/util/util.h \
#   $$PWD/util/vis_util.h \
#   $$PWD/util/zoom.h \
#   $$PWD/vartypes/enhanced/TimeLine.h \
#   $$PWD/vartypes/enhanced/TimeVar.h \
#   $$PWD/vartypes/gui/experimental/GraphScene.h \
#   $$PWD/vartypes/gui/experimental/TimeControlWidget.h \
#   $$PWD/vartypes/gui/experimental/TimeGraphContainer.h \
#   $$PWD/vartypes/gui/experimental/ValueVis.h \
#   $$PWD/vartypes/gui/experimental/VisContainer.h \
#   $$PWD/vartypes/gui/VarItem.h \
#   $$PWD/vartypes/gui/VarItemDelegate.h \
#   $$PWD/vartypes/gui/VarTreeModel.h \
#   $$PWD/vartypes/gui/VarTreeView.h \
#   $$PWD/vartypes/gui/VarTreeViewOptions.h \

HEADERS += \
   $$PWD/vartypes/primitives/VarBlob.h \
   $$PWD/vartypes/primitives/VarBool.h \
   $$PWD/vartypes/primitives/VarBoolVal.h \
   $$PWD/vartypes/primitives/VarDouble.h \
   $$PWD/vartypes/primitives/VarDoubleVal.h \
   $$PWD/vartypes/primitives/VarExternal.h \
   $$PWD/vartypes/primitives/VarInt.h \
   $$PWD/vartypes/primitives/VarIntVal.h \
   $$PWD/vartypes/primitives/VarList.h \
   $$PWD/vartypes/primitives/VarProtoBuffer.h \
   $$PWD/vartypes/primitives/VarProtoBufferVal.h \
   $$PWD/vartypes/primitives/VarQWidget.h \
   $$PWD/vartypes/primitives/VarSelection.h \
   $$PWD/vartypes/primitives/VarString.h \
   $$PWD/vartypes/primitives/VarStringEnum.h \
   $$PWD/vartypes/primitives/VarStringVal.h \
   $$PWD/vartypes/primitives/VarTrigger.h \
   $$PWD/vartypes/primitives/VarType.h \
   $$PWD/vartypes/primitives/VarVal.h \
   $$PWD/vartypes/xml/xmlParser.h \
   $$PWD/vartypes/VarBase64.h \
   $$PWD/vartypes/VarNotifier.h \
   $$PWD/vartypes/VarTypes.h \
   $$PWD/vartypes/VarTypesFactory.h \
   $$PWD/vartypes/VarTypesInstance.h \
   $$PWD/vartypes/VarXML.h

#HEADERS += \
#   $$PWD/net/netraw.h \
#   $$PWD/net/Pipe.h \
#   $$PWD/net/robocup_ssl_client.h \
#   $$PWD/util/field.h \
#   $$PWD/util/field_default_constants.h \

#SOURCES += \
#   $$PWD/net/netraw.cpp \
#   $$PWD/net/robocup_ssl_client.cpp \
#   $$PWD/net/robocup_ssl_server.cpp \
#   $$PWD/util/affinity_manager.cpp \
#   $$PWD/util/camera_calibration.cpp \
#   $$PWD/util/conversions.cpp \
#   $$PWD/util/framelimiter.cpp \
#   $$PWD/util/global_random.cpp \
#   $$PWD/util/image.cpp \
#   $$PWD/util/image_io.cpp \
#   $$PWD/util/lut3d.cpp \
#   $$PWD/util/qgetopt.cpp \
#   $$PWD/util/random.cpp \
#   $$PWD/util/rawimage.cpp \
#   $$PWD/util/ringbuffer.cpp \
#   $$PWD/util/texture.cpp \
#   $$PWD/vartypes/enhanced/TimeLine.cpp \
#   $$PWD/vartypes/enhanced/TimeVar.cpp \
#   $$PWD/vartypes/gui/experimental/GraphScene.cpp \
#   $$PWD/vartypes/gui/experimental/TimeControlWidget.cpp \
#   $$PWD/vartypes/gui/experimental/TimeGraphContainer.cpp \
#   $$PWD/vartypes/gui/experimental/ValueVis.cpp \
#   $$PWD/vartypes/gui/experimental/VisContainer.cpp \
#   $$PWD/vartypes/gui/VarItem.cpp \
#   $$PWD/vartypes/gui/VarItemDelegate.cpp \
#   $$PWD/vartypes/gui/VarTreeModel.cpp \
#   $$PWD/vartypes/gui/VarTreeView.cpp \
#   $$PWD/vartypes/gui/VarTreeViewOptions.cpp \
#
#

SOURCES += \
   $$PWD/vartypes/primitives/VarExternal.cpp \
   $$PWD/vartypes/primitives/VarBlob.cpp \
   $$PWD/vartypes/primitives/VarBool.cpp \
   $$PWD/vartypes/primitives/VarBoolVal.cpp \
   $$PWD/vartypes/primitives/VarDouble.cpp \
   $$PWD/vartypes/primitives/VarDoubleVal.cpp \
   $$PWD/vartypes/primitives/VarInt.cpp \
   $$PWD/vartypes/primitives/VarIntVal.cpp \
   $$PWD/vartypes/primitives/VarList.cpp \
   $$PWD/vartypes/primitives/VarProtoBuffer.cpp \
   $$PWD/vartypes/primitives/VarProtoBufferVal.cpp \
   $$PWD/vartypes/primitives/VarQWidget.cpp \
   $$PWD/vartypes/primitives/VarSelection.cpp \
   $$PWD/vartypes/primitives/VarString.cpp \
   $$PWD/vartypes/primitives/VarStringEnum.cpp \
   $$PWD/vartypes/primitives/VarStringVal.cpp \
   $$PWD/vartypes/primitives/VarTrigger.cpp \
   $$PWD/vartypes/primitives/VarType.cpp \
   $$PWD/vartypes/primitives/VarVal.cpp \
   $$PWD/vartypes/VarBase64.cpp \
   $$PWD/vartypes/VarNotifier.cpp \
   $$PWD/vartypes/VarTypes.cpp \
   $$PWD/vartypes/VarTypesFactory.cpp \
   $$PWD/vartypes/VarTypesInstance.cpp \
   $$PWD/vartypes/xml/xmlParser.cpp \
   $$PWD/vartypes/VarXML.cpp

INCLUDEPATH += \
    $$PWD/capture \
    $$PWD/cmpattern \
    $$PWD/cmvision \
    $$PWD/gl \
    $$PWD/net \
    $$PWD/rfprotocol \
    $$PWD/util \
    $$PWD/vartypes \
    $$PWD/vartypes/enhanced \
    $$PWD/vartypes/gui \
    $$PWD/vartypes/gui/experimental \
    $$PWD/vartypes/primitives \
    $$PWD/vartypes/xml

#DEFINES =

