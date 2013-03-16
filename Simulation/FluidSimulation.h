/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)
    Copyright (C) 2012 Pascal Spörri (pascal.spoerri@gmail.com)
    Copyright (C) 2012 Sabina Schellenberg (sabina.schellenberg@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#ifndef SIMULATION_H
#define SIMULATION_H

#include "platform_includes.h"
#include "Grid2D.h"
#include "SimulationState.h"

using namespace glm;

namespace Simulation {

class FluidSimulation
{

public:
    FluidSimulation(SimulationState& state);
    ~FluidSimulation();

    SimulationState& state;

    Grid2D<float>& water;
    Grid2D<float>& terrain;
    Grid2D<float>& sediment;
    Grid2D<float> tmpSediment;
    Grid2D<float> uVel;
    Grid2D<float> vVel;

    Grid2D<float> lFlux;
    Grid2D<float> rFlux;
    Grid2D<float> tFlux;
    Grid2D<float> bFlux;

    glm::vec2 rainPos;

    // lX and lY have to decrease if we increse the gridsize
    const float lX;
    const float lY;
    const float gravity;
    void update(double dt, bool makeRain=true, bool flood=false);
    void simulateFlow(double dt);
    void simulateErosion(double dt);
    void simulateSedimentTransportation(double dt);
    void simulateEvaporation(double dt);

    void makeRain(double dt);
    void makeFlood(double dt);

    void addRainDrop(const vec2& pos, int rad, float amount);

    void smoothTerrain();

    void computeSurfaceNormals();

    // flux access (takes care of boundaries)
    inline float getRFlux(int y, int x);
    inline float getLFlux(int y, int x);
    inline float getBFlux(int y, int x);
    inline float getTFlux(int y, int x);

    // terrain access
    inline float getTerrain(int y, int x);

    // water access
    inline float getWater(int y, int x);


};

}
#endif // SIMULATION_H
