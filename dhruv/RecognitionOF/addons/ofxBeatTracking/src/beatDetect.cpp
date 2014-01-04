#include "beatDetect.h"
#include "fft.h"

beatDetect::beatDetect()
:iFftSize(512),
iBufferSize(1024)
{
}

void beatDetect::setup()
{
	for (int i = 0; i < iFftSize; i++) {
		fftSmoothed[i] = 0;
	}
	
	for (int i = 0; i < FFT_SUBBANDS; i++) {
		for (int l = 0; l < ENERGY_HISTORY; l++) {
			energyHistory[i][l] = 0;
		}
		fftSubbands[i] = 0;
		averageEnergy[i] = 0;
		fftVariance[i] = 0;
		beatValueArray[i] = 0;
	}
	
	audio_input = new float[iBufferSize];
	magnitude = new float[iFftSize];
	phase = new float[iFftSize];
	power = new float[iFftSize];
	magnitude_average = new float[iFftSize];
//	magnitude_average_snapshot = new float[iFftSize];
	
	for (int i = 0; i < iFftSize; i++) {
		magnitude[i] = 0;
		phase[i] = 0;
		power[i] = 0;
//		magnitude_average_snapshot[i] = 0;
		magnitude_average[i] = 0;
	}
	
	historyPos = 0;
	fftInit = true;
//	beatValue = 1.08;
	enableBeatDetect();
	printf("beatDetect setup OK!! \n");
}

//void beatDetect::updateFFT(float* in_fft, int infft_size)
void beatDetect::updateFFT()
{
	if (fftInit) {
		//fft = in_fft;
		in_fft = magnitude;
		for (int i = 0; i < iFftSize; i++) {
			// take the max, either the smoothed or the incoming:
			if (fftSmoothed[i] < in_fft[i]) {
				fftSmoothed[i] = in_fft[i];
			}
			// let the smoothed value sink to zero:
			fftSmoothed[i] *= 0.90f;
		}

		if (bDetectBeat) {
			for (int i = 0; i < FFT_SUBBANDS; i++) {
				fftSubbands[i] = 0;

				for (int b = 0; b < iFftSize / FFT_SUBBANDS; b++) {
					fftSubbands[i] +=  in_fft[i * (iFftSize / FFT_SUBBANDS) + b];
				}
				fftSubbands[i] = fftSubbands[i] * (float)FFT_SUBBANDS/(float)iFftSize;
				
				for (int b = 0; b < iFftSize / FFT_SUBBANDS; b++) {
					fftVariance[i] += pow(in_fft[i * (iFftSize / FFT_SUBBANDS) + b] - fftSubbands[i], 2);
				}
				fftVariance[i] = fftVariance[i] * (float)FFT_SUBBANDS / (float)iFftSize;
				
				beatValueArray[i] = (-0.0025714 * fftVariance[i]) + 1.35;
			}

			for (int i = 0; i < FFT_SUBBANDS; i++) {
				averageEnergy[i] = 0;
				for (int h = 0; h < ENERGY_HISTORY; h++) {
					averageEnergy[i] += energyHistory[i][h];
				}
				averageEnergy[i] /= ENERGY_HISTORY;
			}

			// put new values into energy history
			for (int i = 0; i < FFT_SUBBANDS; i++) {
				energyHistory[i][historyPos] = fftSubbands[i];
			}
			historyPos = (historyPos + 1) % ENERGY_HISTORY; // forward pointer and rotate if necessary
		}
	}
}

void beatDetect::audioReceived(float *input, int bufferSize)
{
	memcpy(audio_input, input, sizeof(float) * bufferSize);
	
	float avg_power = 0.0f;
	
	myfft.powerSpectrum(0, (int)iFftSize, audio_input, iBufferSize, magnitude, phase, power, &avg_power);
	
	for (int i = 0; i < iFftSize; i++) {
		magnitude[i] = powf(magnitude[i], 0.5);
	}

	for (int i = 0; i < iFftSize; i++) {
		float x = 0.085;
		magnitude_average[i] = (magnitude[i] * x) + (magnitude_average[i] * (1 - x));
	}
}

bool beatDetect::isBeat(int subband)
{
	return fftSubbands[subband] > averageEnergy[subband] * beatValueArray[subband];
}

bool beatDetect::isKick()
{
	return isBeat(0);
}

bool beatDetect::isSnare()
{
	int low = 1;
	int hi = FFT_SUBBANDS / 3;
	int thresh = (hi - low) / 3;
	return isBeatRange(low, hi, thresh);
}

bool beatDetect::isHat()
{
	int low = FFT_SUBBANDS / 2;
	int hi = FFT_SUBBANDS - 1;
	int thresh = (hi - low) / 3;
	return isBeatRange(low, hi, thresh);
}

bool beatDetect::isBeatRange(int low, int high, int threshold)
{
	int num = 0;
	for (int i = low; i < high + 1; i++) {
		if (isBeat(i)) {
			num++;
		}
	}
	return num > threshold;
}

float beatDetect::getMagnitude(int i)
{
	return magnitude[i];
}

float beatDetect::getMagnitudeAverage(int i)
{
	return magnitude_average[i];
}

int beatDetect::getFftSize()
{
	return iFftSize;
}

int beatDetect::getBufferSize()
{
	return iBufferSize;
}
