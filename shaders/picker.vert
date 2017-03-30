#version 430 core

uniform mat4 modelView;
uniform mat4 projection;

layout (location = 0) in vec3 vertex;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 uv;


void main(void) {
    gl_Position = projection * modelView * vec4(vertex, 1.0); 
}
