/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)
    Copyright (C) 2012 Pascal Spörri (pascal.spoerri@gmail.com)
    Copyright (C) 2012 Sabina Schellenberg (sabina.schellenberg@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#include "FluidSimulation.h"

#include <random>
typedef std::mt19937 RANDOM;  // the Mersenne Twister with a popular choice of parameters

#if defined(__GNUG__)
#include "Math/MathUtil.h"
#else
#include "MathUtil.h"
#endif

#if defined(__APPLE__) || defined(__MACH__)
#include <dispatch/dispatch.h>
dispatch_queue_t gcdq = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
#endif

using namespace Simulation;
using namespace glm;
using namespace std;

FluidSimulation::FluidSimulation(SimulationState& state)
    : state(state),
      water(state.water),
      terrain(state.terrain),
      sediment(state.suspendedSediment),
      tmpSediment(state.water.width(),state.water.height()),
      uVel(water.width(), water.height()),
      vVel(water.width(), water.height()),
      lFlux(water.width(), water.height()),
      rFlux(water.width(), water.height()),
      tFlux(water.width(), water.height()),
      bFlux(water.width(), water.height()),
      lX(1.0),
      lY(1.0),
      gravity(9.81)
{
    assert(water.height() == terrain.height() && water.width() == terrain.width());

    // Reset velocity field
    for (uint i=0; i<uVel.width(); ++i) {
        for (uint j=0; j<uVel.height(); ++j) {
            uVel(i,j) = 0;
            vVel(i,j) = 0;
            lFlux(i,j) = rFlux(i,j) = tFlux(i,j) = bFlux(i,j) = 0;
        }
    }
}

FluidSimulation::~FluidSimulation() {
//    delete grid;
}

RANDOM rnd;

void FluidSimulation::makeRain(double dt)
{
    std::uniform_int_distribution<ushort> rndInt(1,water.width()-2);

    for (uint i=0; i<100; i++)
    {
        uint x = rndInt(rnd);
        uint y = rndInt(rnd);

//        water(y,x) += 1;
        water(y-1, x-1)  += 1.0/16.0;
        water(y-1, x)  += 1.0/16.0;
        water(y-1, x+1)  += 1.0/16.0;
        water(y, x-1)  += 1.0/16.0;
        water(y,x)  += 1.0/16.0;
        water(y, x+1)  += 1.0/16.0;
        water(y+1, x-1)  += 1.0/16.0;
        water(y+1, x)  += 1.0/16.0;
        water(y+1, x+1)  += 1.0/16.0;
    }

//    std::uniform_real_distribution<float> rndFloat(0.0f,1.0f);
//    for (uint i=0; i<water.size(); i++)
//    {
//        water(i) += 0.01*rndFloat(rnd);
    //    }
}

void FluidSimulation::makeFlood(double dt)
{
    addRainDrop(rainPos,10,dt*0.01*1);
}

void FluidSimulation::addRainDrop(const vec2 &pos, int rad, float amount)
{
    int rad2 = rad*rad;
    for (int y= -rad; y<=rad; y++)
    {
        int posY = pos.y + y;
        for (int x=-rad; x<=rad; x++)
        {
            int posX = pos.x + x;
            if (posX>=0 && posY>=0 && posX<terrain.width() && posY<terrain.height())
            {
                float d = x*x+y*y;
                if (d <= rad2)
                {
                    float a = amount * (rad2-d);
                    water(posY,posX) += a;
                }
            }
        }
    }
}

//dispatch_queue_t gcdq = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
//dispatch_apply(elements.size(), gcdq, ^(size_t idx)

void FluidSimulation::smoothTerrain()
{
    float maxD = 0.2f;

#if defined(__APPLE__) || defined(__MACH__)
    dispatch_apply(terrain.width(), gcdq, ^(size_t x)
#else
    #pragma omp parallel for
    for (int x=0; x<terrain.width(); ++x)
#endif
    {
        for (int y=0; y<terrain.height(); ++y)
        {
            float h = getTerrain(y,x);

            float hl = getTerrain(y,x-1);
            float hr = getTerrain(y,x+1);
            float ht = getTerrain(y+1,x);
            float hb = getTerrain(y-1,x);

            float dl = h - hl;
            float dr = h - hr;

            float dt = h - ht;
            float db = h - hb;

            tmpSediment(y,x) = h;

            if ((abs(dl) > maxD || abs(dr) > maxD) && dr*dl > 0.0f)
            {
                float avg = (h+hl+hr+ht+hb)/5;
                tmpSediment(y,x) = avg;
            }
            else if ((abs(dt) > maxD || abs(db) > maxD) && dt*db > 0.0f)
            {
                float avg = (h+hl+hr+ht+hb)/5;
                tmpSediment(y,x) = avg;
            }
        }
    }
#if defined(__APPLE__) || defined(__MACH__)
    );
#endif


#if defined(__APPLE__) || defined(__MACH__)
    dispatch_apply(terrain.size(), gcdq, ^(size_t i)
#else
    #pragma omp parallel for
    for (uint i=0; i<terrain.size(); ++i)
#endif
    {
        terrain(i) = tmpSediment(i);
    }

#if defined(__APPLE__) || defined(__MACH__)
    );
#endif
}

void FluidSimulation::computeSurfaceNormals()
{

#if defined(__APPLE__) || defined(__MACH__)
    dispatch_apply(terrain.height(), gcdq, ^(size_t y)
#else
    #pragma omp parallel for
    for (int y=0; y<terrain.height(); ++y)
#endif
    {
        for (int x=0; x<terrain.width(); ++x)
        {
            float r,l,t,b;
            vec3 N;
            r = getTerrain(y,x+1) + getWater(y,x+1);
            l = getTerrain(y,x-1) + getWater(y,x-1);
            t = getTerrain(y+1,x) + getWater(y+1,x);
            b = getTerrain(y-1,x) + getWater(y-1,x);

            N = vec3(l-r, t - b, 2 );
            N = normalize(N);

            state.surfaceNormals(y,x) = N;
        }
    }
#if defined(__APPLE__) || defined(__MACH__)
    );
#endif
}



void FluidSimulation::simulateFlow(double dt)
{

    // Outflux computation settings
    ////////////////////////////////////////////////////////////
    float l = 1;
    float A = 0.00005;

    const float dx = lX;
    const float dy = lY;

    float fluxFactor = dt*A*gravity/l;


    // Outflow Flux Computation with boundary conditions
    ////////////////////////////////////////////////////////////
#if defined(__APPLE__) || defined(__MACH__)
    dispatch_apply(uVel.height(), gcdq, ^(size_t y)
#else
    #pragma omp parallel for
    for (uint y=0; y<uVel.height(); ++y)
#endif
    {
        for (uint x=0; x<uVel.width(); ++x)
        {
            float dh;                               // height difference
            float h0 = terrain(y,x)+water(y,x);     // water height at current cell
            float newFlux;

            // left outflow
            if (x > 0)
            {
                dh = h0 - (terrain(y,x-1)+water(y,x-1));
                newFlux = lFlux(y,x) + fluxFactor*dh;
                lFlux(y,x) = std::max(0.0f,newFlux);
            }
            else
            {
                lFlux(y,x) = 0.0f; // boundary
            }

            // right outflow
            if (x < water.width()-1) {
                dh = h0 - (terrain(y,x+1)+water(y,x+1));
                newFlux = rFlux(y,x) + fluxFactor*dh;
                rFlux(y,x) = std::max(0.0f,newFlux);
            }
            else
            {
                rFlux(y,x) = 0.0f; // boundary
            }

            // bottom outflow
            if (y > 0)
            {
                dh = h0 - (terrain(y-1,x)+water(y-1,x));
                newFlux = bFlux(y,x) + fluxFactor*dh;
                bFlux(y,x) = std::max(0.0f,newFlux);
            }
            else
            {
                bFlux(y,x) = 0.0f; // boundary
            }

            // top outflow
            if (y < water.height()-1) {
                dh = h0 - (terrain(y+1,x)+water(y+1,x));
                newFlux = tFlux(y,x) + fluxFactor*dh;
                tFlux(y,x) = std::max(0.0f,newFlux);
            }
            else
            {
                tFlux(y,x) = 0.0f; // boundary
            }

            // scaling
            float sumFlux = lFlux(y,x)+rFlux(y,x)+bFlux(y,x)+tFlux(y,x);
            float K = std::min(1.0f,float((water(y,x)*dx*dy)/(sumFlux*dt)));
            rFlux(y,x) *= K;
            lFlux(y,x) *= K;
            tFlux(y,x) *= K;
            bFlux(y,x) *= K;
        }
    }
#if defined(__APPLE__) || defined(__MACH__)
    );
#endif

    // Update water surface and velocity field
    ////////////////////////////////////////////////////////////
#if defined(__APPLE__) || defined(__MACH__)
    dispatch_apply(uVel.height(), gcdq, ^(size_t y)
#else
    #pragma omp parallel for
    for (int y=0; y<uVel.height(); ++y)
#endif
    {
        for (int x=0; x<uVel.width(); ++x)
        {
            float inFlow = getRFlux(y,x-1) + getLFlux(y,x+1) + getTFlux(y-1,x) + getBFlux(y+1,x);
            float outFlow = getRFlux(y,x) + getLFlux(y,x) + getTFlux(y,x) + getBFlux(y,x);
            float dV = dt*(inFlow-outFlow);
            float oldWater = water(y,x);
            water(y,x) += dV/(dx*dy);
            water(y,x) = std::max(water(y,x),0.0f);
            float meanWater = 0.5*(oldWater+water(y,x));

            if (meanWater == 0.0f)
            {
                uVel(y,x) = vVel(y,x) = 0.0f;
            }
            else
            {
                uVel(y,x) = 0.5*(getRFlux(y,x-1)-getLFlux(y,x)-getLFlux(y,x+1)+getRFlux(y,x))/(dy*meanWater);
                vVel(y,x) = 0.5*(getTFlux(y-1,x)-getBFlux(y,x)-getBFlux(y+1,x)+getTFlux(y,x))/(dx*meanWater);
            }
        }
    }

#if defined(__APPLE__) || defined(__MACH__)
    );
#endif

}

// flux acess
float FluidSimulation::getRFlux(int y, int x) {
    if (x<0 || x>rFlux.width()-1) {
        return 0.0f;
    } else {
        return rFlux(y,x);
    }
}

float FluidSimulation::getLFlux(int y, int x) {
    if (x<0 || x>lFlux.width()-1) {
        return 0.0f;
    } else {
        return lFlux(y,x);
    }
}

float FluidSimulation::getBFlux(int y, int x) {
    if (y<0 || y>bFlux.height()-1) {
        return 0.0f;
    } else {
        return bFlux(y,x);
    }
}

float FluidSimulation::getTFlux(int y, int x) {
    if (y<0 || y>tFlux.height()-1) {
        return 0.0f;
    } else {
        return tFlux(y,x);
    }
}

float FluidSimulation::getTerrain(int y, int x) {
    return terrain(glm::clamp(y,0,(int) terrain.height()-1),glm::clamp(x,0,(int) terrain.width()-1));
}

float FluidSimulation::getWater(int y, int x){
    return water(glm::clamp(y,0,(int) water.height()-1),glm::clamp(x,0,(int) water.width()-1));
}

void FluidSimulation::simulateErosion(double dt)
{
    const float Kc = 25.0f; // sediment capacity constant
    const float Ks = 0.0001f*12*10; // dissolving constant
    const float Kd = 0.0001f*12*10; // deposition constant

#if defined(__APPLE__) || defined(__MACH__)
    dispatch_apply(sediment.height(), gcdq, ^(size_t y)
#else
    #pragma omp parallel for
    for (int y=0; y<sediment.height(); ++y)
#endif
    {
        for (int x=0; x<sediment.width(); ++x)
        {
            // local velocity
            float uV = uVel(y,x);
            float vV = vVel(y,x);

            // local terrain normal
            vec3 normal = vec3(getTerrain(y,x+1) - getTerrain(y,x-1), getTerrain(y+1,x) - getTerrain(y-1,x), 2 );
            normal = normalize(normal);
            vec3 up(0,0,1);
            float cosa = dot(normal,up);
            float sinAlpha = sin(acos(cosa));
            sinAlpha = std::max(sinAlpha,0.1f);

            // local sediment capacity of the flow
            float capacity = Kc * sqrtf(uV*uV+vV*vV)*sinAlpha*(std::min(water(y,x),0.01f)/0.01f) ;
            float delta = (capacity-sediment(y,x));

            float v = sqrtf(uV*uV+vV*vV);
            float fctr = (std::min(water(y,x),0.01f)/0.01f);

            if (delta > 0.0f)
            {
                float d = Ks*delta;
                terrain(y,x)  -= d;
                water(y,x)    += d;
                sediment(y,x) += d;
            }
            // deposit onto ground
            else if (delta < 0.0f)
            {
                float d = Kd*delta;
                terrain(y,x)  -= d;
                water(y,x)    += d;
                sediment(y,x) += d;
            }
        }
    }
#if defined(__APPLE__) || defined(__MACH__)
    );
#endif
}

void FluidSimulation::simulateSedimentTransportation(double dt)
{
    // semi-lagrangian advection
#if defined(__APPLE__) || defined(__MACH__)
    dispatch_apply(sediment.height(), gcdq, ^(size_t y)
#else
    #pragma omp parallel for
    for (uint y=0; y<sediment.height(); ++y)
#endif
    {
        for (uint x=0; x<sediment.width(); ++x)
        {
            // local velocity
            float uV = uVel(y,x);
            float vV = vVel(y,x);

            // position where flow comes from
            float fromPosX = float(x) - uV*dt;
            float fromPosY = float(y) - vV*dt;

            // integer coordinates
            int x0 = Floor2Int(fromPosX);
            int y0 = Floor2Int(fromPosY);
            int x1 = x0+1;
            int y1 = y0+1;

            // interpolation factors
            float fX = fromPosX - x0;
            float fY = fromPosY - y0;

            // clamp to grid borders
            x0 = clamp(x0,0,int(sediment.width()-1));
            x1 = clamp(x1,0,int(sediment.width()-1));
            y0 = clamp(y0,0,int(sediment.height()-1));
            y1 = clamp(y1,0,int(sediment.height()-1));

            float newVal = mix( mix(sediment(y0,x0),sediment(y0,x1),fX), mix(sediment(y1,x0),sediment(y1,x1),fX), fY);
            tmpSediment(y,x) = newVal;

        }
    }
#if defined(__APPLE__) || defined(__MACH__)
    );
#endif

    // write back new values
#if defined(__APPLE__) || defined(__MACH__)
    dispatch_apply(sediment.size(), gcdq, ^(size_t i)
#else
    #pragma omp parallel for
    for (uint i=0; i<sediment.size(); ++i)
#endif
    {
        sediment(i) = tmpSediment(i);
    }
#if defined(__APPLE__) || defined(__MACH__)
    );
#endif
}

void FluidSimulation::simulateEvaporation(double dt)
{
    const float Ke = 0.00011*0.5; // evaporation constant
#if defined(__APPLE__) || defined(__MACH__)
    dispatch_apply(water.height(), gcdq, ^(size_t y)
#else
    #pragma omp parallel for
    for (uint y=0; y<water.height(); ++y)
#endif
    {
        for (uint x=0; x<water.width(); ++x)
        {
            water(y,x) = std::max(water(y,x)*(1-Ke*dt),0.0);

            if (water(y,x) <0.005f)
            {
                water(y,x) = 0.0f;
            }
        }
    }
#if defined(__APPLE__) || defined(__MACH__)
    );
#endif
}

void FluidSimulation::update(double dt, bool rain, bool flood)
{
    // 1. Add water to the system
    if (rain)
        makeRain(dt);

    if (flood)
        makeFlood(dt);

    // 2. Simulate Flow
    simulateFlow(dt);
    // 3. Simulate Errosion-deposition
    simulateErosion(dt);
    // 4. Advection of suspended sediment
    simulateSedimentTransportation(dt);
    // 5. Simulate Evaporation
    simulateEvaporation(dt);

    smoothTerrain();
    computeSurfaceNormals();


}
