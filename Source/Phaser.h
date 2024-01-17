#pragma once
class Phaser
{
public:

	Phaser();
	~Phaser();

	float dvc1(float input, float vc1, float LFO);

	void processPhaser(float* InAudio, float* OutAudio, float samplesToRender, double fs, const float* LFO);

private:

	float h;
	float vc1{ 0.0f };

};
