/* shell.vert */
#version 460 core

layout(location=0) in vec4 aPosition;
layout(location=1) in vec3 aNormal;

out vec2 positionXY;
out flat int layerIndex;
out float layerHeight;
out vec2 layerUV;
out vec3 normal;

uniform mat4 pvmMatrix;
uniform float layerSpacing;

void main() {

	positionXY = aPosition.xy;
	layerHeight = aPosition.z;
	layerIndex = int(aPosition.w);
	gl_Position = pvmMatrix * vec4(aPosition.xyz, 1.0f);
	layerUV = aPosition.xy;
	normal = normalize(aNormal);

}