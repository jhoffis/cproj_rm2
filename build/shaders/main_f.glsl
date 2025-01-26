#version 460 core
out vec4 FragColor;

in vec3 ourColor2;

layout(location = 0) uniform vec4 ourColor; // we set this variable in the OpenGL code.

void main() {
    FragColor = vec4(mix(ourColor.rgb, ourColor2, .5), 1);
} 
