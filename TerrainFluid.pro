TEMPLATE = app
CONFIG += console
CONFIG -= qt

INCLUDEPATH += external/
QMAKE_CXXFLAGS += -std=c++11

SOURCES += *.cpp \
    Graphics/Shader.cpp \
    Graphics/GLWrapper.cpp \
    Simulation/FluidSimulation.cpp \
    Graphics/IndexBuffer.cpp \
    Math/PerlinNoise.cpp
HEADERS += *.h \
    Graphics/Shader.h \
    Graphics/GLWrapper.h \
    Simulation/FluidSimulation.h \
    SimulationState.h \
    Graphics/VertexBuffer.h \
    Graphics/IndexBuffer.h \
    Camera.h \
    Math/MathUtil.h \
    Graphics/Texture2D.h \
    Graphics/Mesh.h \
    Math/PerlinNoise.h \
    external/tclap/CmdLine.h

OTHER_FILES += \
    Resources/lambert_v.glsl \
    Resources/lambert_f.glsl

package_media.files += Resources/lambert_v.glsl
package_media.files += Resources/lambert_f.glsl

mac {
    INCLUDEPATH += /usr/local/include
    QMAKE_LFLAGS += -lglfw -framework Cocoa -framework OpenGL -framework IOKit
    LIBS+= -lboost_system-mt
    LIBS+= -lboost_filesystem-mt
    LIBS+= -lglew
    LIBS+= -L/usr/local/lib
    OBJECTIVE_SOURCES += external/osx_bundle.mm
    QMAKE_OBJECTIVE_CFLAGS += -stdlib=libc++
    QMAKE_CXXFLAGS += -stdlib=libc++
    QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.9
    QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64

    package_media.path = Contents/Resources
    QMAKE_BUNDLE_DATA += package_media
} else:unix {
    QMAKE_CXXFLAGS += -fopenmp
    QMAKE_LFLAGS += -fopenmp
    CONFIG    += link_pkgconfig
    PKGCONFIG += libglfw
    LIBS+=-lboost_system
    LIBS+=-lboost_filesystem
    LIBS+=-lGLEW
    LIBS+=-lGL

    copydata.commands = $(COPY_DIR) $$PWD/Resources $$OUT_PWD
    first.depends = $(first) copydata
    export(first.depends)
    export(copydata.commands)
    QMAKE_EXTRA_TARGETS += first copydata

}



