TEMPLATE = app
CONFIG += console
CONFIG -= qt

INCLUDEPATH += external/
QMAKE_CXXFLAGS += -std=c++0x

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

QMAKE_CXXFLAGS += -fopenmp
QMAKE_LFLAGS += -fopenmp
mac {
    QMAKE_LFLAGS += -lglfw -framework Cocoa -framework OpenGL -framework IOKit -framework OpenCL
    LIBS+= -lboost_system-mt #-L/usr/local/lib/ -I/usr/local/include/
    LIBS+= -lboost_filesystem-mt
    LIBS+= -lglew
    SOURCES += external/osx_bundle.mm
    QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
    QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64

    package_media.path = Contents/Resources
    QMAKE_BUNDLE_DATA += package_media
} else:unix {
    CONFIG    += link_pkgconfig
    PKGCONFIG += libglfw
    LIBS+=-lboost_system
    LIBS+=-lboost_filesystem
    LIBS+=-lGLEW
}



