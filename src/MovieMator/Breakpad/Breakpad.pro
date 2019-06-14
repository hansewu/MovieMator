#-------------------------------------------------
#
# Project created by QtCreator 2018-11-26T14:32:35
#
#-------------------------------------------------
CONFIG   += link_prl

TARGET = Breakpad
TEMPLATE = lib

DEFINES += BREAKPAD_LIBRARY
win32:DEFINES += QT_STATIC

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

mac {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@executable_path/
    QMAKE_LFLAGS	+= -framework Cocoa
    QMAKE_RPATHDIR += @executable_path/qt_lib/lib
}

win32 {
    target.path = C:\\Projects\\MovieMator
    INSTALLS += target
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

# breakpad
#INCLUDEPATH += $$PWD $$PWD/breakpad $$PWD/breakpad/src
INCLUDEPATH += $$PWD/breakpad/src

# Windows
win32{
   HEADERS +=    $$PWD/breakpad/src/common/windows/string_utils-inl.h \
                 $$PWD/breakpad/src/common/windows/guid_string.h \
                 $$PWD/breakpad/src/client/windows/handler/exception_handler.h \
                 $$PWD/breakpad/src/client/windows/common/ipc_protocol.h \
                 $$PWD/breakpad/src/google_breakpad/common/minidump_format.h \
                 $$PWD/breakpad/src/google_breakpad/common/breakpad_types.h \
                 $$PWD/breakpad/src/client/windows/crash_generation/crash_generation_client.h \
                 $$PWD/breakpad/src/common/scoped_ptr.h

    SOURCES +=  $$PWD/breakpad/src/client/windows/handler/exception_handler.cc \
                $$PWD/breakpad/src/common/windows/string_utils.cc \
                $$PWD/breakpad/src/common/windows/guid_string.cc \
                $$PWD/breakpad/src/client/windows/crash_generation/crash_generation_client.cc
}
# Unix
unix{
     HEADERS += \
            $$PWD/breakpad/src/client/minidump_file_writer-inl.h \
            $$PWD/breakpad/src/client/minidump_file_writer.h \
            $$PWD/breakpad/src/common/using_std_string.h \
            $$PWD/breakpad/src/common/memory.h \
            $$PWD/breakpad/src/common/basictypes.h \
            $$PWD/breakpad/src/common/memory_range.h \
            $$PWD/breakpad/src/common/string_conversion.h \
            $$PWD/breakpad/src/common/convert_UTF.h \
            $$PWD/breakpad/src/google_breakpad/common/minidump_format.h \
            $$PWD/breakpad/src/google_breakpad/common/minidump_size.h \
            $$PWD/breakpad/src/google_breakpad/common/breakpad_types.h \
            $$PWD/breakpad/src/common/scoped_ptr.h \
            $$PWD/breakpad/src/common/md5.h

    SOURCES += \
            $$PWD/breakpad/src/client/minidump_file_writer.cc \
            $$PWD/breakpad/src/common/string_conversion.cc \
            $$PWD/breakpad/src/common/convert_UTF.c \
            $$PWD/breakpad/src/common/md5.cc

    QMAKE_CXXFLAGS+=-g
}

mac{
HEADERS += \
        $$PWD/breakpad/src/client/mac/handler/exception_handler.h \
        $$PWD/breakpad/src/client/mac/handler/minidump_generator.h \
        $$PWD/breakpad/src/client/mac/handler/dynamic_images.h \
        $$PWD/breakpad/src/client/mac/handler/breakpad_nlist_64.h \
        $$PWD/breakpad/src/client/mac/crash_generation/crash_generation_client.h \
        $$PWD/breakpad/src/common/mac/bootstrap_compat.h \
        $$PWD/breakpad/src/common/mac/macho_utilities.h \
        $$PWD/breakpad/src/common/mac/scoped_task_suspend-inl.h \
        $$PWD/breakpad/src/common/mac/file_id.h \
        $$PWD/breakpad/src/common/mac/macho_id.h \
        $$PWD/breakpad/src/common/mac/macho_id.h \
        $$PWD/breakpad/src/common/mac/macho_walker.h \
        $$PWD/breakpad/src/common/mac/string_utilities.h \
        $$PWD/breakpad/src/google_breakpad/common/minidump_exception_mac.h \

SOURCES += \
        $$PWD/breakpad/src/client/mac/handler/exception_handler.cc \
        $$PWD/breakpad/src/client/mac/handler/minidump_generator.cc \
        $$PWD/breakpad/src/client/mac/handler/dynamic_images.cc \
        $$PWD/breakpad/src/client/mac/crash_generation/crash_generation_client.cc \
        $$PWD/breakpad/src/client/mac/handler/breakpad_nlist_64.cc \
        $$PWD/breakpad/src/common/mac/file_id.cc \
        $$PWD/breakpad/src/common/mac/macho_utilities.cc \
        $$PWD/breakpad/src/common/mac/macho_id.cc \
        $$PWD/breakpad/src/common/mac/macho_reader.cc \
        $$PWD/breakpad/src/common/mac/string_utilities.cc \
        $$PWD/breakpad/src/common/mac/bootstrap_compat.cc \
        $$PWD/breakpad/src/common/mac/macho_walker.cc \

OBJECTIVE_HEADERS += \
        $$PWD/breakpad/src/common/mac/MachIPC.h \

OBJECTIVE_SOURCES += \
        $$PWD/breakpad/src/common/mac/MachIPC.mm \
}
