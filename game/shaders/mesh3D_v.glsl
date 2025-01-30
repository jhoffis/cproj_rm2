#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

layout(location = 1) uniform mat4 mvp;
// layout(location = 2) uniform float aspect;
// layout(location = 3) uniform float img_ratio;
// layout(location = 4) uniform int anchor;

void main() {
    gl_Position = mvp * vec4(aPos.xyz, 1.0);
    // gl_Position *= mvp;

    TexCoord = aTexCoord;
}
