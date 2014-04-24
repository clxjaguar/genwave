#include <stdio.h>
#include "genwave.h"

unsigned int ccir2freq(char tone){
	char ccircharacters[14] = { '1',  '2',  '3',  '4',  '5',  '6',  '7',  '8',  '9',  '0',  'A',  'E',  '/', '?'};
	int ccirfrequencies[14] = { 1124, 1197, 1275, 1358, 1446, 1540, 1640, 1747, 1860, 1981, 2400, 2110, 930, 0};
	unsigned int index = 0;
	for(;;){
		if (ccirfrequencies[index] == 0){ break; } // not found
		if (tone == ccircharacters[index]) { break; } // ok
		index++;
	}
	return ccirfrequencies[index];
}

int main(int argc, char **argv){
	char *p;
	int err;
	genwave_t *wave1;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s output.wav 1234567890AE/\n", argv[0]);
		return -1;
	}

	err = genwave_open(&wave1, argv[1], 1, 44100, 16);
	if (err) { return err; }

	genwave_tone(&wave1, 0,  0.5, 0); // pause 0.5s
	p = argv[2];
	while(*p){
		genwave_tone(&wave1, ccir2freq(*p++),  0.100, 1);
	}
	genwave_tone(&wave1, 0,  1, 0); // pause 1s
	genwave_close(&wave1);
	return 0;
}
