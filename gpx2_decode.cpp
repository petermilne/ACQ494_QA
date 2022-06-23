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
#include <assert.h>
#include "popt.h"
#include <vector>
#include <map>

using namespace std;

#include "gpx2_format.h"

struct ChannelData;
typedef map<unsigned, ChannelData> CMAP;

struct ChannelData {
	int sc;
	FILE *fp;

	static CMAP cmap;
	ChannelData(int _sc): sc(_sc) {
		char fname[80];
		snprintf(fname, 80, "gpx2_%02d.dat", sc);
		fp = fopen(fname, "w");
		assert(fp);
	}
	ChannelData(): sc(0), fp(0) {}

	static ChannelData& factory(int sc);
};
CMAP ChannelData::cmap;

ChannelData& ChannelData::factory(int sc) {
	auto _cmap = cmap.find(sc);
	FILE* fp;
	if (_cmap != cmap.end()){
		return _cmap->second;
	}else{
		ChannelData* cdata = new ChannelData(sc);
		cmap[sc] = *cdata;
		return *cdata;
	}

}
namespace G {		/* put any globals in here */
	int verbose = 0;
	int save;

	int max_events = -1;
};

struct poptOption opt_table[] = {
	{
	  "save", 's', POPT_ARG_INT, &G::save, 0, "set to 1 to save data"
	},
	{
	  "max_events", 'M', POPT_ARG_INT, &G::max_events, 0, "put a limit on the number of events"
	},
	{
	  "verbose", 'd', POPT_ARG_INT, &G::verbose, 0, "set debug level"
	},
	POPT_AUTOHELP
	POPT_TABLEEND
};


int decode(void)
{
	unsigned long long tmp;

	for (int event = 0;
		fread(&tmp, sizeof(long long), 1, stdin) == 1 &&
		(G::max_events == -1 || event < G::max_events);
								event++ ){

		if (G::verbose > 2){
			fprintf(stderr, "%d,%016llx\n", event, tmp);
		}

		unsigned sc;
		double seconds = gpx_from_raw(tmp, sc);

		if (G::verbose == 1){
			fprintf(stderr, "%6d,%2d,%14.12f\n", event, sc, seconds);
		}
		if (G::verbose == 2){
			unsigned sc, nref, stop;
			gpx_from_raw(tmp, sc, nref, stop);
			fprintf(stderr, "%6d,%2d,%8d,%8d\n", event, sc, nref, stop);
		}
		if (G::save){
			fwrite(&seconds, sizeof(double), 1, ChannelData::factory(sc).fp);
		}
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
