/* display.frag */
#version 460 core

in vec2 fragUV;
out vec4 fragColour;

layout(binding=0) uniform sampler2D frame;

void main() {
	//Simply outputs the frame's colour.
	fragColour = vec4(texture(frame, fragUV.xy).rgb, 1.0f);
}
