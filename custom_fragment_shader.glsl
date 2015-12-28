#version 330 core 

in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

out vec3 color;

uniform sampler2D textureSampler;
uniform vec3 LightPosition_worldspace;

uniform vec3 LightColor=vec3(1,1,1),AmbientColor=vec3(1,1,1);
uniform float LightPower=10.0f,AmbientPower=0.2f;

void main(){
     vec3 MaterialDiffuseColor=texture(textureSampler, UV).rgb;
     vec3 MaterialAmbientColor=AmbientColor*AmbientPower*MaterialDiffuseColor;
     vec3 MaterialSpecularColor=LightColor*0.3;

     float distance=length(LightPosition_worldspace-Position_worldspace);

     vec3 normal=normalize(Normal_cameraspace);
     vec3 lightDirection=normalize(LightDirection_cameraspace);
     float LightLevel=clamp(dot(normal,lightDirection),0,1)/(distance*distance);
     //Deal only with diffuse and ambient
     color=MaterialAmbientColor+MaterialDiffuseColor*LightPower*LightColor*LightLevel;
}
