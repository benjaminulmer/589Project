#version 430 core

out vec3 colour;

uniform float idColour;

void main(void) {    	
	colour = vec3(idColour, idColour, idColour);
}