/*
 * gpx2_format_unit_test.cpp
 *
 *  Created on: 12 Jun 2022
 *      Author: pgm
 */


#include <stdio.h>
#include "gpx2_format.h"

void mask_test(){
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_SITE_MASK, bit(GPX_SITE_MASK));
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_CHAN_MASK, bit(GPX_CHAN_MASK));
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_NREF_MASK, bit(GPX_NREF_MASK));
	fprintf(stderr, "mask_test %016llx shl %d\n", GPX_STOP_MASK, bit(GPX_STOP_MASK));
}

int main()
{
	mask_test();
	return 0;
}


