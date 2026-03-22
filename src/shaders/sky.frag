/* sky.frag */
#version 460 core

in vec3 aColour;
out vec4 fragColour;

void main() {
	fragColour = vec4(aColour.rgb, 1.0f);
}