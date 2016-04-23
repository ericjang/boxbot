TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += c++11

PROTOBUF_DIR = /contrib/projects/protobuf # installation location of protocol buffers
BOX2D_DIR=/contrib/projects/liquidfun/liquidfun/Box2D
GRPC_DIR=/contrib/projects/grpc

MODE=Debug # Release

INCLUDEPATH += src test
INCLUDEPATH += build # so *.grpc.pb.h can find find *.pb.h
INCLUDEPATH += $$BOX2D_DIR
INCLUDEPATH += $${PROTOBUF_DIR}/include
INCLUDEPATH += $${GRPC_DIR}/include

# TMP

LIBS += -L$$BOX2D_DIR/Box2D/Debug -lliquidfun
LIBS += -L$$BOX2D_DIR/glui/Debug -lglui
LIBS += -L$$BOX2D_DIR/freeglut/Debug -lglut
LIBS += -lGL  # OpenGL
LIBS += -lGLU # freeglut
LIBS += -Lpng # writing PNG files is much smaller
LIBS += -L$${PROTOBUF_DIR}/lib -lprotobuf
LIBS += -L$${GRPC_DIR}/lib -lgpr -lgrpc++_unsecure -lgrpc -lpthread -ldl

## PROTOBUF STUFF

HEADERS += \
    build/boxbot.pb.h \
    build/boxbot.grpc.pb.h \
    build/helloworld.pb.h \
    build/helloworld.grpc.pb.h \
    test/greetertest.h \
    src/policy/rpcclientpolicy.h \
    src/simserver.h \
    src/simapp.h

SOURCES += \
    build/boxbot.pb.cc \
    build/boxbot.grpc.pb.cc \
    build/helloworld.pb.cc \
    build/helloworld.grpc.pb.cc \
    test/greetertest.cpp \
    src/policy/rpcclientpolicy.cpp \
    src/simserver.cpp \
    src/simapp.cpp

# OTHER STUFF
HEADERS += include/boxbot.h \
    src/agents/agentfactory.h \
    src/envfactory.h \
    src/sim.h \
    src/debugdraw.h \
    src/agents/agent.h \
    src/box2dhelper.h \
    src/agents/multiagent.h \
    src/mathutils.h \
    src/policy/policy.h \
    src/policy/testpolicy.h

SOURCES += \
    src/agents/agentfactory.cpp \
    src/envfactory.cpp \
    src/main.cpp \
    src/sim.cpp \
    src/debugdraw.cpp \
    src/agents/agent.cpp \
    src/box2dhelper.cpp \
    src/agents/multiagent.cpp \
    src/mathutils.cpp \
    src/policy/policy.cpp \
    src/policy/testpolicy.cpp \

OTHER_FILES += protobuf/boxbot.proto \
               README.md
