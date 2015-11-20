#include "Sound.h"

sound::sound()
{
	buffer.loadFromFile("LASRHVY2.wav");
	sound_effect.setBuffer(buffer);
	sound_effect.setVolume(100);
}
