#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# Author: Tommy Lee <tommy@visharetech.com>

import argparse
import binascii
import subprocess
import struct
import sys
import os

__version__ = "1.3"

class Args(object):
    pass
    
args = Args()    

def readelf(infile):
    cmd = [ args.target+'readelf', '-l', infile ]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    if p.returncode != 0:
        raise IOError(err)
    for line in out.splitlines():
        if line.startswith('Entry point '):
            startPC = int(line.split()[2], 0)
            return startPC
       
def objdump(infile):
    cmd = [ args.target+'objdump', '-fhw', infile ]
    p = subprocess.Popen(cmd, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    out, err = p.communicate()
    if p.returncode != 0:
        raise IOError(err)
    startPC = -1
    loadAddr = 0xffffffff
    
    out = out.decode("utf-8")
    
    for line in out.splitlines():
        if line.startswith('start address '):
            startPC = int(line.split()[2], 0)
        if line.find('LOAD') != -1:
            loadAddr = min(loadAddr, int(line.split()[4], 16))

    if startPC== -1 or loadAddr == 0xffffffff:
        raise ValueError("Invalid start address or load address.")
        
    return startPC, loadAddr

def objcopy(infile):
    cmd = [ args.target+'objcopy', '-S', '-O', 'binary', infile, args.tmpfile ]
    p = subprocess.Popen(cmd, stderr=subprocess.PIPE)
    _, err = p.communicate()
    if p.returncode != 0:
        raise IOError(err)
    
def clean_up():
    try:
        os.unlink(args.tmpfile)
    except:
        pass    

        
def print_help():
    sys.stderr.write('ViShare image generator v%s\n' % __version__)
    sys.stderr.write('Usage: %s ELF-BINARY IMAGE-FILE.img \n' % os.path.basename(__file__))

def main():
    global args

    parser = argparse.ArgumentParser(description='Generate v4k image file from ELF.', formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('-p', '--objcopy-program', metavar='PROG', default='mips-unknown-elf-objcopy', help='\'objcopy\' program to use.')
    parser.add_argument('-t', '--target', metavar='TARGET', default='mips-unknown-elf', help='Target name of binutils to use')
    parser.add_argument('-s', '--minimum-size', metavar='MINSZ', default=0, help='Minimum size of image.', type=int)
    parser.add_argument('infile', help='Input file as ELF format')
    parser.add_argument('outfile', help='Output image filename' )    
    parser.parse_args(namespace=args)
    
    if args.target != '' and not args.target.endswith('-'):
        args.target += '-'
        
    args.tmpfile = 'tmp.' + os.path.basename(args.infile) + '.bin'
    
    #print vars(args)
   
    try:
        objcopy(args.infile)
        #startPC = readelf(args.infile)
        startPC, loadAddr = objdump(args.infile)
        with open(args.tmpfile, 'rb') as infile:
            binBuf = bytearray(infile.read())
        imgSize = max(args.minimum_size, len(binBuf)+16)
        sizeAlign = 64*1024
        imgSize = ((imgSize+sizeAlign-1)//sizeAlign)*sizeAlign
        
        while len(binBuf)+16 < imgSize:
            binBuf.append(0x00)
        
        checksum = 0
        for b in binBuf:
            checksum += b        

        with open(args.outfile, 'wb') as outfile:
            outfile.write(struct.pack('<LLLL', imgSize, loadAddr, startPC, checksum))
            outfile.write(binBuf)

        print('Created image file %s => %s.' % (args.infile, args.outfile))
        print('  size=     %#x(%d)' % (imgSize, imgSize))
        print('  loadAddr=', hex(loadAddr))
        print('  startPC= ', hex(startPC))
        print('  checksum=', hex(checksum))
            
    except Exception as e:
        sys.stderr.write(str(e))
        sys.exit(-1)
        
    finally:
        clean_up()
    
        


if __name__ == '__main__':
    main()
