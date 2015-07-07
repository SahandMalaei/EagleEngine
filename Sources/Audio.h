#ifndef _AUDIO_H
#define _AUDIO_H 1

#include "Eagle.h"

#define TONE_SINE 0
#define TONE_SQUARE 1
#define TONE_SAWUP 2
#define TONE_SAWDOWN 3
#define TONE_TRIANGLE 4
#define TONE_WHITENOISE 5

#define AUDIO_DEFAULT_SOUND_FREQUENCY 44100.0f
#define AUDIO_DEFAULT_MINIMUM_DISTANCE 1.0f
#define AUDIO_DEFAULT_MAXIMUM_DISTANCE 10000.0f

namespace ProjectEagle
{
	class SoundSample
    {
    private:
	    std::string name;
		std::string category;
		float volume;
		float frequency;

		bool isPaused;

		// 2D

		float pan;

		// 3D

		bool is3D;
		Vector3 position;
		Vector3 lastPosition;
		float minimumDistance, maximumDistance;

		bool playedOnce;

	public:
		FMOD_SOUND *sample;
		FMOD_CHANNEL *channel;
		SoundSample(void);
		~SoundSample(void);
		std::string getName(){return name;}
		std::string getCategory(){return category;}
		void setName(std::string n){name = n;}
		void setCategory(std::string c){category = c;}
		void setVolume(float vol){volume = vol;}
		float getVolume(){return volume;}
		void setPan(float pan){this->pan = pan;}
		float getPan(){return pan;}
		void setFrequency(float frequency){this->frequency = frequency;}
		float getFrequency(){return frequency;}

		void setPaused(bool value){isPaused = value;}
		bool getPaused(){return isPaused;}

		void set3DValue(bool value){is3D = value;}
		bool get3DValue(){return is3D;}

		bool isPlayedOnce(){return playedOnce;}
		void setPlayedOnce(bool value){playedOnce = value;}

		//void enableDistanceFactor(){isDistanceFactorEnabled = 1;}
		//void disableDistanceFactor(){isDistanceFactorEnabled = 0;}
		//bool isDistanceBasedVolumeEnabled(){return isDistanceFactorEnabled;}

		//void enableDynamicPan(){isDynamicPanEnabled = 1;}
		//void disableDynamicPan(){isDynamicPanEnabled = 0;}
		//bool isDynamicDirectionEnabled(){return isDynamicPanEnabled;}

		//void lockPositionOnCamera(){isLockedOnCameraPosition = 1;}
		//void unlockPosition(){isLockedOnCameraPosition = 0;}
		//bool isPositionLockedOnCamera(){return isLockedOnCameraPosition;}

		void setPosition(Vector3 value){position.set(value);}
		void setPosition(float x, float y, float z){position.set(x, y, z);}
		Vector3 getPosition(){return position;}

		void setLastPosition(Vector3 value){lastPosition.set(value);}
		void setLastPosition(float x, float y, float z){lastPosition.set(x, y, z);}
		Vector3 getLastPosition(){return lastPosition;}

		//void setDistanceFactor(float value){distanceFactor = value;}
		//float getDistanceFactor(){return distanceFactor;}

		float getMinimumDistance(){return minimumDistance;}
		float getMaximumDistance(){return maximumDistance;}
		void setMinimumDistance(float value){minimumDistance = value;}
		void setMaximumDistance(float value){maximumDistance = value;}
	};

	class MachineTone
    {
    private:
	    std::string name;
		std::string category;
		float volume;
		float pan;
		float frequency;

	public:
		MachineTone(void);
		~MachineTone(void);

		FMOD_CHANNEL *channel;
		FMOD_DSP *dsp;
		DWORD type;

		std::string getName(){return name;}
		std::string getCategory(){return category;}
		void setName(std::string n){name = n;}
		void setCategory(std::string c){category = c;}

		void setVolume(float vol){volume = vol;}
		float getVolume(){return volume;}

		void setPan(float pan){this->pan = pan;}
		float getPan(){return pan;}

		void setFrequency(float frequency){this->frequency = frequency;}
		float getFrequency(){return frequency;}
	};

	class AudioSystem
	{
		friend class EagleEngine;

	private:
		FMOD_SYSTEM *system;

		typedef std::vector<SoundSample*> SoundSampleList;
		int sampleCount;
		typedef std::vector<MachineTone*> toneListVector;
		int toneCount;
		typedef std::vector<SoundSample*>::iterator Iterator;
		typedef std::vector<MachineTone*>::iterator ToneIterator;

		SoundSampleList sampleList;
		toneListVector toneList;

		Camera *camera;

		// Experimental

		bool playTone(std::string name);
		bool playRandomToneFromCategory(std::string category);
		bool playTone(MachineTone *tone);
		void stopTone(std::string name);
		void stopAllTones();
		void stopAllTonesExcept(std::string name);

		bool generateMachineSound(DWORD type, std::string name, std::string category = "default");

		bool isPlayingTone(std::string name);
		bool toneExists(std::string name);
		MachineTone *findTone(std::string  name);
		std::vector<MachineTone*> findTones(std::string category);

		void setTonePan(std::string name, float pan);
		float getTonePan(std::string name);

		void setToneVolume(std::string name, float volume);
		float getToneVolume(std::string name);
		void increaseToneVolume(std::string name, float value);
		void decreaseToneVolume(std::string name, float value);

		void setToneFrequency(std::string name, float frequency);
		float getToneFrequency(std::string name);
		void increaseToneFrequency(std::string name, float value);
		void decreaseToneFrequency(std::string name, float value);

		Vector3 listenerPosition;
		Vector3 listenerLastPosition;

		bool lockListenerOnCamera;

		FMOD_SYSTEM* getSystem(){return system;}

		bool initialize();
		void update();

	public:
		AudioSystem();
		~AudioSystem();

		int getSoundIndex(std::string name);
		std::string getSoundCategory(std::string name);

		int loadSound2D(std::string fileName, std::string name, std::string category = "Default");
		int loadSound3D(std::string fileName, std::string name, std::string category = "Default");

		void getSoundData(std::string name, short *data, int byteCount);
		//SoundSample* loadSound(std::string fileName);

		bool playSound(std::string name);
		bool playRandomSoundFromCategory(std::string category);
		bool playSound(SoundSample *sample);
		void pauseSound(std::string name);
		void unpauseSound(std::string name);
		void toggleSoundPause(std::string name);
		void stopSound(std::string name);
		void stopAllSounds();
		void stopAllSoundsExcept(std::string name);

		bool isSoundPlaying(std::string name);
		bool soundExists(std::string name);
		SoundSample *findSound(std::string  name);
		std::vector<SoundSample*> findSounds(std::string category);

		SoundSample *getSound(int index){return sampleList[index];}

		void setSoundPan(std::string name, float pan);
		float getSoundPan(std::string name);

		void setSoundVolume(std::string name, float volume);
		float getSoundVolume(std::string name);
		void increaseSoundVolume(std::string name, float value);
		void decreaseSoundVolume(std::string name, float value);

		void setSoundFrequency(std::string name, float frequency);
		float getSoundFrequency(std::string name);
		void increaseSoundFrequency(std::string name, float value);
		void decreaseSoundFrequency(std::string name, float value);

		void setSoundChannelPosition(std::string name, unsigned int milliseconds);
		int getSoundChannelPosition(std::string name);

		void setCameraAsListener(){lockListenerOnCamera = 1;}
		void disableCameraListener(){lockListenerOnCamera = 0;}
		void setCameraListenerLockState(bool state){lockListenerOnCamera = state;}

		void setListenerPosition(int x, int y, int z){lockListenerOnCamera = 0; listenerPosition.set(x, y, z);}
		void setListenerPosition(Vector3 value){lockListenerOnCamera = 0; listenerPosition.set(value);}
		Vector3 getListenerPosition(){return listenerPosition;}

		void setSoundMinimumDistance(std::string name, float minimumDistance);
		void setSoundMaximumDistance(std::string name, float maximumDistance);

		float getSoundMinimumDistance(std::string name);
		float getSoundMaximumDistance(std::string name);

		void setSoundPosition(std::string name, int x, int y, int z);
		void setSoundPosition(std::string name, Vector3 value);
		Vector3 getSoundPosition(std::string name);

		int startRecording(std::string soundName, std::string soundCategory, int soundLengthMilliseconds, bool loop);
		void stopRecording();

		// Index-based sound access

		void getSoundData(int index, short *data, int byteCount);

		std::string getSoundName(int index);
		std::string getSoundCategory(int index);

		bool playSound(int index);
		void pauseSound(int index);
		void unpauseSound(int index);
		void toggleSoundPause(int index);
		void stopSound(int index);
		void stopAllSoundsExcept(int index);

		bool isSoundPlaying(int index);

		void setSoundPan(int index, float pan);
		float getSoundPan(int index);

		void setSoundVolume(int index, float volume);
		float getSoundVolume(int index);
		void increaseSoundVolume(int index, float value);
		void decreaseSoundVolume(int index, float value);

		void setSoundFrequency(int index, float frequency);
		float getSoundFrequency(int index);
		void increaseSoundFrequency(int index, float value);
		void decreaseSoundFrequency(int index, float value);

		void setSoundChannelPosition(int index, unsigned int milliseconds);
		int getSoundChannelPosition(int index);

		void setSoundMinimumDistance(int index, float minimumDistance);
		void setSoundMaximumDistance(int index, float maximumDistance);

		float getSoundMinimumDistance(int index);
		float getSoundMaximumDistance(int index);

		void setSoundPosition(int index, int x, int y, int z);
		void setSoundPosition(int index, Vector3 value);
		Vector3 getSoundPosition(int index);

		char *getDriverName();

		void setDopplerScale(float value);
	};
};

#endif