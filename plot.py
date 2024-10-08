import numpy as np
import matplotlib.pyplot as plt
import sys

path = sys.argv[1]

y = np.fromfile(path, dtype=np.double)
yn = 1e12* np.mod(y, 100e-9)
x = list(range(1, len(y) + 1))

plt.subplot(311)
plt.plot(y)
plt.ylabel("SECONDS")
plt.xlabel("Sample Count")
plt.title(path)

plt.subplot(312)
plt.plot(yn)
plt.ylabel("STOP ps")
plt.xlabel("Sample")
plt.title("")

plt.subplot(313)
plt.hist(yn, bins=100)
plt.xlabel("ps")
plt.ylabel("Frequency")

plt.tight_layout()
plt.show(block=1)
