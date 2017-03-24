#version 430 core

layout (location = 0) out int colour;

uniform int id;

void main(void) {    	
	colour = id;
}