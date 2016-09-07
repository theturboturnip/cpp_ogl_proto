#version 330 core

uniform vec3 DiffuseTint;
uniform sampler2D DiffuseTex;

uniform sampler2D SLightDepthMap;
uniform vec3 SLightColor;
uniform float SLightSpotAttenStart;

in vec4 shadowmapPos;
in vec2 uv;
out vec3 color;

float LightLevel(){
    float shadowmapPosX=shadowmapPos.x/shadowmapPos.w/2.0f+0.5f;
     float shadowmapPosY=shadowmapPos.y/shadowmapPos.w/2.0f+0.5f;
     vec2 newUV=vec2(shadowmapPosX,shadowmapPosY);
     float renderedZ=texture2D(SLightDepthMap,newUV).r;
     float currentZ=shadowmapPos.z/shadowmapPos.w;
     float lightLevel=1,bias=0.005f;
     if(renderedZ<currentZ-bias)
         lightLevel=0;
     //If blurStart<1 it isn't a spotlight
     //Else use spotlight attenuation
     //if d=1 a=0
     //if d<=bs a=1
     //else a is lerped from 1 to 0 by 1-(d-bs)/(1-bs)
     //a=1-max(d-bs,0)/(1-bs)
     //if bs>1.5 a=1 no matter what
     float d=length(shadowmapPos.xy/shadowmapPos.w);
     float atten=1-max(d-SLightSpotAttenStart,0)/(1.0-SLightSpotAttenStart);
     return lightLevel*atten;
}

void main(){
    vec3 inColor=DiffuseTint*texture2D(DiffuseTex,uv).rgb;
    color=SLightColor*inColor*LightLevel();
    /*float ll=LightLevel();
    //color=inColor*ll;
    color=vec3(ll)*inColor;
    color.r = 1-color.r;
    color.r = 1-ll;*/
}
