/*
 * gpx2_format_unit_test.cpp
 *
 *  Created on: 12 Jun 2022
 *      Author: pgm
 */


#include <stdio.h>
#include "gpx2_format.h"

void mask_test(){
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_SITE_MASK,  GPX_BITS::SITE);
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_CHAN_MASK,  GPX_BITS::CHAN);
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_FLAGS_MASK, GPX_BITS::FLAGS);
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_NREF_MASK,  GPX_BITS::NREF);
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_STOP_MASK,  GPX_BITS::STOP);

	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_PPSSIG_MASK,  GPX_BITS::PPSSIG);
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_PPSREF_MASK,  GPX_BITS::PPSREF);
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_PPSTAI_MASK,  GPX_BITS::PPSTAI);
}

int main()
{
	mask_test();
	return 0;
}


