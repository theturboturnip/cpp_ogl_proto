#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

// Ouput data
layout(location = 0) out vec4 color;

// Values that stay constant for the whole mesh.
uniform sampler2D textureSampler;
void main(){

	color = vec4(texture( textureSampler, UV ).rgb,0.7);
}