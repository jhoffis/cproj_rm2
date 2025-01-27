#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor2;
out vec2 TexCoord;

layout(location = 1) uniform vec2 pos;
layout(location = 2) uniform float aspect;
layout(location = 3) uniform float img_ratio;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0, 1.0);
    gl_Position.x *= aspect;
    gl_Position.x *= img_ratio;
    gl_Position.x += 2*pos.x - 1; // So that origo is at the bottom left and that it goes from 0 -> 1
    gl_Position.y += 2*pos.y - 1;

    ourColor2 = aColor;
    TexCoord = aTexCoord;
}
