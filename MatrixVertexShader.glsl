#version 330 core

// Input vertex data, different for all executions of this shader.
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location=1) in vec2 UV;

// Values that stay constant for the whole mesh.
uniform mat4 M,V,P;

void main(){
    // Output position of the vertex, in clip space : MVP * position
    gl_Position =  P*V*M * vec4(vertexPosition_modelspace,1);
}