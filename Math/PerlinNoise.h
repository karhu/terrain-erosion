/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)
    Copyright (C) 2012 Pascal Spörri (pascal.spoerri@gmail.com)
    Copyright (C) 2012 Sabina Schellenberg (sabina.schellenberg@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include "platform_includes.h"
#include <vector>

class PerlinNoise
{
public:
    PerlinNoise(uint seed=0);
    ~PerlinNoise() {}

    float Sample(float x, float y=0, float z=0) const;

protected:

    float grad(int x, int y, int z, float dx, float dy, float dz) const;

    float smoothWeight(float v) const;

    static const int TABLE_SIZE = 256;

    std::vector<ushort> PermTable;

};

#endif // PERLINNOISE_H
