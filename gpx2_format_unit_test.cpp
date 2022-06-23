/*
 * gpx2_format_unit_test.cpp
 *
 *  Created on: 12 Jun 2022
 *      Author: pgm
 */


#include <stdio.h>
#include "gpx2_format.h"

void le_madness(void)
{
	//                         0123456789abcdef
	unsigned long long tai = 0x000012345678abcdULL;
	char *b48 = (char*)&tai + 4;
	fprintf(stderr, "%16llx: ", tai);
	for (int ii = 0; ii < 6; ++ii){
		fprintf(stderr, "%02x", b48[ii]);
	}
	fprintf(stderr, "\n");
}
void mask_test(){
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_SITE_MASK,  GPX_BITS::SITE);
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_CHAN_MASK,  GPX_BITS::CHAN);
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_FLAGS_MASK, GPX_BITS::FLAGS);
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_NREF_MASK,  GPX_BITS::NREF);
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_STOP_MASK,  GPX_BITS::STOP);

	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_PPSSIG_MASK,  GPX_BITS::PPSSIG);
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_PPSREF_MASK,  GPX_BITS::PPSREF);
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_PPSTAI_MASK,  GPX_BITS::PPSTAI);

	le_madness();
}

int main()
{
	mask_test();
	return 0;
}


