/* shell.vert */
#version 460 core

layout(location=0) in vec3 aPosition;
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
	layerIndex = int(gl_InstanceID);
	float zOffset = (layerIndex*layerSpacing);
	layerHeight = zOffset + aPosition.z;
	gl_Position = pvmMatrix * vec4(positionXY, layerHeight, 1.0f);
	layerUV = aPosition.xy;
	normal = aNormal;

}