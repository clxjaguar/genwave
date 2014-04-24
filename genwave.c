#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "genwave.h"

#define PI 3.1415926535897932384626433832795

int genwave_open(genwave_t **wv, const char *filename, const unsigned int channels, const unsigned int samplerate, const unsigned int bitspersample){
	unsigned char zero[4]={0, 0, 0, 0}, buf[4]={0, 0, 0, 0};
	unsigned int tmp;

	// allocating memory
	*wv = malloc(sizeof(genwave_t));
	if (!*wv) { return -1; }
	if (channels == 0){ return -1; }

	// opening file for output
	if (filename == NULL) {
		// stdout ?
		(*wv)->filename = malloc(strlen("(stdout)")+1);
		strcpy((*wv)->filename, "(stdout)");
		(*wv)->fp = stdout;
	}
	else {
		(*wv)->fp = fopen(filename, "wb");
		if (!(*wv)->fp) { free(*wv); *wv=NULL; return -2; }
		(*wv)->filename = malloc(strlen(filename)+1);
		if (!(*wv)->filename) { fclose((*wv)->fp); free(*wv); *wv=NULL; return -3; }
		strcpy((*wv)->filename, filename);
	}

	// memorizing informations
	(*wv)->channels = channels;
	(*wv)->samplerate = samplerate;
	(*wv)->bitspersample = bitspersample;
	(*wv)->samples = 0;
	(*wv)->lasttone_phase = 0.0;
	(*wv)->lasttone_amplitude = 0.0;

	if ((*wv)->bitspersample == 16) {
		(*wv)->maxamplitude = 32768;
	}
	else if ((*wv)->bitspersample == 8) {
		(*wv)->maxamplitude = 128;
	}
	else {
		fprintf(stderr, "Error: only 8 or 16 bits per sample is supported (you asked %u).\n", (*wv)->bitspersample);
        fclose((*wv)->fp); free(*wv); *wv=NULL;
		return -4;
	}

	// writing headers...
	fwrite("RIFF", 4, 1, (*wv)->fp); // .wav magic (4 bytes)
	if ((*wv)->fp == stdout){ // if stdout .. tell not zero since there will not are fseek() ?
		buf[0] = 0x24; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x80; // from ALSA's arecord
		fwrite(buf, 4, 1, (*wv)->fp);
	}
	else {
		fwrite(zero,   4, 1, (*wv)->fp); // filesize – 8 (4 bytes)
	}
	fwrite("WAVE", 4, 1, (*wv)->fp); // indicate the format of the data (4 bytes)
	fwrite("fmt ", 4, 1, (*wv)->fp); // this is the section of the file describing the format specifically (4 bytes)

	buf[0] = 16; buf[1] = 0; buf[2] = 0; buf[3] = 0;
	fwrite(buf,    4, 1, (*wv)->fp); // size of the WAVEFORMATEX data to follow (4 bytes)

	buf[0] =  1; buf[1] = 0;
	fwrite(buf,    2, 1, (*wv)->fp); // PCM format = 1 (2 bytes)

	buf[0] =  channels; buf[1] = channels>>8;
	fwrite(buf,    2, 1, (*wv)->fp); // nChannels (2 bytes)

	buf[0] = samplerate; buf[1] = samplerate>>8; buf[2] = samplerate>>16; buf[3] = samplerate>>24;
	fwrite(buf,    4, 1, (*wv)->fp); // nSamplesPerSec (4 bytes)

	tmp = (samplerate * bitspersample * channels) / 8;
	buf[0] = tmp; buf[1] = tmp>>8; buf[2] = tmp>>16; buf[3] = tmp>>24;
	fwrite(buf,    4, 1, (*wv)->fp); // (Sample Rate * BitsPerSample * Channels) / 8 (4 bytes)

	tmp = (bitspersample * channels) / 8;
	buf[0] =  tmp; buf[1] = tmp>>8;
	fwrite(buf,    2, 1, (*wv)->fp); // (BitsPerSample * Channels) / 8 (2 bytes)

	tmp = bitspersample;
	buf[0] =  tmp; buf[1] = tmp>>8;
	fwrite(buf,    2, 1, (*wv)->fp); // BitsPerSample (2 bytes)
	fwrite("data", 4, 1, (*wv)->fp); // "data" chunk header. Marks the beginning of the data section (4 bytes)
	if ((*wv)->fp == stdout){ // if streaming to stdout
		buf[0] = 0x00; buf[1] = 0x00; buf[2] = 0x00; buf[3] = 0x80; // from ALSA's arecord
		fwrite(buf,    4, 1, (*wv)->fp);
	}
	else {
		fwrite(zero,   4, 1, (*wv)->fp); // SubChunk2Size (4 bytes)
	}
	return 0;
}

int genwave_sample(genwave_t **wv, const int *sample){
	char buf[2];
	unsigned int i = (*wv)->channels, j=0;

	if ((*wv)->bitspersample == 16) {
		while(i--){
			if (sample[j] > 0x7FFF) { buf[1] = 127; buf[0] = 255; }
			else if (sample[j] < -0x8000) { buf[1] = 128; buf[0] = 0; }
			else { buf[0] = sample[j]%256; buf[1] = sample[j]>>8; }
			fwrite(buf, 2, 1, (*wv)->fp);
			j++;
		}
        (*wv)->samples++;
        return 0;
	}
	else if ((*wv)->bitspersample == 8) {
		while(i--){
			if (sample[j] > 127) { buf[0] = 255; }
			else if (sample[j] < -128) { buf[0] = 0; }
			else { buf[0] = sample[j] + 128; }
			fwrite(buf, 1, 1, (*wv)->fp);
			j++;
		}
		(*wv)->samples++;
		return 0;
	}
	return -1;
}

int genwave_close(genwave_t **wv){
	char buf[4]={0, 0, 0, 0};
	unsigned int tmp;

	if ((*wv)->fp != stdout){ // if NOT streaming to stdout
		tmp = ftell((*wv)->fp) - 8;
		buf[0] = tmp; buf[1] = tmp>>8; buf[2] = tmp>>16; buf[3] = tmp>>24;
		fseek((*wv)->fp,  4, SEEK_SET);
		fwrite(buf, 4, 1, (*wv)->fp);

		tmp = ((*wv)->bitspersample)/8 * (*wv)->samples * (*wv)->channels;//NumSamples * NumChannels * BitsPerSample/8
		buf[0] = tmp; buf[1] = tmp>>8; buf[2] = tmp>>16; buf[3] = tmp>>24;
		fseek((*wv)->fp, 40, SEEK_SET);
		fwrite(buf, 4, 1, (*wv)->fp);

		fclose((*wv)->fp);
		fprintf(stderr, "%s closed after %u sample(s) stuffed into it.\n", (*wv)->filename, (*wv)->samples);
	}
	else {
		fprintf(stderr, "%s got %u sample(s) stuffed on it.\n", (*wv)->filename, (*wv)->samples);
	}
	free((*wv)->filename);
	free(*wv);
	*wv = NULL;
	return -1;
}

int genwave_tone(genwave_t **wv, float frequency, float duration, float intensity){
	unsigned int i, j;
	float t, p, amp, samplerate;
	int sample[20];

    if ((*wv)->channels > 20) { return -1; }
	if (duration == 0) { return -1; }

	amp = (float)(*wv)->maxamplitude*intensity;
	samplerate = (float)(*wv)->samplerate;

	if (frequency == 0) { p = 0; } // ok to jump if frequency is zero
	else if ((*wv)->lasttone_amplitude > amp) { // minimal downward jump
        p = PI/2;
	}
	else if ((*wv)->lasttone_amplitude <-amp) { // minimal upward jump
        p = -PI/2;
	}
	else if ((*wv)->lasttone_phase > PI/2 && (*wv)->lasttone_phase < 3*PI/2) {
        p=-asinf((*wv)->lasttone_amplitude/amp)+PI;
	}
	else {
		p=asinf((*wv)->lasttone_amplitude/amp);
	}

	i=0;
	while((t=(float)i++/samplerate)<duration){
		sample[0] = amp*sinf(frequency*2*PI*t+p);
		j = (*wv)->channels; // j=2 if stereo
		while(--j) { sample[j] = sample[0]; }
		genwave_sample(wv, sample);
	}
	(*wv)->lasttone_phase = fmodf(frequency*2*PI*t+p, 2*PI);
	(*wv)->lasttone_amplitude = amp*sinf(frequency*2*PI*t+p);
	return 0;
}

int genwave_dtmf(genwave_t **wv, const char *tones, float duration, float intensity, float silenceduration){
	unsigned int i;
	float t, amp, samplerate, f1, f2;
	const char *c;
	int sample[2];

	amp = (float)(*wv)->maxamplitude*intensity;
	samplerate = (float)(*wv)->samplerate;
	if (duration == 0) { return -1; }

	c = &tones[0];
	while(*c){
		switch(*c) {
			case '1': f1 = 697; f2 = 1209; break;
			case '2': f1 = 697; f2 = 1336; break;
			case '3': f1 = 697; f2 = 1477; break;
			case 'A': f1 = 697; f2 = 1633; break;
			case '4': f1 = 770; f2 = 1209; break;
			case '5': f1 = 770; f2 = 1336; break;
			case '6': f1 = 770; f2 = 1477; break;
			case 'B': f1 = 770; f2 = 1633; break;
			case '7': f1 = 852; f2 = 1209; break;
			case '8': f1 = 852; f2 = 1336; break;
			case '9': f1 = 852; f2 = 1477; break;
			case 'C': f1 = 852; f2 = 1633; break;
			case '*': f1 = 941; f2 = 1209; break;
			case '0': f1 = 941; f2 = 1336; break;
			case '#': f1 = 941; f2 = 1477; break;
			case 'D': f1 = 941; f2 = 1633; break;

			default:
				f1 = 0; f2 = 0;
		}

		// DTMF tone
		i=0;
		while((t=(float)i++/samplerate)<duration){
			sample[0] = 0.5*amp*(sinf(f1*2*PI*t)+sinf(f2*2*PI*t));

			// if stereo, same signal on the two channels please
			if ((*wv)->channels == 2) { sample[1] = sample[0]; }
			genwave_sample(wv, sample);
		}

		// then generate some silence
		i=0; sample[0] = 0;
		if ((*wv)->channels == 2) { sample[1] = 0; }
		while(((float)i++/samplerate)<silenceduration){
			genwave_sample(wv, sample);
		}

		c++; // now next character
	}
	(*wv)->lasttone_phase = 0;
	(*wv)->lasttone_amplitude = 0;
	return 0;
}
