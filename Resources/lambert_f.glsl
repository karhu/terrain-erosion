/****************************************************************************
    Copyright (C) 2012 Adrian Blumer (blumer.adrian@gmail.com)
    Copyright (C) 2012 Pascal Spörri (pascal.spoerri@gmail.com)
    Copyright (C) 2012 Sabina Schellenberg (sabina.schellenberg@gmail.com)

    All Rights Reserved.

    You may use, distribute and modify this code under the terms of the
    MIT license (http://opensource.org/licenses/MIT).
*****************************************************************************/

#version 150

// Transform matrices
/////////////////////////////////////////////////

uniform mat4 uProjMatrix;         // Projection Matrix
uniform mat4 uViewMatrix;         // View Matrix

// Other uniforms
/////////////////////////////////////////////////

uniform vec4 uColor; // Material Color

// Input from Vertex Shader
/////////////////////////////////////////////////

in vec2  vGridCoord;
in float vTerrainHeight;
in float vWaterHeight;
in float vSediment;

in vec3  vP; // shading postion in camera space
in vec3  vN; // shading normal in camera space
in vec4  vFragPos;
in vec3  vColor;
in vec4  vNormal;

// Output
/////////////////////////////////////////////////

out vec4 fColor;

// Main
/////////////////////////////////////////////////

void main(void)
{
    // reconstructed face normals
    //vec3 N=normalize(vec3(dFdx(gl_FragCoord.z),dFdy(gl_FragCoord.z),0.0001));
    vec3 N = normalize(vNormal.rgb);

    // toLight vectors
    vec3 L1 = normalize((uViewMatrix*vec4(-10,10,-10,1)-vFragPos).rgb);
    vec3 L2 = normalize((uViewMatrix*vec4(10,10,10,1)-vFragPos).rgb);
    float light = max(dot(N,L1),0.0f)*0.7 + max(dot(N,L2),0.0f)*0.2;

    vec4 waterColor = vec4(0,0.3,0.9,1);
//    vec4 terrainColor = vec4(0.8,0.8,0.8,1);
    vec4 terrainColor = vec4(17.0/255.0,132.0/255.0,5.0/255.0,1);
    vec4 sedimentColor = vec4(194.0/255.0,141.0/255.0,76.0/255.0,1);
    float factor = clamp((min(vWaterHeight,6.0)/6.0), 0.0, 1.0);
    factor = 1-pow((1-factor),4);
 
    fColor = (factor*waterColor+(1.0-factor)*terrainColor)*light;

//    fColor = mix(fColor,vec4(0,1,0,1),vSediment);
    fColor = mix(fColor,sedimentColor,vSediment);

    float gamma = 2.2;
    fColor.rgb = pow(fColor.rgb,vec3(1,1,1)/gamma);
}
