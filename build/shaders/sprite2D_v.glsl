#version 460 core
layout (location = 0) in vec3 aPos;
// layout (location = 1) in float apsect;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor2;
out vec2 TexCoord;

layout(location = 1) uniform vec3 pos;
layout(location = 2) uniform float aspect;

void main() {
    gl_Position = vec4(aPos.x, aPos.y, 0, 1.0);
    gl_Position.x += pos.x;
    gl_Position.y += -pos.y;
    gl_Position.x /= aspect;
    gl_Position.x *= 2.;
    gl_Position.y *= 2.;
    gl_Position.x -= 1.;
    gl_Position.y += 1.;

    ourColor2 = aColor;
    TexCoord = aTexCoord;
}
