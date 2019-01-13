#define path to directories with protobuf headers and libs

mingw {
  PROTO_DIR = $${MSYS_DIR}/mingw$${BIT}
  LIBS += -lprotobuf
  PROTOC_DIR =
  DLLS += $$MSYS_DIR/mingw$${BIT}/bin/libprotobuf.dll
}
msvc:!amd64: PROTO_DIR = $${VCPKG_DIR}/installed/x86-windows
msvc:amd64:  PROTO_DIR = $${VCPKG_DIR}/installed/x64-windows
msvc {
  LIBS += -L$${PROTO_DIR}/$${PREFIX_STR}lib/ -llibprotobuf$${SUFFIX_STR}
  PROTOC_DIR = $${PROTO_DIR}/tools/protobuf/
  DLLS += $$PROTO_DIR/bin/libprotobuf.dll
}

INCLUDEPATH += $${PROTO_DIR}/include
PROTO_GENERATED_DIR = $$PWD/generated/
INCLUDEPATH += $$PROTO_GENERATED_DIR
PROTOS = $$files($$PWD/*.proto)

