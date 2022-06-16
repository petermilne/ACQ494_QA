#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
import argparse
import sys
import acq400_hapi
import re
import os

def make_title(serial, ch, stops):
    title = 'ACQ494FMC Stop Histogram s/n {} ch {}'.format(serial, ch)
    m = np.mean(stops)
    s = np.std(stops)
    p = np.ptp(stops)

    title += '\n' + str(round(m, 1)) + ' ± ' + str(round(s, 1)) + ' ps ' + '(' + str(round(p, 1)) + ' ptp)'
    return title


def plot(stops, title, serial, ch):
    plt.title(title)
    plt.hist(stops, bins=100)
    plt.xlabel("stop values in ps")
    plt.ylabel("Freq")
    plt.savefig('{}.{}.png'.format(serial, ch))
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
    seconds = np.fromfile(args.times, dtype=np.double)
    stops = 1e12* np.mod(seconds, 100e-9)
    plot(stops, make_title(serial, ch, stops), serial, ch)

    summary_file = 'acq494_validation.csv'
    if not os.path.exists(summary_file):
        with open(summary_file, 'a') as f:
            line = [ 'serial','uut', 'site', 'ch', 'mean', 'std', 'ptp' ]
            f.write(",".join(line) + os.linesep)

    with open(summary_file, 'a') as f:
        line = [ serial, uut.uut, site, ch, str(np.mean(stops)), str(np.std(stops)), str(np.ptp(stops)) ]
        f.write(",".join(line) + os.linesep)


if __name__ == "__main__":
    run_main()
