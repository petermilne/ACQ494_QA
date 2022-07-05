#!/usr/bin/env python3
import numpy as np
import matplotlib.pyplot as plt
import sys
import argparse
import acq400_hapi
import re
import os

def get_args(argStr=None):
    parser = argparse.ArgumentParser(description='plot_compare.1588v2')
    parser.add_argument('px', nargs = 2, help="path to first, second data files")   
    args = parser.parse_args(argStr)

    args.uutinfo = []
    
    for px in args.px:
        m = re.search(r'(acq2106_[0-9]{3})/gpx2_([0-9])([0-9]).*', px)
        args.uutinfo.append({ 'uut': m.group(1), 'site': m.group(2), 'ch': m.group(3) })
        
    args.uuts = [ acq400_hapi.factory(info['uut']) for info in args.uutinfo ]    
    return args
    
def get_data(args):
    ieee1588v2 = np.dtype([('b96', np.uint16, (6,))])
    # these things are NOT really signed, but it makes subtraction easier!
    taitype = np.dtype([('tai', np.int64), ('ns', np.int32), ('ps', np.int32)])
        
    args.ieee1588v2 = []
    args.tai128 = []
    
    for px in args.px:
        raw = np.fromfile(px, dtype=ieee1588v2)
        times = np.empty(len(raw), dtype=taitype)        
        tai128 = np.empty(len(raw), dtype=taitype)
        _tais = np.add(np.multiply(raw['b96'][:,0], 65536), raw['b96'][:,1])        
        times['tai'] = np.add(np.multiply(_tais, 65536), raw['b96'][:,2])
        times['ns']  = np.add(np.multiply(raw['b96'][:,3], 65536), raw['b96'][:,4])
        times['ps']  = np.multiply(raw['b96'][:,5], 1000/65535)       
        
        args.ieee1588v2.append(raw)
        args.tai128.append(times)
    
    lmin = 99999999999999999999999
    for rx in args.tai128:
        if len(rx) < lmin:
            lmin = len(rx)
        
    #lmin = 100
    for ii, rx in enumerate(args.tai128):
        if len(rx) > lmin:
            args.tai128[ii] = args.tai128[ii][:lmin]
    

def get_shot_times(tain, tai):
    print("normalized: {}".format(tain))
    return np.add(np.multiply(np.add(np.multiply(tain, 1e9), tai['ns']), 1e3), tai['ps'])
#    return np.add(np.multiply(tain, 1e9), tai['ns'])    

def uut_title(uutinfo):
    return "acq2106_{} s{} CH{}".format(uutinfo['uut'], uutinfo['site'], uutinfo['ch'])

def plot_data(args):
    for ii in range(0, len(args.tai128[0])):
        if args.tai128[0]['tai'][ii] != args.tai128[1]['tai'][ii]:
            print("ERROR at {} TAI mismatch: {} {}".format(ii, args.tai128[0]['tai'],args.tai128[1]['tai']))
            sys.exit(1)
            
    print("all TAI's match .. we're good")
    args.tai0 = args.tai128[0]['tai'][0]
    
    args.tain = []                    # normalised tai seconds
    args.shot_times = []
    
    for ii in range(0, len(args.tai128)):
        args.shot_times.append(get_shot_times(np.subtract(args.tai128[ii]['tai'], args.tai0), args.tai128[ii]))

        #sys.exit()
#    plt.subplot(311)
    #print(args.tai128[0]['ns'])
    #plt.plot(args.tai128[0]['ns'])
    #plt.plot(args.tai128[0]['ps'])
#    plt.plot(args.shot_times[0])
#    plt.ylabel("ps")
#    plt.xlabel("Sample Count")
#    plt.title(args.uuts[0].uut)

#    plt.subplot(312)
    #plt.plot(args.shot_times[1])
    #print(args.tai128[1]['ns'])
    #plt.plot(args.tai128[1]['ns'])
    #plt.plot(args.tai128[1]['ps'])
    #plt.ylabel("ps")
    #plt.xlabel("Sample Count")
    #plt.title(args.uuts[0].uut)

    #plt.subplot(313)
    #for ii in range(0, 10):
        #print("{} [0] ns {} [1] {} ns  dt {} ns".format(ii, args.tai128[0]['ns'][ii], args.tai128[1]['ns'][ii], args.tai128[0]['ns'][ii]-args.tai128[1]['ns'][ii]))
    #print(np.subtract(args.tai128[0]['ns'], args.tai128[1]['ns']))
    #plt.plot(np.subtract(args.tai128[0]['ns'], args.tai128[1]['ns']))
    delta_ps =  np.subtract(args.shot_times[0], args.shot_times[1])
    plt.plot(delta_ps)
    plt.ylabel("delta ps")
    plt.xlabel("Sample Count")
    plt.title("{} vs {}\n  Delta ps {:.0f} +/- {:.0f}, ptp {:.0f} ".
              format(uut_title(args.uutinfo[0]), uut_title(args.uutinfo[1]),
                     np.mean(delta_ps), np.std(delta_ps), np.ptp(delta_ps)))
    #plt.tight_layout()
    plt.show() 
    
     
def run_main():
    args = get_args()
    print(args)

    for ii, px in enumerate(args.px):
        print("path: {} uutinfo: {}".format(px, args.uutinfo[ii]))
        
    get_data(args)
    plot_data(args)
    

if __name__ == '__main__':
    run_main()