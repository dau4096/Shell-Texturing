/* clouds.shared.glsl */
//File with functions to be shared between all of the "cloud" related shaders.
//Automatically pre-processes "#include <clouds.shared>" into the entire source in this file.



float cloudNoiseOctaves(vec2 position) {
    float result = 0.0f;
    float amplitude = 0.5f;
    float frequency = 1.0f;

    for (int i = 0; i < 5; i++) {
        result += amplitude * cnoise(position * frequency);
        frequency *= 2.0;
        amplitude *= 0.5;
    }

    return result;
}



float getCloudValueForPosition(vec2 position, float time) {
	float noiseValue = cloudNoiseOctaves(
	    (position + WIND_DIRECTION * CLOUD_NOISE_SPEED * time) / CLOUD_NOISE_SIZE
	);

	noiseValue = noiseValue * 0.5f + 0.5f;
	noiseValue = pow(noiseValue, 1.5f);   //Sharper edges
	float erosion = cnoise(position * 4.0 + time);
	noiseValue -= erosion * 0.02f;
	noiseValue *= smoothstep(0.2, 0.8, noiseValue);

	float limitedValue = (noiseValue - CLOUD_MIN_VALUE) / (CLOUD_MAX_VALUE - CLOUD_MIN_VALUE);
	return clamp(limitedValue, 0.0f, 1.0f);
}