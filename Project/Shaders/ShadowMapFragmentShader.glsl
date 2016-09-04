#version 330 core

uniform vec3 inColor;
uniform sampler2D SLightDepthMap;

in vec3 shadowmapPos;
out vec3 color;
void main(){
     vec3 depth=texture(SLightDepthMap,shadowmapPos.xy).rgb;
     float zDist=texture(SLightDepthMap,shadowmapPos.xy).r;
     //if zDist>shadowmapPos.z-0.05f
     float lightLevel=zDist-shadowmapPos.z+0.05f;
     lightLevel=clamp(lightLevel*10000,0,1);
     color=depth;//inColor*lightLevel;//*vec3(1,0.5,0.5);
}