/* sample.glsl */


struct Sample {
	vec3 direction; //Direction to be used when sampling.
	float _padD;
	vec3 colour; //Colour computed for this direction.
	float _padC;
};
layout(std430, binding=0) buffer sampleSSBO {
	Sample samples[];
};


#ifdef SAMPLE_FUNCS
layout(std430, binding=1) buffer ringDataSSBO {
	ivec2 ringData[]; //X: Num samples in this ring  //Y: Ring start offset.
};

void getVertices(in vec3 sampleDir, out int i[3]) {
	//Convert to spherical coordinates.
	float theta = acos(clamp(sampleDir.z, -1.0f, 1.0f));
	float phi = atan(sampleDir.y, sampleDir.x);

	if (phi < 0.0) {phi += 2.0 * PI; /* Make it in the range 0-2π */}

	//Get which ring of samples;
	float t = theta / (PI * 0.5f);
	int ring = clamp(
		int(t*float(numberOfRings)),
		0, numberOfRings-1
	);
	//If on "equator", then use the next ring up.
	if (ring == numberOfRings-1) {
		ring = numberOfRings-2;
	}
	int count = ringData[ring].x;

	//Get pos inside this ring
	float u = phi / (2.0 * PI);
	float fIndex = u * float(count);
	float frac = fract(fIndex);

	int i0 = int(floor(fIndex));
	int i1 = (i0 + 1) % count;

	//If it's the top ring (the top vertex)
	if (ring == 0) {
		int base = 1;

		i[0] = 0;
		i[1] = base + i0;
		i[2] = base + i1;
		return;
	}

	//Otherwise, use this ring & next to make the tri.
	int nextRing = ring + 1;
	int countNext = ringData[nextRing].x;
	float ratio = float(countNext) / float(count);

	int j0 = clamp(
		int(floor(float(i0) * ratio)),
		0, countNext-1
	);
	int j1 = clamp(
		int(floor(float(i0 + 1) * ratio)),
		0, countNext-1
	);

	//Pick which tri & get verts.
	int base0 = ringData[ring].y;
	int base1 = ringData[nextRing].y;
	if (frac < 0.5f) {
		i[0] = base1 + j0 + 1;
		i[1] = base1 + j1 + 1;
		i[2] = base0 + i0 + 1;
	} else {
		i[0] = base0 + i1 + 1;
		i[1] = base0 + i0 + 1;
		i[2] = base1 + j1 + 1;
	}
}

void getVerticesHorizontal(in vec3 sampleDir, out int i[2], out float frac) {
	//Convert to spherical coordinates.
	float phi = atan(sampleDir.y, sampleDir.x);
	if (phi < 0.0) {phi += 2.0 * PI; /* Make it in the range 0-2π */}

	//Get pos inside this ring
	int count = ringData[numberOfRings-1].x;
	float u = phi / (2.0 * PI);
	float fIndex = u * float(count);
	frac = fract(fIndex);

	int i0 = int(floor(fIndex));
	int i1 = (i0 + 1) % count;

	//Otherwise, use this ring.
	int base0 = ringData[numberOfRings-1].y;
	i[0] = base0 + i0 + 1;
	i[1] = base0 + i1 + 1;
}


Sample sampleHemisphere(vec3 sampleDir) {
	int indices[3];
	sampleDir.z = abs(sampleDir.z);
	getVertices(sampleDir, indices);

	//Find closest sample to query.
	//Assume it's the first, but check the other 2.
	int bestIdx = indices[0];
	float bestDot = dot(sampleDir, samples[bestIdx].direction);

	for (int i=1; i<3; i++) {
	    float d = dot(sampleDir, samples[indices[i]].direction);
	    if (d > bestDot) { //Find closest to requested.
	        bestDot = d;
	        bestIdx = indices[i];
	    }
	}
	return samples[bestIdx];
}

vec3 sampleHemisphereHorizontal(vec3 sampleDir) {
	int indices[2];
	float frac;
	getVerticesHorizontal(sampleDir, indices, frac);

	//Blend between the 2 colours.
	return mix(
		samples[indices[0]].colour, samples[indices[1]].colour, frac
	);
}
#endif
