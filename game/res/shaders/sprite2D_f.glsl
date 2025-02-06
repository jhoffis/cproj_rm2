#version 460 core
out vec4 FragColor;

in vec3 ourColor2;
in vec2 TexCoord;

uniform sampler2D ourTexture;

layout(location = 0) uniform vec4 ourColor; // we set this variable in the OpenGL code.
layout(location = 6) uniform int applyColor; // we set this variable in the OpenGL code.

void main() {
    FragColor = texture(ourTexture, TexCoord);
    if (applyColor == 1) {
        FragColor = mix(FragColor, ourColor, 0.2);
    } else if (applyColor == 2) {
        FragColor = mix(FragColor, vec4(0,0,0,1), 0.5);
    }
} 
