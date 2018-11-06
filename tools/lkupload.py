#!/usr/bin/env python

from __future__ import print_function
import os
import serial
import time
from sys import argv
from sys import exit
from sys import stderr

def help():
	print("usage: lkupload.py [<options>] <device> <input>", file=stderr)
	print("   -u <path>     serial port connected to Arduino", file=stderr)
	print("   -a <addr>     starting address (default: 0x9c0)", file=stderr)
	print("   -l <len>      length of data (default: length of file)", file=stderr)
	print("   -i <path>     path to read from", file=stderr)

def main():
	port = None
	addr = 0x9c0
	maxlen = 0
	input = None
	i = 1
	while i < len(argv):
		arg = argv[i]
		i += 1
		if arg == "--help":
			help()
			exit()
		elif arg == "-u" and i < len(argv):
			port = argv[i]
			i += 1
		elif arg == "-a" and i < len(argv):
			try:
				addr = int(argv[i], 0)
			except ValueError:
				print(argv[i] + " is not valid for -a", file=stderr)
				exit()
			i += 1
		elif arg == "-l" and i < len(argv):
			try:
				maxlen = int(argv[i], 0)
			except ValueError:
				print(argv[i] + " is not valid for -l", file=stderr)
				exit()
			i += 1
		elif arg == "-i" and i < len(argv):
			input = argv[i]
			i += 1
		elif port is None:
			port = arg
		elif input is None:
			input = arg
		else:
			help()
			exit()
	if port is None or input is None:
		help()
	else:
		try:
			i = 0
			n = os.stat(input).st_size
			if maxlen > 0 and maxlen < n:
				n = maxlen
			with open(input, "rb") as f:
				with serial.Serial(port=port, baudrate=9600, bytesize=serial.EIGHTBITS,
								   parity=serial.PARITY_NONE, stopbits=serial.STOPBITS_ONE,
								   xonxoff=False, rtscts=True, dsrdtr=False, timeout=10) as ser:
					time.sleep(5)
					if ser.readline().strip() != "READY":
						print("No response from device", file=stderr)
						exit()
					time.sleep(1)
					while i < n:
						linelen = n - i
						if linelen > 16:
							linelen = 16
						b = f.read(linelen)
						s = True
						ser.write("@2%x" % addr)
						for c in b:
							if s:
								ser.write(" %02x" % ord(c))
								s = False
							else:
								ser.write("%02x" % ord(c))
								s = True
						ser.write("\f\n!\n")
						try:
							int(ser.readline().strip())
						except ValueError:
							print("No response from device", file=stderr)
							exit()
						addr += len(b)
						i += len(b)
					exit()
		except KeyboardInterrupt:
			pass

if __name__ == "__main__": main()
