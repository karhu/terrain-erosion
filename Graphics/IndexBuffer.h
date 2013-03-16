/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#ifndef INDEXBUFFER_H
#define INDEXBUFFER_H

#include "platform_includes.h"
#include <vector>

namespace Graphics
{

class IndexBuffer
{
public:

    IndexBuffer();

    ~IndexBuffer();

    /// Binds the index buffer for further use by OpenGL draw calls.
    void Bind();

    /// Number of entries in the index buffer.
    uint IndexCount() const;

    /// Number of entries in the index buffer.
    GLuint IndexType() const;

    /// Sets the data stored in the buffer. Accepts both scalar and vector arrays.
    template<typename T>
    void SetData(const std::vector<T>& data);


protected:

    template<typename T>
    void rememberType(const T& value);

protected:

    GLuint _id;
    uint _indexCount;
    GLuint _indexType;
};


// Implementation
//////////////////////////////////////////////

template<typename T>
void IndexBuffer::SetData(const std::vector<T>& data)
{
    assert(data.size() > 0);

    // Remember number of indices in data
    _indexCount = data.size();

    // Remember the index type
    rememberType(data.front());

    uint byteSize = _indexCount*sizeof(T);

    // Store data on the GPU
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, byteSize, &data.front(), GL_STATIC_DRAW);
}

} // namespace Graphics

#endif // INDEXBUFFER_H
