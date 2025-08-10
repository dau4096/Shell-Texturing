/* shell.vert */
#version 460 core

layout(location=0) in vec3 aPos;
layout(location=1) in vec2 aUV;

out flat int layerIndex;
out flat float layerHeight;
out vec2 layerUV;

uniform mat4 pvmMatrix;
uniform float layerSpacing;

void main() {

	layerHeight = aPos.z;
	layerIndex = int(aPos.z / layerSpacing);
	gl_Position = pvmMatrix * vec4(aPos.xyz, 1.0f);
	layerUV = aUV;

}