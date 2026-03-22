/* sky.vert */
#version 460 core

out vec3 aColour;

struct Sample {
	vec3 direction; //Direction to be used when sampling.
	float _padD;
	vec3 colour; //Colour computed for this direction.
	float _padC;
};
layout(std430, binding=0) buffer sampleSSBO {
	Sample samples[];
};

uniform int numberOfSamples;
uniform mat4 pvmMatrix;


Sample getSample(int id) {
	if (id < numberOfSamples) { //Must be an actual sample to be fetched.
		return samples[id];
	} else {
		//Pseudo-sample, represents (0, 0, -1).
		//Used to close up below the horizon.
		Sample fakeSample;
		fakeSample.direction = vec3(0.0f, 0.0f, -1.0f);
		fakeSample.colour = vec3(0.0f, 0.0f, 0.0f);
		return fakeSample;
	}
}


void main() {
	Sample s = getSample(gl_VertexID);
    gl_Position = pvmMatrix * vec4(s.direction, 1.0f);
    aColour = s.colour;
}