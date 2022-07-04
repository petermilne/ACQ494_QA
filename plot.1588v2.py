#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
import sys
import argparse
import acq400_hapi
import re
import os

path = sys.argv[1]

try:
   uut = acq400_hapi.factory(sys.argv[2])
   m = re.search(r'gpx2_([0-9])([0-9]).*', path)
   site = m.group(1)
   ch = m.group(2)
   serial = uut.svc['s{}'.format(site)].SERIAL
except:
   serial = "unknown"


#dt = np.dtype([('time', [('tai', np.uint64), ('nref', np.uint32), ('stop', np.uint32)])])

ieee1588v2 = np.dtype([('b96', np.uint16, (6,))])

taitype = np.dtype([('tai', np.uint64), ('ns', np.uint32), ('ps', np.uint32)])


#print dt
raw = np.fromfile(path, dtype=ieee1588v2)

times = np.empty(len(raw), dtype=taitype)

_tais = np.add(np.multiply(raw['b96'][:,0], 65536), raw['b96'][:,1])
times['tai']  = np.add(np.multiply(_tais, 65536), raw['b96'][:,2])
times['ns']    = np.add(np.multiply(raw['b96'][:,3], 65536), raw['b96'][:,4])
times['ps']    = np.multiply(raw['b96'][:,5], 1000/65535)

t0 = times['tai'][0]
shot_sec = np.subtract(times['tai'], t0)
shot_times_ref = np.add(shot_sec, np.multiply(times['ns'], 1e-9))
shot_times = np.add(shot_times_ref, np.multiply(times['ps'], 1e-12))

def make_title(serial, ch, stops):
    title = 'ACQ494FMC Delta Times {} {} ch {}'.format(uut.uut, serial, ch)
    m = np.mean(stops)
    s = np.std(stops)*1e6
    p = np.ptp(stops)*1e6
    title += '\n' + str(round(m, 1)) + ' us ' + ' ± ' + str(round(s, 1)) + ' ps ' + '(' + str(round(p, 1)) + ' ptp)'
    return title

dtus = np.multiply(np.subtract(shot_times[1:],shot_times[0:-1]), 1e6)
plt.title(make_title(serial, 1, dtus))
plt.xlabel("sample")
plt.ylabel("DT [us]")
plt.plot(dtus)
plt.show()

#sys.exit()
plt.subplot(311)
plt.plot(times['tai'])
plt.ylabel("TAI")
plt.xlabel("Sample Count")
plt.title(path)

plt.subplot(312)
plt.plot(times['ns'])
plt.ylabel("ns")
plt.xlabel("Sample")
plt.title("")

plt.subplot(313)
plt.plot(times['ps'])
plt.ylabel("ps")
plt.xlabel("Sample")
plt.title("")
plt.tight_layout()
plt.show(block=1)


plt.subplot(311)
plt.title("{} Plot Time in sec from start".format(path))
plt.plot(shot_times)
plt.ylabel("Time from Start")
plt.xlabel("Sample")

dt = np.multiply(np.subtract(shot_times[1:],shot_times[0:-1]), 1e12)
plt.subplot(312)
plt.hist(dt, bins=100)
plt.xlabel("time ps")
plt.ylabel("Frequency")

plt.subplot(313)
dtus = np.multiply(np.subtract(shot_times[1:],shot_times[0:-1]), 1e6)
print(dtus[0:10])
dtus[0] = 9
dtus[1] = 11
plt.plot(dtus)
plt.xlabel("sample")
plt.ylabel("DT [us]")
plt.show()


