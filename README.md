ACQ494 Channel ID

11 : Site 1 Channel 1
12 : Site 1 Channel 2
..
24 : Site 2 Channel 4

Assumed directory structure

~/PROJECTS/acq400_hapi    # copy of latest HAPI
~/PROJECTS/ACQ494_QA

cd PROJECTS
git clone git://eigg/Software/acq400_hapi
git clone git://eigg/Software/ACQ494_QA
cd ACQ494_QA
git checkout --track origin/pgm1
make

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


# low level toolz

## decode to double seconds, print in ascii

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

## decode to NREFS, STOPS fields

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


## decode raw binary (FPGA wire format)

[dt100@naboo ACQ494]$ ../gpx2_decode -M 10 -d3 < acq2106_188_CH00
0,01040406720055a9
1,01040406730055b2
2,01040406740055b8
3,01040406750055bb
4,01040406760055a5
5,010404067700559a
6,0104040678005594




