#version 330 core 

in vec2 UV;

out vec3 color;

uniform sampler2D textureSampler;

uniform vec3 LightColor=vec3(1,1,1),AmbientColor=vec3(1,1,1);
uniform float LightPower=1.0f,AmbientPower=0.9f;

void main(){
     vec3 MaterialDiffuseColor=texture(textureSampler, UV).rgb;
     vec3 MaterialAmbientColor=AmbientColor*AmbientPower*MaterialDiffuseColor;
     color=MaterialAmbientColor;
}
