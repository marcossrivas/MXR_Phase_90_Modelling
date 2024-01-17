#include "LFO.h"

LFO::LFO()
{}

LFO::~LFO()
{}

void LFO::initLFO(int numSamples, double sampleRate)
{
	this->numSamples = numSamples;
	this->sampleRate = sampleRate;
}

void LFO::processLFO(float* writePtr, float frequency)
{
	offset = 2.0f * M_PI * (frequency / sampleRate);

	for (int i = 0; i < numSamples; ++i)
	{
		writePtr[i] = getNextSample();
	}
}

float LFO::getNextSample()
{
	value = (0.4*std::sin(angle)) + 0.43 ; //scale
	angle += offset;
	if (angle >= (2.0f * M_PI))
	{
		angle -= (2.0f * M_PI);
	}
	return value;
}