#include <stdio.h>
#include <stdlib.h>
#include "genwave.h"

int main(int argc, char **argv){
	genwave_t *wave1;
	genwave_open(&wave1, NULL, 2, 11025, 8); // NULL filenames means "write to stdout"
	fprintf(stderr, "Generating endless wav file to stdout...\n");
	for(;;){
		genwave_tone(&wave1, 0, 0.200, 0.5);
		genwave_tone(&wave1, 334, 0.100, 0.5); genwave_tone(&wave1, 0, 0.050, 0.5);
		genwave_tone(&wave1, 334, 0.100, 0.5); genwave_tone(&wave1, 0, 0.050, 0.5);
		genwave_tone(&wave1, 0, 0.150, 0.5);
		genwave_tone(&wave1, 334, 0.100, 0.5); genwave_tone(&wave1, 0, 0.050, 0.5);
		genwave_tone(&wave1, 0, 0.150, 0.5);
		genwave_tone(&wave1, 261, 0.100, 0.5); genwave_tone(&wave1, 0, 0.050, 0.5);
		genwave_tone(&wave1, 334, 0.100, 0.5); genwave_tone(&wave1, 0, 0.050, 0.5);
		genwave_tone(&wave1, 0, 0.150, 0.5);
		genwave_tone(&wave1, 392, 0.200, 0.5); genwave_tone(&wave1, 0, 0.100, 0.5);
		genwave_tone(&wave1, 0, 0.400, 0.5);
		genwave_tone(&wave1, 196, 0.300, 0.5);
		genwave_tone(&wave1, 0, 0.300, 0.5);
	}
	genwave_close(&wave1);
	return 0;
}
