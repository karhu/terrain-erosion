/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include "platform_includes.h"
#include "GLWrapper.h"
#include "Grid2D.h"

namespace Graphics
{

class TextureBase
{
public:
    TextureBase();
    virtual ~TextureBase();

protected:
    GLuint _id;
    GLuint _textureUnit;

public:
    GLuint Id() const { return _id; }
    GLuint TextureUnit() const { return _textureUnit; }
};

template<typename T, TextureFormat FORMAT, uint BPC>
class Texture2D : public TextureBase
{
public:
    void SetData(const Grid2D<T>& data);

    void Map(uint textureUnit);


// some static asserts on template parameters;
static_assert(BPC == 8 || BPC == 16 || BPC == 32,
              "The BitsPerComponent (BPC) parameter must be one of 8,16 or 32.");
static_assert(FORMAT != TextureFormat::Float || BPC != 8,
              "TextureFormat::Float requires 16 or 32 bits per component (BPC).");
static_assert(FORMAT != TextureFormat::FloatNormalized || BPC != 32,
              "TextureFormat::FloatNormalized requires 8 or 16 bits per component (BPC).");
static_assert(FORMAT != TextureFormat::FloatSignedNormalized || BPC != 32,
              "TextureFormat::FloatNormalizedSigned requires 8 or 16 bits per component (BPC).");

};

// Implementation
///////////////////////////////////////////////

inline TextureBase::TextureBase()
{
    glGenTextures(1,&_id);
}

inline TextureBase::~TextureBase()
{
    glDeleteTextures(1,&_id);
}

template<typename T, TextureFormat FORMAT, uint BPC>
inline void Texture2D<T,FORMAT,BPC>::Map(uint textureUnit)
{
    _textureUnit = textureUnit;
    glActiveTexture(GL_TEXTURE0 + textureUnit);
    glBindTexture(GL_TEXTURE_2D,_id);
    glActiveTexture(GL_TEXTURE0);
}

template<typename T, TextureFormat FORMAT, uint BPC>
inline void Texture2D<T,FORMAT,BPC>::SetData(const Grid2D<T>& data)
{
    glBindTexture(GL_TEXTURE_2D, _id);
    glTexImage2D(GL_TEXTURE_2D,
                 0,
                 GL::InternalTextureFormat<FORMAT,BPC,GL::TypeInfo<T>::ElementCount >(), // format used for storage
                 data.width(),data.height(),
                 0,
                 GL::TypeInfo<T>::TexFormat,
                 GL::TypeInfo<T>::ElementType,
                 data.ptr());

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    // (x,y,z) = (s,t,r)


//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);


//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);


//        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
//        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
//        glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);

    glBindTexture(GL_TEXTURE_3D, 0);
}



}
#endif // TEXTURE2D_H


