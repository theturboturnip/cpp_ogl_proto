#version 330 core

layout(location = 0) in vec2 hud_screenspace;
out vec2 UV;

void main(){
    UV.x = hud_screenspace.x;
    UV.y = 1.0-hud_screenspace.y;
    gl_Position = vec4( hud_screenspace.x*2-1.0,
                        hud_screenspace.y*2-1.0,
                        0,1);
}

