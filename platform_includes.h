/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)
    Copyright (C) 2012 Pascal Spörri (pascal.spoerri@gmail.com)
    Copyright (C) 2012 Sabina Schellenberg (sabina.schellenberg@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#ifndef PLATFORM_INCLUDES_H
#define PLATFORM_INCLUDES_H

#define GLFW_INCLUDE_GL3
#define GLFW_NO_GLU
#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <inttypes.h>
#include <cassert>


#include <glm/glm.hpp> // brew install glm


#if defined(__APPLE__) || defined(__MACH__)
// for Mac
//#include <OpenGL/gl3.h>
//#include <OpenGL/glu.h>
#define BOOST_NO_CXX11_NUMERIC_LIMITS
#include <GL/glfw.h>

//#define abs(x) fabs(x)
//#include <>

#elif defined(__linux__) || defined(UNIX)
#include <GL/glew.h> // brew install glew

#include <GL/glx.h>
#include <GL/glfw.h>



#elif defined(_WIN32) || defined(_WIN64)
// for Windows
// No support, sorry
#endif

typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned short ushort;
#endif // PLATFORM_INCLUDES_H
