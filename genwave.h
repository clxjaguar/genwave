typedef struct {
	FILE *fp;
	char *filename;
	unsigned int channels;
	unsigned int samplerate;
	unsigned int bitspersample;
	unsigned int samples;
	unsigned int maxamplitude;
	float lasttone_phase;
	float lasttone_amplitude;
} genwave_t;

int genwave_open(genwave_t **wv, const char *filename, const unsigned int channels, const unsigned int samplerate, const unsigned int bitspersample);
int genwave_sample(genwave_t **wv, const int *sample);
int genwave_close(genwave_t **wv);
int genwave_tone(genwave_t **wv, float frequency, float duration, float intensity);
int genwave_dtmf(genwave_t **wv, const char *tones, float duration, float intensity, float silenceduration);
