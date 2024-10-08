/*
 * gpx2_simulate.cpp
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

#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <vector>

#include <cstdlib>
#include <ctime>

#include "popt.h"

#include "gpx2_format.h"
#include <random>

using namespace std;


namespace G {		/* put any globals in here */
	int event_rate;		// incoming event rate, Hz
	const char* active_channel_list = "11";
	FILE* fp = stdout;
	int verbose = 0;

	int active_channel_count;
	int active_channels[16];

	int max_events = 4;

	int DELTA_NREF = 1;		// @@todo assumed event_rate 10MHz

	int stop0 = 0x37;		// initial stop value
	int stop_noise = 0;		// stop noise, percent

        default_random_engine generator; 
        normal_distribution<double> distribution(0, 1);
};

struct poptOption opt_table[] = {
	{
	  "event_rate", 'E', POPT_ARG_INT, &G::event_rate, 0, "incoming event rate, Hz"
	},
	{
	  "active_chan", 'A', POPT_ARG_STRING, &G::active_channel_list, 0, "list of channels in <S><C>,<S><C> format"
	},
	{
	  "max_events", 'M', POPT_ARG_INT, &G::max_events, 0, "put a limit on the number of events"
	},
	{
	  "verbose", 'd', POPT_ARG_INT, &G::verbose, 0, "set debug level"
	},
	{
	  "stop0", 's', POPT_ARG_INT, &G::stop0, 0, "initial stop value"
	},
	{
          "stop_noise", 0, POPT_ARG_INT, &G::stop_noise, 0, "noise on stop signal (integer %)"
	},
	POPT_AUTOHELP
	POPT_TABLEEND
};


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

	G::active_channel_count = sscanf(G::active_channel_list, "%d,%d,%d,%d,%d,%d,%d,%d",
			&G::active_channels[0], &G::active_channels[1], &G::active_channels[2], &G::active_channels[3],
			&G::active_channels[4], &G::active_channels[5], &G::active_channels[6], &G::active_channels[7]
			);

	if (G::verbose){
		fprintf(stderr, "event_rate %d Hz\n", G::event_rate);
		fprintf(stderr, "active channels %s\n", G::active_channel_list);
		for (int ic = 0; ic < G::active_channel_count; ++ic){
			fprintf(stderr, "channel:%d\n", G::active_channels[ic]);
		}
	}

	std::srand(std::time(nullptr)); // use current time as seed for random generator
}

unsigned stop(void)
// @@todo simulate stop value: initial + noise
{
	int stop_noise = 0;

	
	// ref: https://en.cppreference.com/w/cpp/numeric/random/rand
	if (G::stop_noise > 0){
	        double g_noise = G::distribution(G::generator);
		//cout << "random Gaussian noise" << g_noise << endl;
		double noisepu = g_noise / (RAND_MAX + 1u);
		
		stop_noise = G::stop_noise * noisepu * GPX_STOP_MAX / 100;
		if (G::verbose > 1){
			fprintf(stderr, "stop() %d%%  noisepu %f %u\n", G::stop_noise, noisepu, stop_noise);
		}
	}
	return G::stop0 + stop_noise;
}

unsigned nref_noise(void)
// @@todo simulate nref noise
{
	return 0;
}


void simulate() {
	unsigned nref = 0;

	for (int ii = 0; ii < G::max_events; ++ii){
		for (int jj = 0; jj < G::active_channel_count; ++jj){
			unsigned long long raw = gpx_to_raw(G::active_channels[jj], nref + nref_noise(), stop());
			fwrite(&raw, sizeof(unsigned long long), 1, G::fp);
		}
		nref += G::DELTA_NREF;
	}
}


int main(int argc, const char* argv[]) {
	ui(argc, argv);
	simulate();

}
