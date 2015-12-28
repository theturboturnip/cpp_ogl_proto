#version 330 core

layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec2 vertexUV;
layout(location=2) in vec3 vertexNormal;

out vec2 UV;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDirection_cameraspace;

uniform mat4 M,V,P;
uniform vec3 LightPosition_worldspace;

void main(){
     vec4 vec4vertexPos=vec4(vertexPosition,1);
     gl_Position=P*V*M*vec4vertexPos;
     UV=vertexUV;
     Position_worldspace=(M*vec4vertexPos).xyz;
     vec3 vertexPosition_cameraspace=(V*M*vec4vertexPos).xyz;
     vec3 lightPosition_cameraspace=(V*vec4(LightPosition_worldspace,1)).xyz;
     LightDirection_cameraspace=lightPosition_cameraspace-vertexPosition_cameraspace;
     Normal_cameraspace=(V*transpose(inverse(M))*vec4(vertexNormal,0)).xyz;
}