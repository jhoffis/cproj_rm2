#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor2;

layout(location = 1) uniform float poss; // we set this variable in the OpenGL code.

void main() {
    gl_Position = vec4(aPos.x + poss, aPos.y, aPos.z, 1.0);
    ourColor2 = aColor;
}
