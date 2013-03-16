/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#ifndef MESH_H
#define MESH_H

#include <type_traits>
#include "platform_includes.h"

#include "glm/glm.hpp"

#include <vector>
#include <unordered_map>
#include <typeinfo>


// Slice //////////////////////////////////////////
///////////////////////////////////////////////////

template<typename T>
struct Slice
{
private:
    T* _data;
    ulong _size;

public:

    /// constructor
    Slice(T* data, ulong size) : _data(data), _size(size)
    {}

    /// element access
    T& operator[] (ulong idx)
    {
        return _data[idx];
    }

    /// element count
    ulong Size() { return _size; }
};


// Mesh ///////////////////////////////////////////
///////////////////////////////////////////////////

class Mesh
{
protected:
    struct Property
    {

    };

public:
    Mesh(ulong vertexCount,ulong faceCount);

    /// access to the vertex coordinates
    Slice<glm::vec3> VertexCoordinates();
    Slice<const glm::vec3> VertexCoordinates() const;

    /// access to the face indices
    Slice<glm::uvec3> FaceIndices();
    Slice<const glm::uvec3> FaceIndices() const;

    /// resize mesh
    /// attention: this will invalidate any slices
    void SetFaceCount(uint faceCount);
    void SetVertexCount(uint vertexCount);

    /// access to vertex attributes

protected:
    std::vector<glm::vec3>      _vertexCoords;
    std::vector<glm::uvec3>     _faceIndices;

//    std::unordered_map<std::string,vector>
};



Mesh::Mesh(ulong vertexCount, ulong indexCount)
    : _vertexCoords(vertexCount), _faceIndices(indexCount)
{}

Slice<glm::vec3> Mesh::VertexCoordinates()
{
    const std::type_info& nfo = typeid(Slice<glm::vec3>);
    std::cout << "type_info: " << nfo.name() << "\n";
    return Slice<glm::vec3>( &_vertexCoords.front(),_vertexCoords.size());
}

Slice<const glm::vec3> Mesh::VertexCoordinates() const
{
    return Slice<const glm::vec3>( &_vertexCoords.front(),_vertexCoords.size());
}

Slice<glm::uvec3> Mesh::FaceIndices()
{
    return Slice<glm::uvec3>( &_faceIndices.front(),_faceIndices.size());
}

Slice<const glm::uvec3> Mesh::FaceIndices() const
{
    return Slice<const glm::uvec3>( &_faceIndices.front(),_faceIndices.size());
}

void Mesh::SetFaceCount(uint faceCount)
{
    _faceIndices.resize(faceCount);
}

void Mesh::SetVertexCount(uint vertexCount)
{
    _vertexCoords.resize(vertexCount);
}


#endif // MESH_H
