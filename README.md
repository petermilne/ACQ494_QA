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

