import numpy as np
import matplotlib.pyplot as plt
import sys

path = sys.argv[1]

#dt = np.dtype([('time', [('tai', np.uint64), ('nref', np.uint32), ('stop', np.uint32)])])
dt = np.dtype([('tai', np.uint64), ('nref', np.uint32), ('stop', np.uint32)])
dt = np.dtype([('b96', (np.uint16, 3))])

print(dt)

#print dt
raw = np.fromfile(path, dtype=dt)
nsam = len(raw)

tai  = np.zeros(nsam, dtype=np.uint64)
nsec = np.zeros(nsam, dtype=np.uint32)
psec = np.zeros(nsam, dtype=np.uint32)

tai = np.add(np.multiply(raw[0], 65536), raw[1])
tai = np.add(np.multiply(tai, 65536), raw[2])

nsec = np.add(np.multiply(raw[3], 65536), raw[4])

psec = np.multiply(raw[5], 1000/65536)

plt.subplot(311)
plt.plot(tai)
plt.ylabel("TAI")
plt.xlabel("Sample Count")
plt.title(path)

plt.subplot(312)
plt.plot(nsec)
plt.ylabel("NREF")
plt.xlabel("Sample")
plt.title("")

plt.subplot(313)
plt.plot(psec)
plt.ylabel("STOP")
plt.xlabel("Sample")
plt.title("")

t0 = tai[0]

shot_sec = np.subtract(tai, t0)
shot_sec = np.add(shot_sec, nsec)
shot_sec = np.add(shot_sec, psec))

plt.tight_layout()
plt.show(block=1)


plt.subplot(211)
plt.title("{} Plot Time in sec from start".format(path))
plt.plot(shot_sec)
plt.ylabel("Time from Start")
plt.xlabel("Sample")

dt = np.multiply(np.subtract(shot_sec[1:],shot_sec[0:-1]), 1e12)
plt.subplot(212)
plt.hist(dt, bins=100)
plt.xlabel("time ps")
plt.ylabel("Frequency")

plt.show()
