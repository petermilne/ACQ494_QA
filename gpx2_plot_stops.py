#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
import argparse
import sys
import acq400_hapi
import re

def make_title(serial, ch, stops):
	title = 'ACQ494FMC Stop Histogram s/n {} ch {}'.format(serial, ch)
	m = np.mean(stops)
	s = np.std(stops)
	p = np.ptp(stops)
   
	title += str(round(m, 1)) + ' ± ' + str(round(s, 1)) + ' ps ' + '(' + str(round(p, 1)) + ' ptp)'
	return title


def plot(times, serial, ch):
	seconds = np.fromfile(times, dtype=np.double)
	stops = 1e12* np.mod(seconds, 100e-9)

	plt.title(make_title(serial, ch, stops))
	plt.hist(stops, bins=100)
	plt.xlabel("stop values in ps")
	plt.ylabel("Freq")
	plt.show()

def get_args(argStr=None):
    parser = argparse.ArgumentParser(description='gpx2_plot_stops')
    parser.add_argument('--times', '-t', default='gpx2_11.dat', help='file containing double times[n] eg gpx2_11.dat')
    parser.add_argument('uut', nargs=1)
    return parser.parse_args(argStr)

def run_main():
    args = get_args()
    uut = acq400_hapi.factory(args.uut[0])
    m = re.search(r'gpx2_([0-9])([0-9]).dat', args.times)
    print(m.group(0))
    site = m.group(1)
    ch = m.group(2)
	
    serial = uut.svc['s{}'.format(site)].SERIAL
    plot(args.times, serial, ch)


if __name__ == "__main__":
	run_main()
