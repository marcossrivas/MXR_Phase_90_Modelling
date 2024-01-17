#include "Mix.h"

Mix::Mix()
{}

void Mix::processMix(float* inAudio, float* inPhaser, float* outAudio, int samplesToRender, float mixValue) 
{
	for (int i = 0; i < samplesToRender; i++)
	{
		outAudio[i] = inAudio[i] + (mixValue * inPhaser[i]);
	}
}

Mix::~Mix()
{}


