#version 430 core

out vec4 colour;

uniform sampler2D image;
uniform int hasTexture;
uniform vec4 objColour;
in vec3 N;
in vec3 L;
in vec3 V;
in vec2 UV;

void main(void) {    	

	float diffuse =  (dot(N, L) + 1) / 2;
	vec4 diffuseColour = diffuse * objColour;

	/***** Image-based texturing *****/
	vec4 imgColour = texture(image, UV);

	if (hasTexture > 0) {
		colour = diffuseColour * imgColour;
	}
	else {
		colour = diffuseColour;
	}
}