#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location=1) in vec2 UV;
layout(location=2) in vec3 normal;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform vec3 inColor;
uniform mat4 SLightMVP;

out vec4 shadowmapPos;
out vec2 uv;
void main(){
    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  MVP * vec4(vertexPosition_modelspace,1);
    shadowmapPos= (SLightMVP*vec4(vertexPosition_modelspace,1));
    uv=UV;
}
