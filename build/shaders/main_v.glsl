#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

out vec3 ourColor2;
out vec2 TexCoord;

layout(location = 1) uniform float poss; // we set this variable in the OpenGL code.

void main() {
    gl_Position = vec4(aPos.x + poss, aPos.y, aPos.z, 1.0);
    ourColor2 = aColor;
    TexCoord = aTexCoord;
}
