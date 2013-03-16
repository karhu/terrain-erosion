/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#include "Shader.h"

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <boost/filesystem.hpp>

using namespace std;
using namespace Graphics;

Shader::Shader()
{
    _idVertexShader = glCreateShader(GL_VERTEX_SHADER);
    _idFragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    _idProgram  = glCreateProgram();
}

Shader::~Shader()
{
    if (_idProgram != -1)
    {
        glDeleteShader(_idVertexShader);
        glDeleteShader(_idFragmentShader);
        glDeleteProgram(_idProgram);
    }
}

GLuint Shader::ProgramId() const
{
    return _idProgram;
}

const Shader* Shader::_currentlyBound = nullptr;

bool Shader::Load(const std::string& vertexPath, const std::string& fragmentPath )
{
    string infoLog;

    // remember path
    _vertexSourcePath = vertexPath;
    _fragmentSourcePath = fragmentPath;

    // load source
    string vertexSource = readTextFile(vertexPath);
    string fragmentSource = readTextFile(fragmentPath);

    // openGl resources
    GLint vId = glCreateShader(GL_VERTEX_SHADER);
    GLint fId = glCreateShader(GL_FRAGMENT_SHADER);
    GLint pId  = glCreateProgram();

    // compile vertex shader
    /////////////////////////////////////////////////////

    GL::ShaderSource(vId,vertexSource.c_str());
    glCompileShader(vId);

    cout << "Compiling vertex shader: ";
    if (GL::GetCompileStatus(vId))
    {
        cout << "OK" << "\n";
    }
    else
    {
        infoLog = GL::GetShaderInfoLog(vId);
        cout << "ERROR: " << vertexPath << "\n";
        cout << infoLog << "\n";
        glDeleteShader(vId);
        glDeleteShader(fId);
        glDeleteProgram(pId);
        return false;
    }

    // compile fragment shader
    /////////////////////////////////////////////////////

    GL::ShaderSource(fId,fragmentSource.c_str());
    glCompileShader(fId);

    cout << "Compiling vertex shader: ";
    if (GL::GetCompileStatus(fId))
    {
        cout << "OK" << "\n";
    }
    else
    {
        infoLog = GL::GetShaderInfoLog(fId);
        cout << "ERROR: " << fragmentPath << "\n";
        cout << infoLog << "\n";
        glDeleteShader(vId);
        glDeleteShader(fId);
        glDeleteProgram(pId);
        return false;
    }

    // linking
    /////////////////////////////////////////////////////

    glAttachShader(pId,vId);
    glAttachShader(pId,fId);
    glLinkProgram(pId);

    cout << "Linking shaders: ";
    if (GL::GetLinkStatus(pId))
    {
        cout << "OK" << "\n";
    }
    else
    {
        infoLog = GL::GetProgramInfoLog(pId);
        cout << "ERROR: " << vertexPath << ", " << fragmentPath << "\n";
        glDeleteShader(vId);
        glDeleteShader(fId);
        glDeleteProgram(pId);
        cout << infoLog << "\n";
        return false;
    }

    if (_idProgram != -1)
    {
        glDeleteShader(_idVertexShader);
        glDeleteShader(_idFragmentShader);
        glDeleteProgram(_idProgram);
    }

    _idVertexShader = vId;
    _idFragmentShader = fId;
    _idProgram = pId;

    // onLoad
    loadUniformInfo();
    loadAttributeInfo();
    //    onLoad();
    return true;
}

bool Shader::Reload()
{
    return Load(_vertexSourcePath,_fragmentSourcePath);
}

void Shader::Bind() const
{
    glUseProgram(_idProgram);
    Shader::_currentlyBound = this;
}

void Shader::UnBind() const
{
    glUseProgram(0);
    Shader::_currentlyBound = nullptr;
}

bool Shader::SetUniform(const string &name, const glm::vec3 &v)
{
    try
    {
        UniformInfo& info = _uniformInfo.at(name);
        assert(info.Type == GL::ShaderVariableType::vec3_t);
        assert(info.Length == 1);
        assert(Shader::_currentlyBound == this);
        glUniform3fv(info.Location,1, &v.x);
        return true;
    }
    catch (out_of_range& oor)
    {
        return false;
    }
}

bool Shader::SetUniform(const string &name, const glm::vec4 &v)
{
    try
    {
        UniformInfo& info = _uniformInfo.at(name);

        assert(info.Type == GL::ShaderVariableType::vec4_t);
        assert(info.Length == 1);
        assert(Shader::_currentlyBound == this);
        glUniform4fv(info.Location,1, &v.x);
        return true;
    }
    catch (out_of_range& oor)
    {
        return false;
    }
}

bool Shader::SetUniform(const string &name, const glm::mat4 &v, bool transpose)
{
    try
    {
        UniformInfo& info = _uniformInfo.at(name);
        assert(info.Type == GL::ShaderVariableType::mat4_t);
        assert(info.Length == 1);
        assert(Shader::_currentlyBound == this);
        glUniformMatrix4fv(info.Location,1, transpose, &v[0].x);
        return true;
    }
    catch (out_of_range& oor)
    {
        return false;
    }
}

bool Shader::SetUniform(const string &name, bool v)
{
    try
    {
        UniformInfo& info = _uniformInfo.at(name);
        assert(info.Type == GL::ShaderVariableType::bool_t);
        assert(info.Length == 1);
        assert(Shader::_currentlyBound == this);
        glUniform1i(info.Location,v);
        return true;
    }
    catch (out_of_range& oor)
    {
        return false;
    }
}

bool Shader::SetUniform(const string &name, int v)
{
    try
    {
        UniformInfo& info = _uniformInfo.at(name);
        assert(info.Type == GL::ShaderVariableType::int_t);
        assert(info.Length == 1);
        assert(Shader::_currentlyBound == this);
        glUniform1i(info.Location,v);
        return true;
    }
    catch (out_of_range& oor)
    {
        return false;
    }
}

bool Shader::SetUniform(const string &name, const TextureBase &tex)
{
    try
    {
        UniformInfo& info = _uniformInfo.at(name);
        assert(info.Type == GL::ShaderVariableType::sampler2D_t);
        assert(info.Length == 1);
        assert(Shader::_currentlyBound == this);
        glUniform1i(info.Location,tex.TextureUnit()+GL_TEXTURE0);
        return true;
    }
    catch (out_of_range& oor)
    {
        return false;
    }
}

bool Shader::MapAttribute(const string& name, uint location)
{
    AttributeInfo& at = _attributeInfo[name];
    at.Location = location;
    at.Name = location;

    if (at.Present)
    {
        glBindAttribLocation(_idProgram, location, name.c_str());

        // Relink shader
        glLinkProgram(_idProgram);
        if (!GL::GetLinkStatus(_idProgram))
        {
            auto infoLog = GL::GetProgramInfoLog(_idProgram);
            throw ShaderException("Shader Exception :: Path=\""+_vertexSourcePath+ "\" :: Linking after attribute mapping :: " + infoLog);
        }
    }

    return at.Present;
}

uint Shader::AttributeLocation(const string &name)
{
    try
    {
       AttributeInfo& at = _attributeInfo.at(name);
       if (at.Location == -1)
       {
           throw ShaderException("Shader Exception :: Path=\""+_vertexSourcePath+ "\" :: Attribute is not mapped to any location :: "+name);
       }
       if (!at.Present)
       {
           throw ShaderException("Shader Exception :: Path=\""+_vertexSourcePath+ "\" :: Attribute is currently not present in the shader :: "+name);
       }
       return at.Location;
    }
    catch (out_of_range& oor)
    {
        throw ShaderException("Shader Exception :: Path=\""+_vertexSourcePath+ "\" :: Attribute not found :: "+name);
    }
}

const string &Shader::PathVertexSource() const
{
    return _vertexSourcePath;
}

const string &Shader::PathFragmentSource() const
{
    return _fragmentSourcePath;
}

void Shader::loadUniformInfo()
{
    _uniformCount = GL::GetProgramProperty(_idProgram,GL::ProgramProperty::UniformCount);

    cout << "UniformCount: " << _uniformCount << "\n"; // DEBUG

    _uniformInfo.clear();
    for (uint i=0; i<_uniformCount; i++)
    {
        UniformInfo info;
        GL::GetUniformInfo(_idProgram,i,info.Length,info.Type,info.Name);
        info.Location = GL::GetUniformLocation(_idProgram, info.Name);

        cout << "\t" << info.Name << " (" << GL::StringOf(info.Type) << "," << info.Length << "): " << info.Location << "\n"; // DEBUG

        _uniformInfo[info.Name] = info;
    }
}

void Shader::loadAttributeInfo()
{
    _attributeCount = GL::GetProgramProperty(_idProgram,GL::ProgramProperty::AttributeCount);
    cout << "AttributeCount: " << _attributeCount << "\n"; // DEBUG

    for (int i=0; i<_attributeCount; i++)
    {
        AttributeInfo info;
        GL::GetAttributeInfo(_idProgram,i,info.Length,info.Type,info.Name);
        info.Location = -1;
        info.Present = true;


        auto iter = _attributeInfo.find(info.Name);
        if (iter != _attributeInfo.end())
        {
            AttributeInfo& oldAt  = (*iter).second;
            info.Location = oldAt.Location;
            if (info.Present)
            {
                glBindAttribLocation(_idProgram,info.Location, info.Name.c_str());
            }
        }


        cout << "\t" << info.Name << " (" << GL::StringOf(info.Type) << "," << info.Length << "): " << info.Location << "\n"; // DEBUG

        _attributeInfo[info.Name] = info;
    }

    // Relink shader
    glLinkProgram(_idProgram);
    if (!GL::GetLinkStatus(_idProgram))
    {
        auto infoLog = GL::GetProgramInfoLog(_idProgram);
        throw ShaderException("Shader Exception :: Path=\""+_vertexSourcePath+ "\" :: Linking after attribute mapping :: " + infoLog);
    }
}

string Shader::readTextFile(const string &path)
{
    std::ifstream in(path.c_str());
    return std::string((std::istreambuf_iterator<char>(in)),
                       std::istreambuf_iterator<char>());
}

std::shared_ptr<Shader> ShaderManager::LoadShader(const string &vertexPath, const string &fragmentPath)
{
    // Create Shader
    shared_ptr<Shader> sptr(new Shader());
    sptr->Load(vertexPath,fragmentPath);

    // Remember Shader
    Entry e;
    e.shaderPtr = sptr;
    e.timestamp = std::time(NULL);
    _loadedShaders.insert(e);

    // Return Shader
    return sptr;
}

void ShaderManager::Update()
{
    for (set<Entry>::iterator iter = _loadedShaders.begin(); iter != _loadedShaders.end(); iter++)
    {
        Entry e = *iter;
        shared_ptr<Shader> sPtr = e.shaderPtr.lock();

        bool remove = false;
        if(sPtr)
        {
            boost::filesystem::path pV(sPtr->PathVertexSource());
            boost::filesystem::path pF(sPtr->PathFragmentSource());

            if (boost::filesystem::exists(pV) && boost::filesystem::exists(pF))
            {
                std::time_t tv = boost::filesystem::last_write_time(pV) ;
                std::time_t tf = boost::filesystem::last_write_time(pF) ;
                if (tv > e.timestamp || tf > e.timestamp)
                {
                    sPtr->Reload();
                    e.timestamp = std::max(tv,tf);
                    _loadedShaders.erase(iter);
                    _loadedShaders.insert(e);
                }
            }
            else
            {
                remove = true;
            }

        }
        else
        {
            remove = true;
        }

        if (remove)
        {
            _loadedShaders.erase(iter);
        }
    }
}


