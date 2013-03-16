/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#ifndef SHADER_H
#define SHADER_H

#include "platform_includes.h"
#include "GLWrapper.h"
#include "Exception.h"

#include "Texture2D.h"

#include <unordered_map>
#include <set>
#include <memory>
#include <ctime>

namespace Graphics
{

class Shader
{

public:

    class ShaderException : public Exception
    {
    public:
        ShaderException(const std::string& message) : Exception(message) {}
    };

protected:

    struct UniformInfo
    {
        std::string Name;                   /// name of the uniform variable
        GL::ShaderVariableType Type;        /// type of the uniform variable
        int Length;                         /// length of the uniform array
        uint Location;                      /// location property
    };

    struct AttributeInfo
    {
        std::string Name;
        GL::ShaderVariableType Type;
        int Length;
        int Location;
        bool Present;
    };

public:

    Shader();
    ~Shader();

    GLuint ProgramId() const;

    bool Load(const std::string& vertexPath, const std::string& fragmentPath );
    bool Reload();

    /// Bind this shader to start using it.
    void Bind() const;

    /// Unbind any shader currently in use.
    void UnBind() const;

    /// Set Uniforms
    bool SetUniform(const std::string& name, const glm::vec3& v);
    bool SetUniform(const std::string& name, const glm::vec4& v);
    bool SetUniform(const std::string& name, const glm::mat4& v, bool transpose = false);
    bool SetUniform(const std::string& name, bool v);
    bool SetUniform(const std::string& name, int v);
    bool SetUniform(const std::string& name, const TextureBase& tex);

    /// Attributes
    bool MapAttribute(const std::string& name, uint location);
    uint AttributeLocation(const std::string& name);

    /// Paths
    const std::string& PathVertexSource() const;
    const std::string& PathFragmentSource() const;

protected:

    void loadUniformInfo();
    void loadAttributeInfo();

protected:

    std::string readTextFile(const std::string& path);

protected:

    GLint _idProgram;
    GLint _idVertexShader;
    GLint _idFragmentShader;

    std::string _vertexSourcePath;
    std::string _fragmentSourcePath;

    int _uniformCount;
    std::unordered_map<std::string,UniformInfo> _uniformInfo;

    int _attributeCount;
    std::unordered_map<std::string,AttributeInfo> _attributeInfo;

protected:
    static const Shader* _currentlyBound;

};


class ShaderManager
{
public:
    std::shared_ptr<Shader> LoadShader(const std::string& vertexPath, const std::string& fragmentSource);
    void Update();
protected:

    struct Entry
    {
        std::weak_ptr<Shader> shaderPtr;
        std::time_t timestamp;

        bool operator<(const Entry& other)const
        {
            return other.timestamp < this->timestamp;
        }
    };

    std::set<Entry> _loadedShaders;
};

}
#endif // SHADER_H
