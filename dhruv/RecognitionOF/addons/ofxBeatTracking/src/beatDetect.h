#ifndef _BEAT_DETECT
#define _BEAT_DETECT

#include "ofMain.h"
#include "fft.h"

#include <list>

#define FFT_BINS 512
#define FFT_SUBBANDS 32
#define ENERGY_HISTORY 43

using namespace std;

class beatDetect
{
public:
	beatDetect();
	virtual ~beatDetect() {}

	void setup();
	// FFT functions
//	void initFFT();
	void updateFFT();
	//void updateFFT(float* in_fft, int infft_size);
//	void drawSmoothedFFT();
//	void drawSubbands();
	//void drawBeats();
	void audioReceived(float* input, int bufferSize);
	
	// detect beats
	void enableBeatDetect() {bDetectBeat = true;}
	void disableBeatDetect() {bDetectBeat = false;}
	
	bool isBeat(int subband);
	bool isKick();
	bool isSnare();
	bool isHat();
	bool isBeatRange(int low, int high, int threshold);
//	void setBeatValue(float bv) {beatValue = bv;}
	
	float getMagnitude(int i);
	float getMagnitudeAverage(int i);
	
	int getFftSize();
	int getBufferSize();
	
private:
	ofSoundPlayer soundtrack;
	fft myfft;
	
	// fft & beat detection
	float fftSmoothed[FFT_BINS];
	float fftSubbands[FFT_SUBBANDS];
	float averageEnergy[FFT_SUBBANDS];
	float fftVariance[FFT_SUBBANDS];
	float beatValueArray[FFT_SUBBANDS];
	float energyHistory[FFT_SUBBANDS][ENERGY_HISTORY];
	
	float *in_fft;
//	float beatValue;
	int historyPos;
	bool fftInit;
	bool bDetectBeat;
	
	// for fft object
	float *magnitude, *phase, *power, *audio_input;
	float *magnitude_average;
//	float *magnitude_average_snapshot;
	
	int iFftSize;
	int iBufferSize;
};

#endif	// _BEAT_DETECT
