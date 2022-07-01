import numpy as np
import matplotlib.pyplot as plt
import sys

path = sys.argv[1]

#dt = np.dtype([('time', [('tai', np.uint64), ('nref', np.uint32), ('stop', np.uint32)])])
dt = np.dtype([('tai', np.uint64), ('nref', np.uint32), ('stop', np.uint32)])

#print dt
y = np.fromfile(path, dtype=dt)

plt.subplot(311)
plt.plot(y['tai'])
plt.ylabel("TAI")
plt.xlabel("Sample Count")
plt.title(path)

plt.subplot(312)
plt.plot(y['nref'])
plt.ylabel("NREF")
plt.xlabel("Sample")
plt.title("")

plt.subplot(313)
plt.plot(y['stop'])
plt.ylabel("STOP")
plt.xlabel("Sample")
plt.title("")

t0 = y['tai'][0]

shot_sec = np.subtract(y['tai'], t0)
shot_times_ref = np.add(shot_sec, np.multiply(y['nref'], 100e-9))
shot_times = np.add(shot_times_ref, np.multiply(y['stop'], 1e-12))

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
