#pragma once
#include <cmath>

#define M_PI 3.14159265358979323846

class LFO
{
public:

	LFO();
	~LFO();

	void initLFO(int numSamples, double sampleRate);
	void processLFO(float* writePtr, float frequency);
	float getNextSample();

private:

	int numSamples;
	double sampleRate;

	float angle{ 0.0f };
	float offset{ 0.0f };
	float frequency;
	float value{ 0.0f };
};