include(config.pri)
include(personal.pri)
#check personal info

MATLAB_DIR = "/usr/local/MATLAB/R2018b/"
MATLAB_BIN = "/usr/local/MATLAB/R2018b/bin/matlab"

msvc {
  isEmpty(VCPKG_DIR): error("To use MSVC you need to specify the path to VCPKG_DIR!")
}
mingw {
  isEmpty(MSYS_DIR): error("To use MINGW you need to specify the path to MSYS_DIR!")
}

isEmpty(MATLAB_DIR): error("You need to specify the path to MATLAB_DIR!")
include(proto/proto.pri)

SHARED_DIR = $$PWD/macsCommon

include (LARCmaCS/LARCmaCS.pri)


#Need this???
#where to place built objects
OBJECTS_DIR = build/LARCmaCS/obj$${SUFFIX_STR}

#where to place temporary moc sources
MOC_DIR = build/LARCmaCS/moc

#where to place auto-generated UI files
UI_DIR = build/LARCmaCS/ui

#where to place intermediate resource files
RCC_DIR = build/LARCmaCS/resources

defineTest(copyToDestdir) {
	files = $$1

	for(FILE, files) {
		DDIR = $$DESTDIR
		# Replace slashes in paths with backslashes for Windows
		win32:FILE ~= s,/,\\,g
		win32:DDIR ~= s,/,\\,g
		QMAKE_POST_LINK += $$QMAKE_COPY $$quote($$FILE) $$quote($$DDIR) $$escape_expand(\\n\\t)
	}

	export(QMAKE_POST_LINK)
}

INIT_FILES = \
        $$PWD/resources/LARCmaCS.exe.embed.manifest \
        $$PWD/resources/gamepads.txt \
        $$PWD/resources/help.png \
        $$PWD/resources/help.html

copyToDestdir($$INIT_FILES)

MATLAB_BIN = $${MATLAB_DIR}\..\bin\win$$BIT
msvc: OTHER_BIN = '$$[QT_INSTALL_BINS];$${PROTO_DIR}/$${PREFIX_STR}bin/'
mingw: OTHER_BIN = '$${MSYS_DIR}\mingw$${BIT}\bin'
QMAKE_SUBSTITUTES += LARCmaCS.cmd.in
OTHER_FILES += $$QMAKE_SUBSTITUTES

#DEPENDPATH += "/usr/lib/x86_64-linux-gnu/"
#INCLUDEPATH += "/usr/lib/x86_64-linux-gnu/"

LIBS += -lprotobuf

#DEPENDPATH += "/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/"
#INCLUDEPATH += "/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/"

#DEPENDPATH += "/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/extern/include/"
#INCLUDEPATH += "/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/extern/include/"
INCLUDEPATH += "/usr/local/MATLAB/R2018b/extern/include"

LIBS += -L/usr/local/MATLAB/R2018b/bin/glnxa64 -leng \
        -L/usr/local/MATLAB/R2018b/bin/glnxa64 -lmat \
        -L/usr/local/MATLAB/R2018b/bin/glnxa64 -lmx \
        -L/usr/local/MATLAB/R2018b/bin/glnxa64 -licudata


#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -leng
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmat
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmx
##LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwresource_core
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -licudata

#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_locale
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_date_time
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -ltbb
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -licuio
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_wserialization
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_prg_exec_monitor
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_chrono
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmat
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_filesystem
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lhdf5
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_context
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_container
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_serialization
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_unit_test_framework
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -leng
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwi18n
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_signals
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_system
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_log_setup
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_timer
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_atomic
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_coroutine
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwfl
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmx
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_program_options
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -ltbbmalloc
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwfoundation_usm
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_regex
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_log
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_wave
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_graph
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -licuuc
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_thread
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_random
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lut
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwboost_iostreams
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -licudata
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -lmwcpp11compat
#LIBS += -L/data/workspace/robocup/iakov/LARCmaCS/docker/tmp/R2018a/bin/glnxa64/ -licui18n
