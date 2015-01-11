/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)
    Copyright (C) 2012 Pascal Spörri (pascal.spoerri@gmail.com)
    Copyright (C) 2012 Sabina Schellenberg (sabina.schellenberg@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#include "TerrainFluidSimulation.h"

#include <chrono>
#include <iostream>

#include "platform_includes.h"

using namespace glm;
using namespace Graphics;


TerrainFluidSimulation::TerrainFluidSimulation(uint dim)
    : _simulationState(dim,dim),
      _simulation(_simulationState),
      _rain(false),
      _rainPos(dim/2,dim/2),
      _flood(false)
{}

void TerrainFluidSimulation::Run()
{
    init();
    runMainloop();
}

void TerrainFluidSimulation::Stop()
{
    _finished = true;
}

void TerrainFluidSimulation::runMainloop()
{
    using namespace std::chrono;

    // Settings
    double dt = 1000.0/(60); // 60 fps physics simulation

    // Setup
    high_resolution_clock clock;
    high_resolution_clock::time_point currentTime, newTime;
    high_resolution_clock::duration frameTime;
    high_resolution_clock::duration accumulator(0);

    // count number of simulation steps
    ulong counterSim = 0;

    _finished = false;
    currentTime = clock.now();
    while(!_finished)
    {
        newTime = clock.now();
        frameTime = newTime - currentTime;
        currentTime = newTime;
        accumulator += frameTime;

        // physics simulation
        updatePhysics(dt);

        // check for input events
        glfwPollEvents();
        cameraMovement(dt);

        // timing info
        counterSim++;
        if (counterSim%100 == 0)
        {
            double ms = std::chrono::duration_cast<milliseconds>(accumulator).count()/100.0;
            std::cout << 1000.0 /ms << " FPS\n";
            accumulator = high_resolution_clock::duration(0);
            counterSim = 0;
        }


        // input handling
        checkInput();

        // check for shader change
#if defined(__APPLE__) || defined(__MACH__)

#else
        _shaderManager.Update();
#endif
        // rendering
        render();
    }

}

void TerrainFluidSimulation::checkInput()
{
    // exit
    if (glfwGetKey(GLFW_KEY_ESC))
    {
        _finished = true;
    }

    if (glfwGetKey('O')) _rain = true;
    if (glfwGetKey('P')) _rain = false;

    if (glfwGetKey('K')) _flood = true;
    if (glfwGetKey('L')) _flood = false;


    // move rain position
    float d = 1.0f;
    if (glfwGetKey(GLFW_KEY_UP)) _rainPos.y += d;
    if (glfwGetKey(GLFW_KEY_DOWN)) _rainPos.y -= d;
    if (glfwGetKey(GLFW_KEY_RIGHT)) _rainPos.x += d;
    if (glfwGetKey(GLFW_KEY_LEFT)) _rainPos.x -= d;

    _simulation.rainPos = _rainPos;
}

void TerrainFluidSimulation::cameraMovement(double dt)
{
    float dtSeconds = dt/1000.0f;

    // camera control
    float camSpeed = 1.0f*dtSeconds;
    float rotSpeed = 60.0f*dtSeconds;

    vec3 yAxis(0,1,0);
    vec3 xAxis(1,0,0);

    if (glfwGetKey('D')) _cam.TranslateLocal(vec3(camSpeed,0,0));
    if (glfwGetKey('A')) _cam.TranslateLocal(vec3(-camSpeed,0,0));

    if (glfwGetKey('R')) _cam.TranslateLocal(vec3(0,camSpeed,0));
    if (glfwGetKey('F')) _cam.TranslateLocal(vec3(0,-camSpeed,0));

    if (glfwGetKey('W')) _cam.TranslateLocal(vec3(0,0,-camSpeed));
    if (glfwGetKey('S')) _cam.TranslateLocal(vec3(0,0,camSpeed));

    if (glfwGetKey('Q')) _cam.GlobalRotate(yAxis,-rotSpeed);
    if (glfwGetKey('E')) _cam.GlobalRotate(yAxis,rotSpeed);

    if (glfwGetKey('T')) _cam.LocalRotate(xAxis,-rotSpeed);
    if (glfwGetKey('G')) _cam.LocalRotate(xAxis,rotSpeed);
}

void TerrainFluidSimulation::updatePhysics(double dt)
{
    // Run simulation
    _simulation.update(dt,_rain,_flood);

    // Copy data to GPU
    _terrainHeightBuffer.SetData(_simulationState.terrain);
    _waterHeightBuffer.SetData(_simulationState.water);
    _sedimentBuffer.SetData(_simulationState.suspendedSediment);
    _normalBuffer.SetData(_simulationState.surfaceNormals);

}

void TerrainFluidSimulation::render()
{
    // Resize
    int w,h;
    bool resize = false;
    glfwGetWindowSize(&w,&h);
    if (w != _width)
    {
        _width = w;
        resize = true;
    }
    if (h != _height)
    {
        _height = h;
        resize = true;
    }
    if (resize)
    {
        glViewport(0,0,_width,_height);
        _cam.SetAspectRatio(float(_width)/float(_height));
    }

    // clear buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // bind shader
    _testShader->Bind();

    auto viewMatrix = _cam.ViewMatrix();
    _testShader->SetUniform("uProjMatrix", _cam.ProjMatrix());
    _testShader->SetUniform("uViewMatrix",viewMatrix);
    _testShader->SetUniform("uViewMatrixNormal", transpose(inverse(viewMatrix)) );
    _testShader->SetUniform("uGridSize",(int)_simulationState.terrain.width());

    // bind data
    _gridCoordBuffer.MapData(_testShader->AttributeLocation("inGridCoord"));
    _terrainHeightBuffer.MapData(_testShader->AttributeLocation("inTerrainHeight"));
    _waterHeightBuffer.MapData(_testShader->AttributeLocation("inWaterHeight"));
    _sedimentBuffer.MapData(_testShader->AttributeLocation("inSediment"));
    _normalBuffer.MapData(_testShader->AttributeLocation("inNormal"));

    _gridIndexBuffer.Bind();

    // render terrain
    _testShader->SetUniform("uColor", vec4(242.0/255.0,224.0/255.0,201.0/255.0,1));
    _testShader->SetUniform("uIsWater",false);
    glDrawElements(GL_TRIANGLES, _gridIndexBuffer.IndexCount(), _gridIndexBuffer.IndexType(),0);

    // unbind shader
    _testShader->UnBind();

    // finish
    glfwSwapBuffers();
    glFinish();
}

void TerrainFluidSimulation::init()
{
    // Settings
#if defined(__APPLE__) || defined(__MACH__)
    std::string resourcePath = osx_GetBundleResourcesPath()+"/";
#else
    std::string resourcePath("Resources/");
#endif

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create some data
    Grid2D<vec2> gridCoords;
    std::vector<uint> gridIndices;

    uint dimX = _simulationState.terrain.width();
    uint dimY = _simulationState.terrain.height();

    Grid2DHelper::MakeGridIndices(gridIndices,dimX,dimY);
    Grid2DHelper::MakeUniformGrid(gridCoords,dimX,dimY);

    // Send data to the GPU
    _gridIndexBuffer.SetData(gridIndices);
    _gridCoordBuffer.SetData(gridCoords);

    _terrainHeightBuffer.SetData(_simulationState.terrain);
    _waterHeightBuffer.SetData(_simulationState.water);
    _sedimentBuffer.SetData(_simulationState.suspendedSediment);

    // Load and configure shaders
    _testShader = _shaderManager.LoadShader(resourcePath+"lambert_v.glsl",resourcePath+"lambert_f.glsl");
    _testShader->MapAttribute("inGridCoord",0);
    _testShader->MapAttribute("inTerrainHeight",1);
    _testShader->MapAttribute("inWaterHeight",2);
    _testShader->MapAttribute("inSediment",3);
    _testShader->MapAttribute("inNormal",7);

    // position camera
    _cam.TranslateGlobal(vec3(0.0f,0.2,2));

    // OpenGL Settings
    glClearColor(0.4f,0.4f,0.4f,0.0f);
    //glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
//    glFrontFace(GL_CW); // clockwise
//    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    
    _terrainHeightBuffer.SetData(_simulationState.terrain);
    _waterHeightBuffer.SetData(_simulationState.water);
    _sedimentBuffer.SetData(_simulationState.suspendedSediment);
    _normalBuffer.SetData(_simulationState.surfaceNormals);
    
}


