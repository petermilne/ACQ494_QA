# ACQ494_QA

Dual purpose package
- compiles on host, acquires, decodes analyses data
- embedded package for live decode

# ACQ494 Channel ID

11 : Site 1 Channel 1
12 : Site 1 Channel 2
..
24 : Site 2 Channel 4

## Assumed directory structure

```bash
~/PROJECTS/ACQ494_QA	  # this code
~/PROJECTS/ACQ400         # main ARM runtime (if cross compiling)
~/PROJECTS/acq400_hapi    # copy of latest HAPI
```

### Install at D-TACQ

```bash
cd PROJECTS
git clone git://eigg/Software/acq400_hapi
git clone git://eigg/Software/ACQ494_QA
cd ACQ494_QA
git checkout --track origin/pgm1
make
```

### Install Remote

```bash
d PROJECTS

git clone https://github.com/petermilne/acq400_hapi.git

# Private Repo: send me a github ID for share
git clone https://github.com/petermilne/ACQ494_QA.git
cd ACQ494_QA
git checkout --track origin/pgm1
make
```

## Capture, Decode, Analyse

```bash
./acq494_validation_test [UUT] [1000000]

[dt100@naboo ACQ494_QA]$ ls -l ACQ494
total 15724
-rw-r--r-- 1 dt100 d-tacq 8000000 Jun 16 18:22 acq2106_188_CH00
-rw-r--r-- 1 dt100 d-tacq     360 Jun 16 18:22 acq494_validation.csv
-rw-r--r-- 1 dt100 d-tacq   27848 Jun 16 18:07 F49420002.1.png
-rw-r--r-- 1 dt100 d-tacq   27638 Jun 16 18:12 F49420002.2.png
-rw-r--r-- 1 dt100 d-tacq   27376 Jun 16 18:22 F49420002.4.png
-rw-r--r-- 1 dt100 d-tacq      50 Jun 16 18:22 format
-rw-r--r-- 1 dt100 d-tacq 8000000 Jun 16 18:22 gpx2_14.dat


[dt100@naboo ACQ494_QA]$ ls -1 ACQ494
acq2106_188_CH00			# RAW data from latest shot
acq494_validation.csv			# summary table: adds a row each time
F49420002.1.png				# archive plots
F49420002.2.png
F49420002.4.png
format					# ignore (for kst)
gpx2_14.dat				# current decoded data (double seconds), channel 14
```

## WR Synchronized Test:

```
TRG=ext,rising ./acq494_validation_test acq2106_188 acq2106_054

# on cs-studio, observe both boxes are at ARM, and press WRTD_TXI

./plot_compare.1588v2.py ACQ494/acq2106_188/gpx2_11.1588v2 ACQ494/acq2106_054/gpx2_11.1588v2

```

./plot_compare.1588v2.py ACQ494/acq2106_188/gpx2_11.1588v2 ACQ494/acq2106_054/gpx2_11.1588v2
```

### IEEE1588v2 data

```
Standard format, recommended for interchange
 * https://www.intel.com/content/www/us/en/docs/programmable/683639/21-1-19-4-0/ptp-timestamp-and-tod-formats.html
 * Bits [95:48]: Seconds (48 bits).
 * Bits [47:16]: Nanoseconds (32 bits). This field overflows at 1 billion.
 * Bits [15:0]: Fractions of nanosecond (16 bits). This field is a true fraction; it overflows at 0xFFFF.
```

gpx2decode can generate this data format as follows:

```
gpx2decode --1588v2=1 < DATA
..
this generates a file
gpx2_SC.1588v2

Where S = Site and C is channel  eg
gpx2_11.1588v2   : Site1, Channel1

A file is generated per channel found in the data set.
```

This format is OK for data transfer, but it's NOT suitable for math in python
Instead we convert to "TAI128":

```
 * Bits [127:64]: Seconds (64 bits).
 * Bits [63:32]: Nanoseconds (32 bits). This field overflows at 1 billion.
 * Bits [31:0]:  Picoseconds
```

The types are represented in numpy as:

```python
    ieee1588v2 = np.dtype([('b96', np.uint16, (6,))])
    taitype = np.dtype([('tai', np.uint64), ('ns', np.uint32), ('ps', np.uint32)])
```

## low level toolz

### decode to double seconds, print in ascii

```bash
[dt100@naboo ACQ494]$ ../gpx2_decode -M 10 -d1 < acq2106_188_CH00
     0,14,0.026379421929
     1,14,0.026379521938
     2,14,0.026379621944
     3,14,0.026379721947
     4,14,0.026379821925
     5,14,0.026379921914
     6,14,0.026380021908
     7,14,0.026380121905
     8,14,0.026380221950
     9,14,0.026380321905
```

### decode to NREFS, STOPS fields

```bash
[dt100@naboo ACQ494]$ ../gpx2_decode -M 10 -d2 < acq2106_188_CH00
     0,14,  263794,   21929
     1,14,  263795,   21938
     2,14,  263796,   21944
     3,14,  263797,   21947
     4,14,  263798,   21925
     5,14,  263799,   21914
     6,14,  263800,   21908
     7,14,  263801,   21905
     8,14,  263802,   21950
     9,14,  263803,   21905
```

### decode raw binary (FPGA wire format)

```bash
[dt100@naboo ACQ494]$ ../gpx2_decode -M 10 -d3 < acq2106_188_CH00
0,01040406720055a9
1,01040406730055b2
2,01040406740055b8
3,01040406750055bb
4,01040406760055a5
5,010404067700559a
6,0104040678005594
```



