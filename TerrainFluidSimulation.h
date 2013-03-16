/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)
    Copyright (C) 2012 Pascal Spörri (pascal.spoerri@gmail.com)
    Copyright (C) 2012 Sabina Schellenberg (sabina.schellenberg@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#ifndef TERRAINFLUIDSIMULATION_H
#define TERRAINFLUIDSIMULATION_H

#include "Simulation/FluidSimulation.h"

#include "Graphics/Shader.h"
#include "Graphics/VertexBuffer.h"
#include "Graphics/IndexBuffer.h"
#include "Graphics/Texture2D.h"

#include "Camera.h"

#include "SimulationState.h"

#if defined(__APPLE__) || defined(__MACH__)
#include "osx_bundle.h"
#endif

#include <memory>

class TerrainFluidSimulation
{
public:
    TerrainFluidSimulation(uint dim=200);

    void Run();

    void Stop();

protected:

    /// Starts simulation main loop.
    void runMainloop();

    /// Checks and handles input events.
    void checkInput();

    /// Advances physics by timestep dt (in milliseconds).
    void updatePhysics(double dt);

    /// Renders the simulation
    void render();

    void cameraMovement(double dt);

    void init();

protected:
    bool _finished;

    bool _rain;
    bool _flood;
    glm::vec2 _rainPos;

    SimulationState _simulationState;
    Simulation::FluidSimulation _simulation;


    Graphics::ShaderManager             _shaderManager;
    Graphics::VertexBuffer<float>       _terrainHeightBuffer;
    Graphics::VertexBuffer<float>       _waterHeightBuffer;
    Graphics::VertexBuffer<glm::vec2>   _gridCoordBuffer;
    Graphics::IndexBuffer               _gridIndexBuffer;
    Graphics::VertexBuffer<float>       _sedimentBuffer;
    Graphics::VertexBuffer<glm::vec3>   _normalBuffer;


    Graphics::Texture2D<float,Graphics::TextureFormat::Float,32> _waterHeightTexture;

    Camera _cam;

    std::shared_ptr<Graphics::Shader> _testShader;

    int _width, _height;

};

#endif // TERRAINFLUIDSIMULATION_H
