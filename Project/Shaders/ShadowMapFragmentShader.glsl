#version 330 core

uniform vec3 DiffuseTint;
uniform sampler2D DiffuseTex;

uniform sampler2D SLightDepthMap;
uniform vec3 SLightColor;

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
     return lightLevel;
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
