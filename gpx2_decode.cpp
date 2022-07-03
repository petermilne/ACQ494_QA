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

namespace G {		/* put any globals in here */
	int verbose = 0;
	int save;
	int do1588v2 = 0;
	int doTAI128 = 0;
	int max_events = -1;
	int taigui = 0; 		// run gui exclusive
	const char* strftime_fmt = "%Y%m%d:%H:%M:%S";
};

struct ChannelData;
typedef map<unsigned, ChannelData> CMAP;

struct ChannelData {
	int sc;
	FILE *fp;
	FILE *fp1588v2;
	FILE *fpTAI128;

	static CMAP cmap;
	ChannelData(int _sc): sc(_sc) {
		char fname[80];
		snprintf(fname, 80, "gpx2_%02d.dat", sc);
		fp = fopen(fname, "w");
		assert(fp);
		if (G::do1588v2){
			snprintf(fname, 80, "gpx2_%02d.1588v2", sc);
			fp1588v2 = fopen(fname, "w");
                	assert(fp1588v2);
		}
		if (G::doTAI128){
			snprintf(fname, 80, "gpx2_%02d.TAI128", sc);
			fpTAI128 = fopen(fname, "w");
                	assert(fpTAI128);
		}
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

struct poptOption opt_table[] = {
	{
	  "save", 's', POPT_ARG_INT, &G::save, 0, "set to 1 to save data"
	},
	{
	  "1588v2", 0, POPT_ARG_INT, &G::do1588v2, 0, "set to 1 to save 1588 style"
	},
	{
	  "TAI128", 0, POPT_ARG_INT, &G::doTAI128, 0, "set to 1 to save TAI:64, NREF:32 STOP:32" 
        },
	{
	  "max_events", 'M', POPT_ARG_INT, &G::max_events, 0, "put a limit on the number of events"
	},
	{
	  "verbose", 'd', POPT_ARG_INT, &G::verbose, 0, "set debug level"
	},
	{
	  "TAIGUI", 'g', POPT_ARG_INT, &G::taigui, 0, "run a gui showing current TAI and surrounding events"
	},
	POPT_AUTOHELP
	POPT_TABLEEND
};

const unsigned PPSSIG = GPX_PPSSIG >> GPX_BITS::PPSSIG;

/* 1588v2:
 * https://www.intel.com/content/www/us/en/docs/programmable/683639/21-1-19-4-0/ptp-timestamp-and-tod-formats.html
 * Bits [95:48]: Seconds (48 bits).
 * Bits [47:16]: Nanoseconds (32 bits). This field overflows at 1 billion.
 * Bits [15:0]: Fractions of nanosecond (16 bits). This field is a true fraction; it overflows at 0xFFFF.
 *
 * FIRST up: let's not try pack it.. store 64, 32, 32 TAI128
 *
 * OK< here's the plan: shift and split into an array of 6 u16's
 */

typedef unsigned short B96[6];

#define WMASK 	0x0000ffff

void store1558v2(FILE* fp, unsigned long long tai, unsigned nref, unsigned stop, short nref_snap)
{
	unsigned nsec = nref * 100 + stop/1000;
	unsigned frac_nsec = (stop%1000) * 65535/1000;
	B96 b96;
	b96[0] = tai >> 32;
	b96[1] = (tai >> 16) & WMASK;
	b96[2] = tai & WMASK;
	b96[3] = nsec >> 16;
	b96[4] = nsec & WMASK;
	b96[5] = frac_nsec;
	fwrite(&b96, sizeof(short), 6, fp);
}
void storeTAI128(FILE* fp, unsigned long long tai, unsigned nref, unsigned stop, short nref_snap)
{
	if (nref_snap != 0){
		fprintf(stderr, "WARNING: store1558v2() %16llu %8u %8llu %d\n", tai, nref, stop, nref_snap);
	}
	fwrite(&tai, sizeof(tai), 1, fp);
	fwrite(&nref, sizeof(nref), 1, fp);
	fwrite(&stop, sizeof(stop), 1, fp);
}
int decode(void)
{
	unsigned long long tmp;
	bool header_printed = false;

	for (int event = 0;
		fread(&tmp, sizeof(long long), 1, stdin) == 1 &&
		(G::max_events == -1 || event < G::max_events);
								event++ ){
		if (!header_printed){
			switch(G::verbose){
			case 1:
				fprintf(stderr, "%6s,%2s,%14s\n", "evt", "sc", "seconds");
			case 2:
				fprintf(stderr, "%6s,%2s,%8s,%8s\n", "evt", "sc", "NREF", "STOP");
			}
			header_printed = true;
		}
		if (G::verbose > 3){
			fprintf(stderr, "%d,%016llx\n", event, tmp);
		}
		if ((tmp&GPX_FILLER_MASK) == GPX_FILLER){
			continue;
		}
		if (G::verbose > 2){
			fprintf(stderr, "%d,%016llx\n", event, tmp);
		}

		unsigned long long raw_pps;
		unsigned long long tai;
		short nref_snap;

		if (gpx2_is_pps(tmp, tai, nref_snap)) {
			raw_pps = tmp;
			if (G::verbose){
				fprintf(stderr, "%6d,%2x,%32llu,%04x\n", event, PPSSIG, tai, nref_snap);
			}
			continue;
		}
		unsigned sc;
		double seconds = gpx_from_raw(tmp, sc);

		if (G::verbose == 1){
			if (gpx2_valid_sc(sc)){
				fprintf(stderr, "%6d,%2d,%14.12f\n", event, sc, seconds);
			}else{
				fprintf(stderr, "data not valid\n");
				return 1;
			}
		}
		unsigned nref, stop;
		gpx_from_raw(tmp, sc, nref, stop);

		if (G::verbose == 2){
			if (gpx2_valid_sc(sc)){
				fprintf(stderr, "%6d,%2d,%8d,%8d\n", event, sc, nref, stop);
			}else{
				fprintf(stderr, "data not valid\n");
				return 1;
			}
		}
		if (G::save){
			fwrite(&seconds, sizeof(double), 1, ChannelData::factory(sc).fp);
		}
		if (G::do1588v2){
			if (gpx2_valid_sc(sc)){
				store1558v2(ChannelData::factory(sc).fp1588v2, tai, nref, stop, nref_snap);
			}else{
				fprintf(stderr, "ERROR: bad sc\n");
			}
		}
		if (G::doTAI128){
			if (gpx2_valid_sc(sc)){
				storeTAI128(ChannelData::factory(sc).fpTAI128, tai, nref, stop, nref_snap);
			}else{
				fprintf(stderr, "ERROR: bad sc\n");
			}

		}
	}
	return 0;
}

#define CLEARSCR	"\033[2J"
#define HOME		"\033[H"

const char* timestr(char* buf, int maxbuf, unsigned long long tai)
{
	time_t now = (time_t)tai;
	strftime(buf, maxbuf, G::strftime_fmt, localtime(&now));
	return buf;
}
int taigui_handle_buffer(int ibuf, FILE* fp) {
	unsigned long long tmp;
	bool header_printed = false;
	bool found_pps = false;

	for (int event = 0;
		fread(&tmp, sizeof(long long), 1, fp) == 1; ){
		if ((tmp&GPX_FILLER_MASK) == GPX_FILLER){
			continue;
		}
		if (G::verbose > 2){
			fprintf(stderr, "%d,%016llx\n", event, tmp);
		}



		if ((tmp&GPX_FILLER_MASK) == GPX_FILLER){
			continue;
		}

		unsigned long long tai;
		short nref_snap;

		if (gpx2_is_pps(tmp, tai, nref_snap)) {
			if (G::verbose){
#define NTS			80
				char ts[NTS];
				fprintf(stderr, CLEARSCR HOME);
				fprintf(stderr, "buffer:%03d\n", ibuf);
				fprintf(stderr, "%6s,%2s,%32s,%30s\n", "event", "sc", "tai", "tai date");
				fprintf(stderr, "%6d,%2x,%32llu,%30s\n", event, PPSSIG, tai, timestr(ts, NTS, tai));
			}
			found_pps = true;
			continue;
		}
		if (found_pps){
			unsigned nref, stop, sc;
			gpx_from_raw(tmp, sc, nref, stop);

			if (gpx2_valid_sc(sc)){
				if (!header_printed){
					fprintf(stderr, "%6s,%2s,%32s,%8s,%8s\n", "event", "sc", "tai", "nref", "stop");
					header_printed = true;
				}
				fprintf(stderr, "%6d,%2d,%32llu,%8d,%8d\n", event, sc, tai, nref, stop);
				if (++event > G::max_events){
					found_pps = false;
					event = 0;
				}
			}else{
				fprintf(stderr, "data not valid\n");
				return 1;
			}
		}
	}
}

#define BQF "/dev/acq400.0.bqf"
#define HBF "/dev/acq400.0.hb/%03d"

int taigui(void)
// live display of current TAI and +/-G::max_events events
{
	FILE* fp = fopen(BQF, "r");
	if (fp == 0){
		perror(BQF);
		return -1;
	}
	char bqline[32];
	while(fgets(bqline, 32, fp)){
		char bufname[80];
		int ibuf = atoi(bqline);
		snprintf(bufname, 80, HBF, ibuf);
		FILE* bfp = fopen(bufname, "r");
		if (bfp == 0){
			perror(bufname);
			return -2;
		}
		if (G::verbose){
			fprintf(stderr, "%03d,", ibuf); fflush(stderr);
		}
		taigui_handle_buffer(ibuf, bfp);
		fclose(bfp);
	}
	fclose(fp);
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
	if (G::taigui){
		return taigui();
	}else{
		return decode();
	}
}
