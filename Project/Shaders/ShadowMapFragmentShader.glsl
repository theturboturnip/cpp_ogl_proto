#version 330 core

uniform vec3 inColor;
uniform sampler2D SLightDepthMap;
uniform float farPlane,nearPlane;

in vec3 shadowmapPos;
out vec3 color;

float InterpretShadowmap(float shadowmapValue){
      float shadowmapValue01=2*shadowmapValue-1;
      float toReturn=2*farPlane*nearPlane/(farPlane+nearPlane-shadowmapValue01*(farPlane-nearPlane));
      return toReturn;
}

void main(){
     float shadowmapPosX=(shadowmapPos.x+1)/2;
     float shadowmapPosY=(shadowmapPos.y+1)/2;
     vec2 newUV=vec2(shadowmapPosX,shadowmapPosY);
     //vec3 depth=texture(SLightDepthMap,shadowmapPos.xy).rgb;
     float light_depth=texture2D(SLightDepthMap,newUV).r;
     float zDist=InterpretShadowmap(light_depth);
     float currentZ=InterpretShadowmap(shadowmapPos.z);
     //if zDist>shadowmapPos.z-0.05f
     float lightLevel=zDist-currentZ;
     if (lightLevel>=0.95f)
        lightLevel=1;
     else
        lightLevel=0;
     //color=vec3(shadowmapPos.z);//inColor*lightLevel;//*vec3(1,0.5,0.5);
     //color=vec3((shadowmapPos.x+1)/2,(shadowmapPos.y+1)/2,(shadowmapPos.z+1)/2);//inColor*lightLevel;//*vec3(1,0.5,0.5);
     //color=inColor*lightLevel;//*vec3(1,0.5,0.5);
     color=vec3(lightLevel);
}