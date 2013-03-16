/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)
    Copyright (C) 2012 Pascal Spörri (pascal.spoerri@gmail.com)
    Copyright (C) 2012 Sabina Schellenberg (sabina.schellenberg@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

using namespace glm;

Camera::Camera()
{
    SetProjection();
}

void Camera::SetProjection(float angle, float aspect, float near, float far)
{
    _angle = angle;
    _aspect = aspect;
    _near = near;
    _far = far;
    _projMatrix = glm::perspective(angle,aspect,near,far);
}

void Camera::SetAspectRatio(float aspect)
{
    _aspect = aspect;
    _projMatrix = glm::perspective(_angle,_aspect,_near,_far);
}

const glm::mat4x4 &Camera::ProjMatrix()
{
    return _projMatrix;
}

const glm::mat4x4 &Camera::ViewMatrix()
{
//    glm::mat4_cast(_forward);
    recomputeViewMatrix();
    return _viewMatrix;
}

void Camera::TranslateGlobal(const vec3 &delta)
{
    _position += delta;
    recomputeViewMatrix();
}

void Camera::TranslateLocal(const vec3 &delta)
{
    vec4 d = vec4(delta,0.0f);
    d = mat4_cast(inverse(_forward))*d;
    _position += vec3(d.x,d.y,d.z);
}

void Camera::LocalRotate(const vec3 &axis, float angle)
{
    // deg to rad
    float a = M_PI*angle/180.0f;

    vec3 ax = normalize(axis);

    float sina = sinf(a/2.0f);
    float cosa = cosf(a/2.0f);

    ax *= sina;
    float s = cosa;

    fquat offset(s,ax);

    _forward = offset * _forward;

}

void Camera::GlobalRotate(const vec3 &axis, float angle)
{
    // deg to rad
    float a = M_PI*angle/180.0f;

    vec3 ax = normalize(axis);

    float sina = sinf(a/2.0f);
    float cosa = cosf(a/2.0f);

    ax *= sina;
    float s = cosa;

    fquat offset(s,ax);

    _forward =  _forward*offset;
}

void Camera::recomputeViewMatrix()
{
    normalize(_forward);

    glm::mat4 Identity = glm::mat4(1.0f); // identity matrix

    _viewMatrix =  mat4_cast(_forward)*glm::translate(Identity,-_position);
}
