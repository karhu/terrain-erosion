/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)
    Copyright (C) 2012 Pascal Spörri (pascal.spoerri@gmail.com)
    Copyright (C) 2012 Sabina Schellenberg (sabina.schellenberg@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#ifndef CAMERA_H
#define CAMERA_H

#include "platform_includes.h"
#include <glm/gtc/quaternion.hpp>

// Backwards compatibility
#define GLM_FORCE_RADIANS
using namespace glm;

class Camera
{
public:

    Camera();

    void SetProjection(float angle=45.0f, float aspect=800.0f/600.0f, float near=0.1f, float far=100.0f);

    void SetAspectRatio(float aspect);

    /// Matrix access
    const glm::mat4x4& ProjMatrix();
    const glm::mat4x4& ViewMatrix();

    // Transform
    void TranslateGlobal(const glm::vec3& delta);
    void TranslateLocal(const glm::vec3& delta);
    void LocalRotate(const glm::vec3& axis, float angle);

    void GlobalRotate(const glm::vec3& axis, float angle);

protected:

    void recomputeViewMatrix();
protected:

    glm::vec3   _position;
    glm::fquat  _forward;
    glm::fquat  _upward;

    glm::mat4x4 _viewMatrix;
    glm::mat4x4 _projMatrix;

    float _angle;
    float _near, _far;
    float _aspect;

};



#endif // CAMERA_H
