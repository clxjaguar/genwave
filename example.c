#include <stdio.h>
#include <stdlib.h>
#include "genwave.h"

int main(int argc, char **argv){
	int i, j;
	genwave_t *wave0, *wave1;

	genwave_open(&wave0, "example_jointdemo.wav", 1, 44100, 16);
	genwave_open(&wave1, "example.wav", 2, 22050, 16);

	genwave_tone(&wave1, 440.0, 3, 0.4);
	genwave_dtmf(&wave1, "0324587458", 0.1, 0.5, 0.05);
	genwave_tone(&wave1, 0, 2, 0);

	// v23 for genwave_tone() demo !
	genwave_tone(&wave1, 1500.0, 0.5, 3);
	for (j=0; j<10; j++){
		i = rand()%3000;
		while(i--){
			if (rand()&1) { genwave_tone(&wave1, 1300.0, 0.000833, 1); }
			else {          genwave_tone(&wave1, 2100.0, 0.000833, 1); }
		}
		genwave_tone(&wave1, 1700.0, (float)(rand()%50)/1000.0, 1);
	}
	genwave_tone(&wave1, 440.0, 0.75, 0.4);
	genwave_tone(&wave1, 440.0, 0.75, 0);
	genwave_tone(&wave1, 440.0, 0.75, 0.4);
	genwave_tone(&wave1, 440.0, 0.75, 0);
	genwave_tone(&wave1, 440.0, 0.75, 0.4);
	genwave_tone(&wave1, 440.0, 0.75, 0);
	genwave_tone(&wave0, 1000.0, 0.0007, 1.0);
	genwave_tone(&wave0, 3000.0, 0.0005, 0.5);
	genwave_tone(&wave0, 1000.0, 0.0012, 0.8);
	genwave_tone(&wave0, 2000.0, 0.0004, 0.5);
	genwave_tone(&wave0, 2000.0, 0.0004, 0.3);
	genwave_tone(&wave0, 0.0,    0.0004, 0.0);

	genwave_close(&wave0);
	genwave_close(&wave1);
	return 0;
}
