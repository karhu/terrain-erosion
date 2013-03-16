/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)
    Copyright (C) 2012 Pascal Spörri (pascal.spoerri@gmail.com)
    Copyright (C) 2012 Sabina Schellenberg (sabina.schellenberg@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#include "PerlinNoise.h"
#include "vector"

#include <random>
#include <glm/glm.hpp>
#include <glm/gtx/compatibility.hpp>
#include "MathUtil.h"


using namespace std;
#if defined(__APPLE__) || defined(__MACH__)
// for Mac
using namespace glm;

#elif defined(__linux__) || defined(UNIX)
using namespace glm::gtx::compatibility;

#elif defined(_WIN32) || defined(_WIN64)
// for Windows
// No support, sorry
#endif

typedef std::mt19937 RANDOM;  // the Mersenne Twister with a popular choice of parameters

PerlinNoise::PerlinNoise(uint seed):
    PermTable(2*TABLE_SIZE,0)
{
    vector<uint> perm(TABLE_SIZE);

    // init random number gen
    RANDOM rnd;
    rnd.seed(seed);
    std::uniform_int_distribution<ushort> rndInt(0,TABLE_SIZE-1);

    // fill the table
    for (int i=0; i<TABLE_SIZE; i++)
    {
        perm[i] = i;
    }

    // permute the table
    for (int i=0; i<TABLE_SIZE; i++)
    {
        ushort p = rndInt(rnd);

        ushort v1 = perm[i];
        ushort v2 = perm[p];

        perm[p] = v1;
        perm[i] = v2;
    }

    // copy to the actual table
    for (int i=0; i<TABLE_SIZE; i++)
    {
        ushort v = perm[i];
        PermTable[i] = v;
        PermTable[i+TABLE_SIZE] = v;
    }
}


float PerlinNoise::Sample(float x, float y, float z) const
{

    // Convert indices to cell indices and offsets
    ////////////////////////////////////////////////

    int ix = Floor2Int(x);
    int iy = Floor2Int(y);
    int iz = Floor2Int(z);

    float dx = x - ix;
    float dy = y - iy;
    float dz = z - iz;

    ix &= (TABLE_SIZE-1);
    iy &= (TABLE_SIZE-1);
    iz &= (TABLE_SIZE-1);

    // Compute lattice influences
    ////////////////////////////////////////////////

    float w000 = grad(ix  , iy  , iz  , dx  , dy  , dz  );
    float w100 = grad(ix+1, iy  , iz  , dx-1, dy  , dz  );
    float w010 = grad(ix  , iy+1, iz  , dx  , dy-1, dz  );
    float w110 = grad(ix+1, iy+1, iz  , dx-1, dy-1, dz  );
    float w001 = grad(ix  , iy  , iz+1, dx  , dy  , dz-1);
    float w101 = grad(ix+1, iy  , iz+1, dx-1, dy  , dz-1);
    float w011 = grad(ix  , iy+1, iz+1, dx  , dy-1, dz-1);
    float w111 = grad(ix+1, iy+1, iz+1, dx-1, dy-1, dz-1);


    // Interpolate between these values
    ////////////////////////////////////////////////

    float wx = smoothWeight(dx);
    float wy = smoothWeight(dy);
    float wz = smoothWeight(dz);

    // interpolation in x direction
    float x00 = lerp(w000,w100,wx);
    float x10 = lerp(w010,w110,wx);
    float x01 = lerp(w001,w101,wx);
    float x11 = lerp(w011,w111,wx);

    // interpolation in y direction
    float y0 = lerp(x00,x10,wy);
    float y1 = lerp(x01,x11,wy);

    // interpolation in z direction
    float result = lerp(y0,y1,wz);

    return result;

}



float PerlinNoise::grad(int x, int y, int z, float dx, float dy, float dz) const
{
    // get gradient index
    int h = PermTable[PermTable[PermTable[x]+y]+z];
    h &= 15;

    float u = h<8 || h==12 || h==13 ? dx : dy;
    float v = h<4 || h==12 || h==13 ? dy : dz;

    return ((h&1) ? -u : u) + ((h&2) ? -v : v);
}

float PerlinNoise::smoothWeight(float v) const
{
    float v3 = v*v*v;
    float v4 = v3*v;
    float v5 = v4*v;

    return 6.0f*v5 - 15.0f*v4 + 10.0f*v3;
}
