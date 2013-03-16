/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)
    Copyright (C) 2012 Pascal Spörri (pascal.spoerri@gmail.com)
    Copyright (C) 2012 Sabina Schellenberg (sabina.schellenberg@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#ifndef GRID2D_H
#define GRID2D_H

#include "platform_includes.h"
#include <vector>

template<typename T>
class Grid2D
{
protected:
    uint _width;
    uint _height;
    uint _size;
    std::vector<T> _data;

public:
    Grid2D(uint w=0, uint h=0)
        : _width(w), _height(h), _size(w*h), _data(_size)
    {}

    uint width() const { return _width; }
    uint height() const { return _height;}
    uint size() const { return _size; }

    void resize(uint w, uint h)
    {
        _width = w;
        _height = h;
        _size = w*h;
        _data.resize(w*h);
    }

    T& operator ()(uint y, uint x);
    const T& operator ()(uint y, uint x) const;

    T& operator ()(uint i);
    const T& operator ()(uint i) const;

    T* ptr() { return &_data[0]; }
    const T* ptr() const { return &_data[0]; }
};

template<typename T>
T &Grid2D<T>::operator ()(uint y, uint x)
{
    return _data[y*_width+x];
}

template<typename T>
const T& Grid2D<T>::operator ()(uint y, uint x) const
{
    return _data[y*_width+x];
}

template<typename T>
T &Grid2D<T>::operator ()(uint i)
{
    return _data[i];
}

template<typename T>
const T& Grid2D<T>::operator ()(uint i) const
{
    return _data[i];
}


class Grid2DHelper
{
public:
    static void MakeUniformGrid( Grid2D<glm::vec2>& outGrid, uint w, uint h)
    {
        // create coordinate grid
        outGrid.resize(w,h);
        uint idx=0;
        for (uint y=0; y<h; y++)
        {
            for (uint x=0; x<w; x++)
            {
                outGrid(idx) = glm::vec2(float(x)/(w-1),float(y)/(h-1));
                idx++;
            }
        }
    }

    static void MakeGridIndices( std::vector<uint>& outIndices, uint w, uint h)
    {
        outIndices.clear();
        outIndices.reserve((w-1)*(h-1)*6);
        for (uint y=0; y<h-1; y++)
        {
            for (uint x=0; x<w-1; x++)
            {
                uint p00 = y*w+x;
                uint p10 = p00 + 1;
                uint p01 = p00 + w;
                uint p11 = p10 + w;

                // add the two faces
                outIndices.push_back(p00); outIndices.push_back(p11); outIndices.push_back(p10);
                outIndices.push_back(p00); outIndices.push_back(p01); outIndices.push_back(p11);
            }
        }

    }

};

#endif // GRID2D_H
