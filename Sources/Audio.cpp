#include "Eagle.h"

#ifndef PLATFORM_WP8
	#include <Include/FMOD/Windows/fmod.hpp>
#else
	#include <Include/FMOD/Windows Phone/fmod.hpp>
#endif


namespace ProjectEagle
{
	SoundSample::SoundSample()
	{
		sample = 0;
		channel = 0;
		volume = 1;
		pan = 0;
		frequency = AUDIO_DEFAULT_SOUND_FREQUENCY;

		isPaused = 0;

		//isDistanceFactorEnabled = 0;
		//isDynamicPanEnabled = 0;
		//isLockedOnCameraPosition = 1;

		is3D = 0;

		position.set(0, 0, 0);
		lastPosition.set(0, 0, 0);
		
		minimumDistance = AUDIO_DEFAULT_MINIMUM_DISTANCE;
		maximumDistance = AUDIO_DEFAULT_MAXIMUM_DISTANCE;

		playedOnce = 0;
	}

	SoundSample::~SoundSample()
	{
		if(sample != 0)
		{
			FMOD_Sound_Release(sample);

			sample = 0;
		}
	}

	MachineTone::MachineTone()
	{
		dsp = 0;
		channel = 0;
		volume = 1;
		pan = 0;
		type = 0;
		frequency = AUDIO_DEFAULT_SOUND_FREQUENCY;
	}

	MachineTone::~MachineTone()
	{
		if(dsp != 0)
		{
			FMOD_DSP_Release(dsp);

			dsp = 0;
		}
	}

	AudioSystem::AudioSystem()
	{
		system = 0;
		sampleCount = 0;

		lockListenerOnCamera = 1;
		listenerPosition.set(0, 0, 0);
	}

	AudioSystem::~AudioSystem()
	{
		stopAllSounds();

		for(Iterator i = sampleList.begin(); i != sampleList.end(); ++i)
		{
			(*i) = 0;
		}

		FMOD_System_Release(system);
	}

	bool AudioSystem::initialize()
	{
		unsigned int version;
		int numdrivers;
		FMOD_SPEAKERMODE speakermode;
		FMOD_CAPS caps;

		FMOD_RESULT result;

		result = FMOD_System_Create(&system);

		if(result != FMOD_OK)
		{
			return 0;
		}

		if(result != FMOD_OK)
		{
			return 0;
		}

		result = FMOD_System_GetVersion(system, &version);

		if(version < FMOD_VERSION) 
		{
			return 0;
		}
		
		result = FMOD_System_GetNumDrivers(system, &numdrivers);
		
		if(result != FMOD_OK)
		{
			return 0;
		}

		if(numdrivers == 0)
		{
			result = FMOD_System_SetOutput(system, FMOD_OUTPUTTYPE_NOSOUND);

			if(result != FMOD_OK)
			{
				return 0;
			}
		}

		result = FMOD_System_Init(system, 256, FMOD_INIT_NORMAL, 0);

		if(result != FMOD_OK)
		{
			return 0;
		}

		result = FMOD_System_Set3DSettings(system, 1.0f, 1.0f, 0.01f);

		if(result != FMOD_OK)
		{
			return 0;
		}

		camera = graphics.getCamera();

		return 1;
	}

	void AudioSystem::update()
	{
		FMOD_VECTOR fmodListenerPosition, fmodListenerVelocity, listenerForward, listenerUp;

		Vector3 listenerVelocity = listenerPosition - listenerLastPosition;
		fmodListenerVelocity.x = listenerVelocity.x;
		fmodListenerVelocity.y = listenerVelocity.y;
		fmodListenerVelocity.z = listenerVelocity.z;

		if(lockListenerOnCamera)
		{
			fmodListenerPosition.x = camera->getPosition().x;
			fmodListenerPosition.y = camera->getPosition().y;
			fmodListenerPosition.z = camera->getPosition().z;

			listenerForward.x = -camera->getLook().x;
			listenerForward.y = camera->getLook().y;
			listenerForward.z = camera->getLook().z;

			listenerUp.x = camera->getUp().x;
			listenerUp.y = camera->getUp().y;
			listenerUp.z = camera->getUp().z;
		}
		else
		{
			fmodListenerPosition.x = listenerPosition.x;
			fmodListenerPosition.y = listenerPosition.y;
			fmodListenerPosition.z = listenerPosition.z;

			listenerForward.x = 0;
			listenerForward.y = 0;
			listenerForward.z = -1;

			listenerUp.x = 0;
			listenerUp.y = -1;
			listenerUp.z = 0;
		}

		FMOD_System_Set3DListenerAttributes(system, 0, &fmodListenerPosition, &fmodListenerVelocity, &listenerForward, &listenerUp);

		listenerLastPosition = listenerPosition;

		for(int i = 0; i < sampleCount; ++i)
		{
			//break;

			SoundSample *sample = sampleList[i];

			if(!sample->get3DValue()) continue;

			FMOD_VECTOR position, velocity;

			Vector3 samplePosition, sampleLastPosition;

			samplePosition = sample->getPosition();
			sampleLastPosition = sample->getLastPosition();

			position.x = samplePosition.x;
			position.y = samplePosition.y;
			position.z = samplePosition.z;

			velocity.x = samplePosition.x - sampleLastPosition.x;
			velocity.y = samplePosition.y - sampleLastPosition.y;
			velocity.z = samplePosition.z - sampleLastPosition.z;

			FMOD_Channel_Set3DAttributes(sample->channel, &position, &velocity);

			//sample->setPan(pan);

			//Debug::throwMessage("Pan : " + FLOAT_TO_STRING(pan));

			sample->setLastPosition(samplePosition);
		}

		FMOD_System_Update(system);
	}

	int AudioSystem::loadSound2D(std::string fileName, std::string name, std::string category)
	{
		if(fileName.length() == 0 || name.length() == 0) return 0;

		SoundSample *sample = new SoundSample();
		sample->setName(name);
		sample->setCategory(category);

		FMOD_RESULT r;

		r = FMOD_System_CreateSound(system, fileName.c_str(), FMOD_2D | FMOD_LOOP_NORMAL, 0, &sample->sample);

		if(r != FMOD_OK)
		{
			Debug::throwError("Error loading sample from file " + fileName + ". Error code : " + INT_TO_STRING(r));

			return -1;
		}

		sampleList.push_back(sample);

		//playSound(sampleCount);
		//stopSound(sampleCount);

		sampleCount++;

		return sampleCount - 1;
	}

	int AudioSystem::loadSound3D(std::string fileName, std::string name, std::string category)
	{
		if(fileName.length() == 0 || name.length() == 0) return 0;

		SoundSample *sample = new SoundSample();
		sample->setName(name);
		sample->setCategory(category);

		FMOD_RESULT r;

		r = FMOD_System_CreateSound(system, fileName.c_str(), FMOD_3D | FMOD_LOOP_NORMAL, 0, &sample->sample);

		if(r != FMOD_OK)
		{
			Debug::throwError("Error loading sample from file " + fileName + ". Error code : " + INT_TO_STRING(r));

			return -1;
		}

		FMOD_Sound_Set3DMinMaxDistance(sample->sample, AUDIO_DEFAULT_MINIMUM_DISTANCE, AUDIO_DEFAULT_MAXIMUM_DISTANCE);

		sample->set3DValue(1);

		sampleList.push_back(sample);

		//playSound(sampleCount);
		//stopSound(sampleCount);

		sampleCount++;

		return sampleCount - 1;
	}

	bool AudioSystem::generateMachineSound(DWORD type, std::string name, std::string category)
	{
		MachineTone *tone = new MachineTone();
		tone->setName(name);
		tone->setCategory(category);
		tone->type = type;

		if(FMOD_System_CreateDSPByType(system, FMOD_DSP_TYPE_OSCILLATOR, &tone->dsp) != FMOD_OK)
		{
			Debug::throwError("Error in tone generation", "Sound Error");
			return 0;
		}

		if(FMOD_DSP_SetParameter(tone->dsp, FMOD_DSP_OSCILLATOR_RATE, 440.0f) != FMOD_OK)
			return 0;

		toneList.push_back(tone);
		return 1;
	}

	// Sounds

	bool AudioSystem::soundExists(std::string name)
	{
		for(Iterator i = sampleList.begin(); i != sampleList.end(); ++i)
		{
			if((*i)->getName() == name)
			{
				return 1;
			}
		}
		return 0;
	}

	bool AudioSystem::isSoundPlaying(std::string name)
	{
		SoundSample *sample = findSound(name);
		if(sample == 0) return 0;

		if(!sample->isPlayedOnce()) return 0;

		int index;
		FMOD_Channel_GetIndex(sample->channel, &index);

		//free((void *)sample);

		return (index > 0);
	}

	bool AudioSystem::isSoundPlaying(int index)
	{
		SoundSample *sample = sampleList[index];
		if(sample == 0) return 0;

		if(!sample->isPlayedOnce()) return 0;

		int channelIndex;
		FMOD_Channel_GetIndex(sample->channel, &channelIndex);

		//free((void *)sample);

		return (channelIndex > 0);
	}

	SoundSample* AudioSystem::findSound(std::string name)
	{
		SoundSample *sample = 0;
		for(Iterator i = sampleList.begin(); i != sampleList.end(); ++i)
		{
			if((*i)->getName() == name)
			{
				sample = (*i);
				break;
			}
		}
		return sample;
	}

	std::vector<SoundSample*> AudioSystem::findSounds(std::string category)
	{
		std::vector<SoundSample*> s;
		for(Iterator i = sampleList.begin(); i != sampleList.end(); ++i)
		{
			if((*i)->getCategory() == category)
			{
				s.push_back(*i);
			}
		}
		return s;
	}

	bool AudioSystem::playSound(std::string name)
	{
		FMOD_RESULT r;
		SoundSample *sample = findSound(name);
		if(!sample) return 0;
		if(sample->sample != 0)
		{
			sample->setPlayedOnce(1);

			r = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sample->sample, 1, &sample->channel);
			if(r != FMOD_OK) return 0;
			FMOD_Channel_SetLoopCount(sample->channel, 0);
			FMOD_Channel_SetPaused(sample->channel, 0);

			if(FMOD_Channel_SetPan(sample->channel, sample->getPan()) != FMOD_OK)
				Debug::throwError("Sound Error in setPan, error code : " + std::to_string((long float)FMOD_Channel_SetPan(sample->channel, sample->getPan())));
			if(FMOD_Channel_SetVolume(sample->channel, sample->getVolume()) != FMOD_OK)
				Debug::throwError("Sound Error in setVolume, error code : " + std::to_string((long float)FMOD_Channel_SetVolume(sample->channel, sample->getVolume())));
			if(FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency()) != FMOD_OK)
				Debug::throwError("Sound Error in setFrequency, error code : " + std::to_string((long float)FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency())));
		}

		return 1;
	}

	bool AudioSystem::playSound(SoundSample *sample)
	{
		FMOD_RESULT r;
		if(sample == 0) return 0;
		if(sample->sample == 0) return 0;

		sample->setPlayedOnce(1);

		r = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sample->sample, 1, &sample->channel);
		if(r != FMOD_OK) return 0;
		FMOD_Channel_SetLoopCount(sample->channel, 0);
		FMOD_Channel_SetPaused(sample->channel, 0);
		if(FMOD_Channel_SetPan(sample->channel, sample->getPan()) != FMOD_OK)
			Debug::throwError("Sound Error in setPan, error code : " + std::to_string((long float)FMOD_Channel_SetPan(sample->channel, sample->getPan())));
		if(FMOD_Channel_SetVolume(sample->channel, sample->getVolume()) != FMOD_OK)
			Debug::throwError("Sound Error in setVolume, error code : " + std::to_string((long float)FMOD_Channel_SetVolume(sample->channel, sample->getVolume())));
		if(FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency()) != FMOD_OK)
			Debug::throwError("Sound Error in setFrequency, error code : " + std::to_string((long float)FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency())));

		return 1;
	}

	void AudioSystem::pauseSound(std::string name)
	{
		if(!isSoundPlaying(name)) return;

		SoundSample *sample = findSound(name);
		//if(sample == 0) return;
		FMOD_Channel_SetPaused(sample->channel, 1);

		sample->setPaused(1);
	}

	void AudioSystem::unpauseSound(std::string name)
	{
		if(!isSoundPlaying(name)) return;

		SoundSample *sample = findSound(name);
		//if(sample == 0) return;
		FMOD_Channel_SetPaused(sample->channel, 0);

		sample->setPaused(0);
	}

	void AudioSystem::toggleSoundPause(std::string name)
	{
		if(!isSoundPlaying(name)) return;

		SoundSample *sample = findSound(name);
		//if(sample == 0) return;
		FMOD_Channel_SetPaused(sample->channel, !sample->getPaused());

		sample->setPaused(!sample->getPaused());
	}

	void AudioSystem::stopSound(std::string name)
	{
		if(!isSoundPlaying(name)) return;

		SoundSample *sample = findSound(name);
		//if(sample == 0) return;
		FMOD_Channel_Stop(sample->channel);
	}

	void AudioSystem::stopAllSounds()
	{
		for(Iterator i = sampleList.begin(); i != sampleList.end(); ++i)
		{
			FMOD_Channel_Stop((*i)->channel);
		}
	}

	void AudioSystem::stopAllSoundsExcept(std::string name)
	{
		for(Iterator i = sampleList.begin(); i != sampleList.end(); ++i)
		{
			if((*i)->getName() != name)
			{
				FMOD_Channel_Stop((*i)->channel);
			}
		}
	}

	bool AudioSystem::playRandomSoundFromCategory(std::string category)
	{
		std::string name = findSounds(category)[rand() % findSounds(category).size()]->getName();

		if(!findSound(name)) return 0;

		if(!isSoundPlaying(name)) playSound(name);
		return 1;
	}

	void AudioSystem::setSoundPan(std::string name, float pan)
	{
		SoundSample *sample = findSound(name);

		if(sample == 0) return;

		if(FMOD_Channel_SetPan(sample->channel, pan) != FMOD_OK)
			Debug::throwError("Sound Error in setPan, error code : " + std::to_string((long float)FMOD_Channel_SetPan(sample->channel, pan)));
		sample->setPan(pan);
	}

	float AudioSystem::getSoundPan(std::string name)
	{
		SoundSample *sample = findSound(name);
		return sample->getPan();
	}

	void AudioSystem::setSoundVolume(std::string name, float volume)
	{
		SoundSample *sample = findSound(name);

		if(sample == 0) return;

		if(FMOD_Channel_SetVolume(sample->channel, volume) != FMOD_OK)
			Debug::throwError("Sound Error in setVolume, error code : " + std::to_string((long float)FMOD_Channel_SetVolume(sample->channel, sample->getVolume())));
		sample->setVolume(volume);
	}

	float AudioSystem::getSoundVolume(std::string name)
	{
		SoundSample *sample = findSound(name);

		if(sample == 0) return 0;

		return sample->getVolume();
	}

	void AudioSystem::increaseSoundVolume(std::string name, float value)
	{
		SoundSample *sample = findSound(name);

		if(sample == 0) return;

		if(FMOD_Channel_SetVolume(sample->channel, sample->getVolume() + value) != FMOD_OK)
			Debug::throwError("Sound Error in setVolume, error code : " + std::to_string((long float)FMOD_Channel_SetVolume(sample->channel, sample->getVolume())));
		sample->setVolume(sample->getVolume() + value);
	}

	void AudioSystem::decreaseSoundVolume(std::string name, float value)
	{
		SoundSample *sample = findSound(name);
		
		if(sample == 0) return;

		if(FMOD_Channel_SetVolume(sample->channel, sample->getVolume() - value) != FMOD_OK)
			Debug::throwError("Sound Error in setVolume, error code : " + std::to_string((long float)FMOD_Channel_SetVolume(sample->channel, sample->getVolume())));
		sample->setVolume(sample->getVolume() - value);
	}

	void AudioSystem::setSoundFrequency(std::string name, float frequency)
	{
		SoundSample *sample = findSound(name);
		
		if(sample == 0) return;

		if(FMOD_Channel_SetFrequency(sample->channel, frequency) != FMOD_OK)
			Debug::throwError("Sound Error in setFrequency, error code : " + std::to_string((long float)FMOD_Channel_SetFrequency(sample->channel, frequency)));
		sample->setFrequency(frequency);
	}

	float AudioSystem::getSoundFrequency(std::string name)
	{
		SoundSample *sample = findSound(name);
		return sample->getFrequency();
	}

	void AudioSystem::increaseSoundFrequency(std::string name, float value)
	{
		SoundSample *sample = findSound(name);
		
		if(sample == 0) return;

		if(FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency() + value) != FMOD_OK)
			Debug::throwError("Sound Error in setFrequency, error code : " + std::to_string((long float)FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency() + value)));
		sample->setFrequency(sample->getFrequency() + value);
	}

	void AudioSystem::decreaseSoundFrequency(std::string name, float value)
	{
		SoundSample *sample = findSound(name);
		
		if(sample == 0) return;

		if(FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency() - value) != FMOD_OK)
			Debug::throwError("Sound Error in setFrequency, error code : " + std::to_string((long float)FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency() - value)));
		sample->setFrequency(sample->getFrequency() - value);
	}

	// Index-based sound access

	bool AudioSystem::playSound(int index)
	{
		FMOD_RESULT r;
		SoundSample *sample = sampleList[index];
		if(!sample) return 0;
		if(sample->sample != 0)
		{
			r = FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, sample->sample, 1, &sample->channel);
			if(r != FMOD_OK) return 0;
			FMOD_Channel_SetLoopCount(sample->channel, -1);
			FMOD_Channel_SetPaused(sample->channel, 0);

			if(FMOD_Channel_SetPan(sample->channel, sample->getPan()) != FMOD_OK)
				Debug::throwError("Sound Error in setPan, error code : " + std::to_string((long float)FMOD_Channel_SetPan(sample->channel, sample->getPan())));
			if(FMOD_Channel_SetVolume(sample->channel, sample->getVolume()) != FMOD_OK)
				Debug::throwError("Sound Error in setVolume, error code : " + std::to_string((long float)FMOD_Channel_SetVolume(sample->channel, sample->getVolume())));
			if(FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency()) != FMOD_OK)
				Debug::throwError("Sound Error in setFrequency, error code : " + std::to_string((long float)FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency())));
		}
		//free((void *)sample);
		return 1;
	}

	void AudioSystem::pauseSound(int index)
	{
		if(!isSoundPlaying(index)) return;

		SoundSample *sample = sampleList[index];
		//if(sample == 0) return;
		FMOD_Channel_SetPaused(sample->channel, 1);

		sample->setPaused(1);
	}

	void AudioSystem::unpauseSound(int index)
	{
		if(!isSoundPlaying(index)) return;

		SoundSample *sample = sampleList[index];
		//if(sample == 0) return;
		FMOD_Channel_SetPaused(sample->channel, 0);

		sample->setPaused(0);
	}

	void AudioSystem::toggleSoundPause(int index)
	{
		if(!isSoundPlaying(index)) return;

		SoundSample *sample = sampleList[index];
		//if(sample == 0) return;
		FMOD_Channel_SetPaused(sample->channel, !sample->getPaused());

		sample->setPaused(!sample->getPaused());
	}

	void AudioSystem::stopSound(int index)
	{
		if(!isSoundPlaying(index)) return;

		SoundSample *sample = sampleList[index];
		//if(sample == 0) return;
		FMOD_Channel_Stop(sample->channel);
	}

	void AudioSystem::stopAllSoundsExcept(int index)
	{
		int counter = -1;
		for(Iterator i = sampleList.begin(); i != sampleList.end(); ++i)
		{
			counter++;

			if(counter != index)
			{
				FMOD_Channel_Stop((*i)->channel);
			}
		}
	}

	void AudioSystem::setSoundPan(int index, float pan)
	{
		SoundSample *sample = sampleList[index];
		//if(sample == 0) return;
		if(FMOD_Channel_SetPan(sample->channel, pan) != FMOD_OK)
			Debug::throwError("Sound Error in setPan, error code : " + std::to_string((long float)FMOD_Channel_SetPan(sample->channel, pan)));
		sample->setPan(pan);
	}

	float AudioSystem::getSoundPan(int index)
	{
		SoundSample *sample = sampleList[index];
		return sample->getPan();
	}

	void AudioSystem::setSoundVolume(int index, float volume)
	{
		SoundSample *sample = sampleList[index];
		//if(sample == 0) return;
		if(FMOD_Channel_SetVolume(sample->channel, volume) != FMOD_OK)
			Debug::throwError("Sound Error in setVolume, error code : " + std::to_string((long float)FMOD_Channel_SetVolume(sample->channel, sample->getVolume())));
		sample->setVolume(volume);
	}

	float AudioSystem::getSoundVolume(int index)
	{
		SoundSample *sample = sampleList[index];
		//if(sample == 0) return;
		return sample->getVolume();
	}

	void AudioSystem::increaseSoundVolume(int index, float value)
	{
		SoundSample *sample = sampleList[index];
		//if(sample == 0) return;
		if(FMOD_Channel_SetVolume(sample->channel, sample->getVolume() + value) != FMOD_OK)
			Debug::throwError("Sound Error in setVolume, error code : " + std::to_string((long float)FMOD_Channel_SetVolume(sample->channel, sample->getVolume())));
		sample->setVolume(sample->getVolume() + value);
	}

	void AudioSystem::decreaseSoundVolume(int index, float value)
	{
		SoundSample *sample = sampleList[index];
		//if(sample == 0) return;
		if(FMOD_Channel_SetVolume(sample->channel, sample->getVolume() - value) != FMOD_OK)
			Debug::throwError("Sound Error in setVolume, error code : " + std::to_string((long float)FMOD_Channel_SetVolume(sample->channel, sample->getVolume())));
		sample->setVolume(sample->getVolume() - value);
	}

	void AudioSystem::setSoundFrequency(int index, float frequency)
	{
		SoundSample *sample = sampleList[index];
		//if(sample == 0) return;
		if(FMOD_Channel_SetFrequency(sample->channel, frequency) != FMOD_OK)
			Debug::throwError("Sound Error in setFrequency, error code : " + std::to_string((long float)FMOD_Channel_SetFrequency(sample->channel, frequency)));
		sample->setFrequency(frequency);
	}

	float AudioSystem::getSoundFrequency(int index)
	{
		SoundSample *sample = sampleList[index];
		return sample->getFrequency();
	}

	void AudioSystem::increaseSoundFrequency(int index, float value)
	{
		SoundSample *sample = sampleList[index];
		//if(sample == 0) return;
		if(FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency() + value) != FMOD_OK)
			Debug::throwError("Sound Error in setFrequency, error code : " + std::to_string((long float)FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency() + value)));
		sample->setFrequency(sample->getFrequency() + value);
	}

	void AudioSystem::decreaseSoundFrequency(int index, float value)
	{
		SoundSample *sample = sampleList[index];
		//if(sample == 0) return;
		if(FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency() - value) != FMOD_OK)
			Debug::throwError("Sound Error in setFrequency, error code : " + std::to_string((long float)FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency() - value)));
		sample->setFrequency(sample->getFrequency() - value);
	}

	// Tones

	bool AudioSystem::toneExists(std::string name)
	{
		for(ToneIterator i = toneList.begin(); i != toneList.end(); ++i)
		{
			if((*i)->getName() == name)
			{
				return 1;
			}
		}
		return 0;
	}

	bool AudioSystem::isPlayingTone(std::string name)
	{
		MachineTone *sample = findTone(name);
		if(sample == 0) return 0;

		int index;
		FMOD_Channel_GetIndex(sample->channel, &index);

		//free((void *)sample);

		return (index > 0);
	}

	MachineTone* AudioSystem::findTone(std::string name)
	{
		MachineTone *sample = 0;
		for(ToneIterator i = toneList.begin(); i != toneList.end(); ++i)
		{
			if((*i)->getName() == name)
			{
				sample = (*i);
				break;
			}
		}
		return sample;
	}

	std::vector<MachineTone*> AudioSystem::findTones(std::string category)
	{
		std::vector<MachineTone*> s;
		for(ToneIterator i = toneList.begin(); i != toneList.end(); ++i)
		{
			if((*i)->getCategory() == category)
			{
				s.push_back(*i);
			}
		}
		return s;
	}

	bool AudioSystem::playTone(std::string name)
	{
		FMOD_RESULT r;
		MachineTone *sample = findTone(name);
		if(!sample) return 0;
		if(sample->dsp != 0)
		{
			r = FMOD_System_PlayDSP(system, FMOD_CHANNEL_FREE, sample->dsp, 1, &sample->channel);
			if(r != FMOD_OK) return 0;
			r = FMOD_DSP_SetParameter(sample->dsp, FMOD_DSP_OSCILLATOR_TYPE, sample->type);
			if(r != FMOD_OK) return 0;

			FMOD_Channel_SetLoopCount(sample->channel, -1);
			FMOD_Channel_SetPaused(sample->channel, 0);

			if(FMOD_Channel_SetPan(sample->channel, sample->getPan()) != FMOD_OK)
				Debug::throwError("Sound Error in setPan, error code : " + std::to_string((long float)FMOD_Channel_SetPan(sample->channel, sample->getPan())));
			if(FMOD_Channel_SetVolume(sample->channel, sample->getVolume()) != FMOD_OK)
				Debug::throwError("Sound Error in setVolume, error code : " + std::to_string((long float)FMOD_Channel_SetVolume(sample->channel, sample->getVolume())));
			if(FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency()) != FMOD_OK)
				Debug::throwError("Sound Error in setFrequency, error code : " + std::to_string((long float)FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency())));
		}
		//free((void *)sample);
		return 1;
	}

	bool AudioSystem::playTone(MachineTone *sample)
	{
		FMOD_RESULT r;
		if(sample == 0) return 0;
		if(sample->dsp == 0) return 0;

		r = FMOD_System_PlayDSP(system, FMOD_CHANNEL_FREE, sample->dsp, 1, &sample->channel);
		if(r != FMOD_OK) return 0;
		r = FMOD_DSP_SetParameter(sample->dsp, FMOD_DSP_OSCILLATOR_TYPE, sample->type);
		if(r != FMOD_OK) return 0;

		FMOD_Channel_SetLoopCount(sample->channel, -1);
		FMOD_Channel_SetPaused(sample->channel, 0);
		if(FMOD_Channel_SetPan(sample->channel, sample->getPan()) != FMOD_OK)
			Debug::throwError("Sound Error in setPan, error code : " + std::to_string((long float)FMOD_Channel_SetPan(sample->channel, sample->getPan())));
		if(FMOD_Channel_SetVolume(sample->channel, sample->getVolume()) != FMOD_OK)
			Debug::throwError("Sound Error in setVolume, error code : " + std::to_string((long float)FMOD_Channel_SetVolume(sample->channel, sample->getVolume())));
		if(FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency()) != FMOD_OK)
			Debug::throwError("Sound Error in setFrequency, error code : " + std::to_string((long float)FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency())));

		return 1;
	}

	void AudioSystem::stopTone(std::string name)
	{
		if(!isPlayingTone(name)) return;

		MachineTone *sample = findTone(name);
		//if(sample == 0) return;
		FMOD_Channel_Stop(sample->channel);
	}

	void AudioSystem::stopAllTones()
	{
		for(ToneIterator i = toneList.begin(); i != toneList.end(); ++i)
		{
			FMOD_Channel_Stop((*i)->channel);
		}
	}

	void AudioSystem::stopAllTonesExcept(std::string name)
	{
		for(ToneIterator i = toneList.begin(); i != toneList.end(); ++i)
		{
			if((*i)->getName() != name)
			{
				FMOD_Channel_Stop((*i)->channel);
			}
		}
	}

	bool AudioSystem::playRandomToneFromCategory(std::string category)
	{
		std::string name = findTones(category)[rand() % findTones(category).size()]->getName();
		if(!findTone(name)) return 0;
		if(!isPlayingTone(name)) playTone(name);
		return 1;
	}

	void AudioSystem::setTonePan(std::string name, float pan)
	{
		MachineTone *sample = findTone(name);
		//if(sample == 0) return;
		if(FMOD_Channel_SetPan(sample->channel, pan) != FMOD_OK)
			Debug::throwError("Sound Error in setPan, error code : " + std::to_string((long float)FMOD_Channel_SetPan(sample->channel, pan)));
		sample->setPan(pan);
	}

	float AudioSystem::getTonePan(std::string name)
	{
		MachineTone *sample = findTone(name);
		return sample->getPan();
	}

	void AudioSystem::setToneVolume(std::string name, float volume)
	{
		MachineTone *sample = findTone(name);
		//if(sample == 0) return;
		if(FMOD_Channel_SetVolume(sample->channel, volume) != FMOD_OK)
			Debug::throwError("Sound Error in setVolume, error code : " + std::to_string((long float)FMOD_Channel_SetVolume(sample->channel, sample->getVolume())));
		sample->setVolume(volume);
	}

	float AudioSystem::getToneVolume(std::string name)
	{
		MachineTone *sample = findTone(name);
		//if(sample == 0) return;
		return sample->getVolume();
	}

	void AudioSystem::increaseToneVolume(std::string name, float value)
	{
		MachineTone *sample = findTone(name);
		//if(sample == 0) return;
		if(FMOD_Channel_SetVolume(sample->channel, sample->getVolume() + value) != FMOD_OK)
			Debug::throwError("Sound Error in setVolume, error code : " + std::to_string((long float)FMOD_Channel_SetVolume(sample->channel, sample->getVolume())));
		sample->setVolume(sample->getVolume() + value);
	}

	void AudioSystem::decreaseToneVolume(std::string name, float value)
	{
		MachineTone *sample = findTone(name);
		//if(sample == 0) return;
		if(FMOD_Channel_SetVolume(sample->channel, sample->getVolume() - value) != FMOD_OK)
			Debug::throwError("Sound Error in setVolume, error code : " + std::to_string((long float)FMOD_Channel_SetVolume(sample->channel, sample->getVolume())));
		sample->setVolume(sample->getVolume() - value);
	}

	void AudioSystem::setToneFrequency(std::string name, float frequency)
	{
		MachineTone *sample = findTone(name);
		//if(sample == 0) return;
		if(FMOD_Channel_SetFrequency(sample->channel, frequency) != FMOD_OK)
			Debug::throwError("Sound Error in setFrequency, error code : " + std::to_string((long float)FMOD_Channel_SetFrequency(sample->channel, frequency)));
		sample->setFrequency(frequency);
	}

	float AudioSystem::getToneFrequency(std::string name)
	{
		MachineTone *sample = findTone(name);
		return sample->getFrequency();
	}

	void AudioSystem::increaseToneFrequency(std::string name, float value)
	{
		MachineTone *sample = findTone(name);
		//if(sample == 0) return;
		if(FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency() + value) != FMOD_OK)
			Debug::throwError("Sound Error in setFrequency, error code : " + std::to_string((long float)FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency() + value)));
		sample->setFrequency(sample->getFrequency() + value);
	}

	void AudioSystem::decreaseToneFrequency(std::string name, float value)
	{
		MachineTone *sample = findTone(name);
		//if(sample == 0) return;
		if(FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency() - value) != FMOD_OK)
			Debug::throwError("Sound Error in setFrequency, error code : " + std::to_string((long float)FMOD_Channel_SetFrequency(sample->channel, sample->getFrequency() - value)));
		sample->setFrequency(sample->getFrequency() - value);
	}

	void AudioSystem::setSoundMinimumDistance(std::string name, float minimumDistance)
	{
		SoundSample *sample = findSound(name);

		sample->setMinimumDistance(minimumDistance);

		FMOD_Sound_Set3DMinMaxDistance(sample->sample, minimumDistance, sample->getMaximumDistance());
	}

	void AudioSystem::setSoundMaximumDistance(std::string name, float maximumDistance)
	{
		SoundSample *sample = findSound(name);

		sample->setMaximumDistance(maximumDistance);

		FMOD_Sound_Set3DMinMaxDistance(sample->sample, sample->getMinimumDistance(), maximumDistance);
	}

	float AudioSystem::getSoundMinimumDistance(std::string name)
	{
		SoundSample *sample = findSound(name);

		return sample->getMinimumDistance();
	}

	float AudioSystem::getSoundMaximumDistance(std::string name)
	{
		SoundSample *sample = findSound(name);

		return sample->getMaximumDistance();
	}

	void AudioSystem::setSoundMinimumDistance(int index, float minimumDistance)
	{
		SoundSample *sample = sampleList[index];

		sample->setMinimumDistance(minimumDistance);

		FMOD_Sound_Set3DMinMaxDistance(sample->sample, minimumDistance, sample->getMaximumDistance());
	}

	void AudioSystem::setSoundMaximumDistance(int index, float maximumDistance)
	{
		SoundSample *sample = sampleList[index];

		sample->setMaximumDistance(maximumDistance);

		FMOD_Sound_Set3DMinMaxDistance(sample->sample, sample->getMinimumDistance(), maximumDistance);
	}

	float AudioSystem::getSoundMinimumDistance(int index)
	{
		SoundSample *sample = sampleList[index];

		return sample->getMinimumDistance();
	}

	float AudioSystem::getSoundMaximumDistance(int index)
	{
		SoundSample *sample = sampleList[index];

		return sample->getMaximumDistance();
	}

	void AudioSystem::setSoundPosition(std::string name, int x, int y, int z)
	{
		SoundSample *sample = findSound(name);

		if(!sample) return;

		sample->setPosition(x, y, z);
	}

	void AudioSystem::setSoundPosition(std::string name, Vector3 value)
	{
		SoundSample *sample = findSound(name);

		if(!sample) return;

		sample->setPosition(value);
	}

	Vector3 AudioSystem::getSoundPosition(std::string name)
	{
		SoundSample *sample = findSound(name);

		if(!sample) return Vector3(0, 0, 0);

		return sample->getPosition();
	}

	void AudioSystem::setSoundPosition(int index, int x, int y, int z)
	{
		SoundSample *sample = sampleList[index];

		if(!sample) return;

		sample->setPosition(x, y, z);
	}

	void AudioSystem::setSoundPosition(int index, Vector3 value)
	{
		SoundSample *sample = sampleList[index];

		if(!sample) return;

		sample->setPosition(value);
	}

	Vector3 AudioSystem::getSoundPosition(int index)
	{
		SoundSample *sample = sampleList[index];

		if(!sample) return Vector3(0, 0, 0);

		return sample->getPosition();
	}

	char *AudioSystem::getDriverName()
	{
		char *name = new char[128];

		FMOD_System_GetDriverInfo(system, 0, name, 128, 0);

		return name;
	}

	void AudioSystem::setDopplerScale(float value)
	{
		FMOD_System_Set3DSettings(system, value, 1.0f, 0.01f);
	}

	int AudioSystem::getSoundIndex(std::string name)
	{
		int index = 0;

		for(Iterator i = sampleList.begin(); i != sampleList.end(); ++i)
		{
			if((*i)->getName() == name)
			{
				return index;
			}

			index++;
		}

		return -1;
	}

	std::string AudioSystem::getSoundCategory(std::string name)
	{
		for(Iterator i = sampleList.begin(); i != sampleList.end(); ++i)
		{
			if((*i)->getName() == name)
			{
				return (*i)->getCategory();
			}
		}

		return "NULL";
	}

	std::string AudioSystem::getSoundName(int index)
	{
		SoundSample *sample = sampleList[index];

		if(!sample) return "NULL";

		return sample->getName();
	}

	std::string AudioSystem::getSoundCategory(int index)
	{
		SoundSample *sample = sampleList[index];

		if(!sample) return "NULL";

		return sample->getCategory();
	}

	int AudioSystem::startRecording(std::string soundName, std::string soundCategory, int soundLengthMilliseconds, bool loop)
	{
		SoundSample *sample;
		sample = new SoundSample();
		sample->setName(soundName);
		sample->setCategory(soundCategory);

		FMOD_CREATESOUNDEXINFO exinfo;

		memset(&exinfo, 0, sizeof(FMOD_CREATESOUNDEXINFO));

		exinfo.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
		exinfo.numchannels = 2;
		exinfo.format = FMOD_SOUND_FORMAT_PCM16;
		exinfo.defaultfrequency = 44100;
		exinfo.length = exinfo.defaultfrequency * sizeof(short) * exinfo.numchannels * soundLengthMilliseconds / 1000.0;

		FMOD_System_CreateSound(system, 0, FMOD_2D | FMOD_OPENUSER, &exinfo, &sample->sample);


		unsigned int length0, length1;

		void *dat0, *dat1;

		FMOD_Sound_Lock(sample->sample, 0, 44100 * soundLengthMilliseconds / 1000.0, &dat0, &dat1, &length0, &length1);

		//Debug::throwMessage(INT_TO_STRING(len0) + ", " + INT_TO_STRING(len1));
		//FMOD_Sound_ReadData(sound, data, 1024);

		//if(dat0) graphics.drawPoint(100, *(int *)(dat0) / 5000000.0 + 200, colors.White);

		for(int i = 0; i < length0; i += 4)
		{
			int *a = (int *)(dat0) + i;
			*a = 0;
			//console.print(INT_TO_STRING(*a));
		}

		FMOD_Sound_Unlock(sample->sample, dat0, dat1, length0, length1);


		FMOD_System_RecordStart(system, 0, sample->sample, loop);


		sampleList.push_back(sample);

		playSound(sampleCount);
		stopSound(sampleCount);

		sampleCount++;

		return sampleCount - 1;
	}

	void AudioSystem::stopRecording()
	{
		FMOD_System_RecordStop(system, 0);
	}

	void AudioSystem::getSoundData(std::string name, short *data, int byteCount)
	{
		SoundSample *sample = findSound(name);
		if(!sample) return;

		unsigned int len0, len1;

		void *dat0, *dat1;

		//44100
		FMOD_Sound_Lock(sample->sample, 0, byteCount, &dat0, &dat1, &len0, &len1);

		//Debug::throwMessage(INT_TO_STRING(len0) + ", " + INT_TO_STRING(len1));
		//FMOD_Sound_ReadData(sound, data, 1024);

		//if(dat0) graphics.drawPoint(100, *(int *)(dat0) / 5000000.0 + 200, colors.White);

		for(int i = 0; i < len0; i += 4)
		{
			data[i / 4] = *((short *)(dat0) + i);
		}

		FMOD_Sound_Unlock(sample->sample, dat0, dat1, len0, len1);
	}

	void AudioSystem::getSoundData(int index, short *data, int byteCount)
	{
		SoundSample *sample = sampleList[index];
		if(!sample) return;

		unsigned int len0, len1;

		void *dat0, *dat1;

		//44100
		FMOD_Sound_Lock(sample->sample, 0, byteCount, &dat0, &dat1, &len0, &len1);

		//Debug::throwMessage(INT_TO_STRING(len0) + ", " + INT_TO_STRING(len1));
		//FMOD_Sound_ReadData(sound, data, 1024);

		//if(dat0) graphics.drawPoint(100, *(int *)(dat0) / 5000000.0 + 200, colors.White);

		for(int i = 0; i < len0; i += 4)
		{
			data[i / 4] = *((short *)(dat0) + i);
		}

		FMOD_Sound_Unlock(sample->sample, dat0, dat1, len0, len1);
	}

	void AudioSystem::setSoundChannelPosition(std::string name, unsigned int milliseconds)
	{
		SoundSample *sample = findSound(name);

		FMOD_Channel_SetPosition(sample->channel, milliseconds, FMOD_TIMEUNIT_MS);
	}

	int AudioSystem::getSoundChannelPosition(std::string name)
	{
		SoundSample *sample = findSound(name);

		unsigned int channelPosition;

		FMOD_Channel_GetPosition(sample->channel, &channelPosition, FMOD_TIMEUNIT_MS);

		return channelPosition;
	}

	void AudioSystem::setSoundChannelPosition(int index, unsigned int milliseconds)
	{
		SoundSample *sample = sampleList[index];
		if(!sample) return;

		FMOD_Channel_SetPosition(sample->channel, milliseconds, FMOD_TIMEUNIT_MS);
	}

	int AudioSystem::getSoundChannelPosition(int index)
	{
		SoundSample *sample = sampleList[index];
		if(!sample) return 0;

		unsigned int channelPosition;

		FMOD_Channel_GetPosition(sample->channel, &channelPosition, FMOD_TIMEUNIT_MS);

		return channelPosition;
	}

	int AudioSystem::getSoundLoopCount(std::string name)
	{
		SoundSample *sample = findSound(name);
		if(!sample)
		{
			return 0;
		}

		int loopCount = 0;
		FMOD_Channel_GetLoopCount(sample->channel, &loopCount);

		return loopCount;
	}

	void AudioSystem::setSoundLoopCount(std::string name, int count)
	{
		SoundSample *sample = findSound(name);
		if(!sample)
		{
			return;
		}

		FMOD_Channel_SetLoopCount(sample->channel, count);
	}
};