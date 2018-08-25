#!/usr/bin/env python3
import sys
from binascii import hexlify

def main(argv):
    filename = argv[0]
#    print filename
    f = open(filename, 'r+')

    outfile = argv[1]
    o = open(outfile, 'wib')

    NoOfBytes = 0
    o.write("Bitstream: ")
    while True:
        byte = f.read(1)
        if byte == '':
            NoOfWords = NoOfBytes/4
            o.write("\nNumber of Words: ")
            o.write(str(NoOfWords))
            break

        NoOfBytes = NoOfBytes + 1
        byte = hexlify(byte)
        o.write(byte)
#        print byte

if __name__ == "__main__":
    main(sys.argv[1:])
