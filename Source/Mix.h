#pragma once
class Mix
{
public:

	Mix();

	void processMix(float* inAudio, float* inPhaser, float* outAudio, int samplesToRender, float mixValue);

	~Mix();
};

