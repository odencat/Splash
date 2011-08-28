DEPENDPATH += . ../Shared/jsoncpp
INCLUDEPATH += ../Shared/jsoncpp/include
TEMPLATE = lib
SOURCES += src/lib_json/json_reader.cpp src/lib_json/json_value.cpp src/lib_json/json_writer.cpp
HEADERS += include/json/autolink.h include/json/config.h include/json/forwards.h include/json/json.h include/json/reader.h include/json/value.h include/json/writer.h
