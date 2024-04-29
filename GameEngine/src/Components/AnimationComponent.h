#pragma once


#include<SDL.h>


struct AnimationComponent {
	int numFrames;
	int currentFrame;
	int frameSpeedRate;
	bool isLoop;
	int startTime;

	AnimationComponent(int numFrames = 1, int frameSpeedRate = 1, bool isLoop = true) {
		this->numFrames = numFrames;
		this->frameSpeedRate = frameSpeedRate;
		this->isLoop = isLoop;
		this->currentFrame = 1;
		this->startTime = SDL_GetTicks();
	}
};