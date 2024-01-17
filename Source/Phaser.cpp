#include "Phaser.h"

Phaser::Phaser()
{}
Phaser::~Phaser()
{}

float Phaser::dvc1(float input, float vc1, float LFO)
{
	float dy = -((-input + vc1) / (LFO * 0.001128));
	return dy;
}

void Phaser::processPhaser(float* InAudio, float* OutAudio, float samplesToRender, double fs, const float* LFO)
{
	h = (1.0f / fs);

	for (int i = 0; i < samplesToRender; i++) 
	{
		float input = InAudio[i];
		float k1 = dvc1(input, vc1, LFO[i]);
		float n1 = vc1 + (h * k1);
		float k2 = dvc1(input, n1, LFO[i]);
		vc1 = vc1 + (0.5 * h * (k1 + k2));

		OutAudio[i] = (input - (2 * vc1));
	}
}