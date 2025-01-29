#version 460 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormals;
layout (location = 2) in vec2 aTexCoord;

out vec2 TexCoord;

// layout(location = 1) uniform vec2 pos;
// layout(location = 2) uniform float aspect;
// layout(location = 3) uniform float img_ratio;
// layout(location = 4) uniform int anchor;

void main() {
    gl_Position = vec4(aPos.xyz, 1.0);

    TexCoord = aTexCoord;
}
