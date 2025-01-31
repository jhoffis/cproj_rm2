#version 460 core
out vec4 FragColor;

in vec3 ourColor2;
in vec2 TexCoord;

uniform sampler2D ourTexture;

layout(location = 0) uniform vec4 ourColor; // we set this variable in the OpenGL code.

void main() {
    FragColor = texture(ourTexture, TexCoord);
} 
