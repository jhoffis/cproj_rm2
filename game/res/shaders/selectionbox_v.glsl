// quad.vert
#version 460 core
layout(location = 0) in vec3 inPosition;

/* You can pass per-square data through the point if you wish, but
   for an exact port of your code we don’t need any. */
void main()
{
    gl_Position = vec4(inPosition, 1.0);   // required, even if unused
}

