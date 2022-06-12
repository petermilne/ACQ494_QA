/*
 * gpx2_decode.cpp
 *
 *  Created on: 10 Jun 2022
 *      Author: pgm
 */
/* ------------------------------------------------------------------------- */
/*   Copyright (C) 2022 Peter Milne, D-TACQ Solutions Ltd
 *                      <Peter dot Milne at D hyphen TACQ dot com>

    http://www.d-tacq.com

    This program is free software; you can redistribute it and/or modify
    it under the terms of Version 2 of the GNU General Public License
    as published by the Free Software Foundation;

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                */
/* ------------------------------------------------------------------------- */


#include <stdio.h>
#include "popt.h"

#include "gpx2_format.h"

namespace G {		/* put any globals in here */
	int verbose = 0;
};

struct poptOption opt_table[] = {
	{
	  "verbose", 'd', POPT_ARG_INT, &G::verbose, 0, "set debug level"
	},
	POPT_AUTOHELP
	POPT_TABLEEND
};


int decode(void)
{
	unsigned long long tmp;
	int event = 0;
	while(fread(&tmp, sizeof(long long), 1, stdin) == 1){
		if (G::verbose > 1){
			fprintf(stderr, "%d,%016llx\n", event, tmp);
		}
		unsigned sc, nref, stop;
		gpx_from_raw(tmp, sc, nref, stop);
		if (G::verbose){
			fprintf(stderr, "%6d,%2d,%8d,%8d\n", event, sc, nref, stop);
		}
		event += 1;
	}
	return 0;
}


void ui(int argc, const char** argv)
{
	poptContext opt_context =
			poptGetContext(argv[0], argc, argv, opt_table, 0);

	int rc;

	while ( (rc = poptGetNextOpt( opt_context )) >= 0 ){
		switch(rc){
		default:
			;
		}
	}
}


int main(int argc, const char* argv[])
{
	ui(argc, argv);
	return decode();
}
