DEPENDPATH += . ../Shared/jsoncpp
INCLUDEPATH += ../Shared/jsoncpp/include
TEMPLATE = lib

HEADERS += \
    $$PWD/include/json/allocator.h \
    $$PWD/include/json/assertions.h \
    $$PWD/include/json/config.h \
    $$PWD/include/json/forwards.h \
    $$PWD/include/json/json.h \
    $$PWD/include/json/json_features.h \
    $$PWD/include/json/reader.h \
    $$PWD/include/json/value.h \
    $$PWD/include/json/version.h \
    $$PWD/include/json/writer.h \
    $$PWD/src/lib_json/json_tool.h \
    $$PWD/src/lib_json/json_valueiterator.inl

SOURCES += \
    $$PWD/src/lib_json/json_reader.cpp \
    $$PWD/src/lib_json/json_value.cpp \
    $$PWD/src/lib_json/json_writer.cpp
