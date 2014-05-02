/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)
    Copyright (C) 2012 Pascal Spörri (pascal.spoerri@gmail.com)
    Copyright (C) 2012 Sabina Schellenberg (sabina.schellenberg@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#include <iostream>
#include <stdlib.h>

#include "tclap/CmdLine.h"
#include "platform_includes.h"

#include "TerrainFluidSimulation.h"

using namespace std;

TerrainFluidSimulation* simulationPtr = 0;

int onWindowClose()
{
    if (simulationPtr) simulationPtr->Stop();
    return GL_TRUE;
}

int main(int argc, char** argv)
{
    // Settings ////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////

    int windowWidth = 800;
    int windowHeight = 600;
    uint terrainDim = 300;

    // Read Command Line Arguments /////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////
    try
    {
        TCLAP::CmdLine cmd("Terrain Eroision & Fluid Simulation.", ' ', "0.9");
        TCLAP::ValueArg<uint> dimArg("d","dim","Size of the terrain. Default: 300.",false,300,"uint");
        cmd.add(dimArg);
        cmd.parse( argc, argv );
        terrainDim = dimArg.getValue();
    }
    catch (TCLAP::ArgException &e)
    {
        std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
    }


    // Open GL Stuff ///////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////

    // init GLFW
    if (!glfwInit())
    {
        std::cerr << "[GLFW] Error initialising GLFW" << std::endl;
        exit(1);
    }


    glfwOpenWindowHint(GLFW_FSAA_SAMPLES, 4); // 4x antialiasing
    // Use OpenGL Core v3.2
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MAJOR, 3);
    glfwOpenWindowHint(GLFW_OPENGL_VERSION_MINOR, 2);
    glfwOpenWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwOpenWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    // Open an OpenGL window
    if( !glfwOpenWindow( windowWidth,windowHeight, 8,8,8,8,8,8, GLFW_WINDOW ) )
    {
        std::cerr << "[GLFW] Error opening window" << std::endl;
        glfwTerminate();
        exit(1);
    }

    int major, minor, rev;

    glfwGetGLVersion(&major, &minor, &rev);

    fprintf(stdout, "OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
    // Init Glew (OpenGL Extension Loading)

#if defined(__APPLE__) || defined(__MACH__)
    // Do nothing
#else
    // http://stackoverflow.com/questions/8302625/segmentation-fault-at-glgenvertexarrays-1-vao
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err)
    {
        std::cerr << "[GLEW] Init failed: " << glewGetErrorString(err) << std::endl;
        glfwTerminate();
        exit(1);
    }
#endif
    // Start Simulation ////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////

    glfwSetWindowCloseCallback(onWindowClose);

    simulationPtr = new TerrainFluidSimulation(terrainDim);
    simulationPtr->Run();

    delete simulationPtr;
    cout << "The end." << endl;
    return 0;
}





