#!/usr/bin/env python


"""
A TCP server to receive logs from clients.

Note that currently, this is only write logs to disk.

"""

import time
import os
import shutil
import sys
sys.path.append("/opt/ThreatSphere")

from logger import *
from twisted.internet import reactor, protocol
from twisted.protocols import basic

class CLogReceiver(basic.LineReceiver):
	delimiter = '\n' # unix terminal style newlines. remove this line
                     # for use with Telnet

	def __init__(self):
		self.fd2Write	= None
		self.file2Write	= ""
		self.c		= 64

	def connectionMade(self):
		self.c	= 64

		c_folder  = '/capture/data/' + self.transport.client[0]
		if not os.path.exists(c_folder):
			os.makedirs(c_folder)

		log_folder	= c_folder + "/" + "logs"
		if not os.path.exists(log_folder):
			os.makedirs(log_folder)

		dst_folder	= "/capture/Argus/data/input/" + self.transport.client[0]
		if not os.path.exists(dst_folder):
			os.makedirs(dst_folder)

		dst_folder	+= "/logs"
		if not os.path.exists(dst_folder):
			os.makedirs(dst_folder)

		self.log_folder	= log_folder
		self.dst_folder	= dst_folder

		self.createFD2Write()
		#self.sendLine("I'm the guy !")

	def lineReceived(self, line):
		# Ignore blank lines
		if not line: return
                #print line
		# Check if this is valide
		if not self.validateRawLogRecord(line) : return

		# Save ..
		self.saveIt(line)

	def validateRawLogRecord(self, rLogRecord):
		return True

	def saveIt(self, rLogRecord):
		self.revisitFD2Write()
		self.fd2Write.write(rLogRecord + '\n')

		pass

	def createFD2Write(self):
	
		file	= self.log_folder + '/' + time.strftime('%Y-%m-%d_%H-%M', time.localtime(time.time()))
                logger.debug("will create a new file to write %s" %(file))
                logger.debug("the current file is %s"%(self.file2Write))
		if (self.file2Write != file):
			if (self.fd2Write != None):
				self.fd2Write.close()
				self.fd2Write	= None
                                logger.debug("the two file is diff")

			if os.path.exists(self.file2Write):
				i	= self.file2Write.rfind("/")
				_fn	= self.file2Write[i+1:]
				dst_file	= self.dst_folder + "/" + _fn
				print "Copy " + self.file2Write + " to " + dst_file
				shutil.copyfile(self.file2Write, dst_file)
			else:
				print "File " + self.file2Write + " does not exist!"
			self.file2Write	= file

		if (self.fd2Write is None):
			self.fd2Write	= open(file, 'a')	
                        logger.debug("using the new file to write")
                logger.debug("the new file is the same with old file,so keep write it")
		
	def revisitFD2Write(self):
		if (self.c <= 0):
			self.c	= 64
			self.createFD2Write()
		else:
			self.c	-= 1

	def connectionLost(self, reason):
		if os.path.exists(self.file2Write):
			i	= self.file2Write.rfind("/")
			_fn	= self.file2Write[i+1:]
			dst_file	= self.dst_folder + "/" + _fn
			print "Copy " + self.file2Write + " to " + dst_file

			shutil.copyfile(self.file2Write, dst_file)
		else:
			print "File " + self.file2Write + " does not exist!"

		self.fd2Write.close()

		#reactor.stop()



f=protocol.Factory()
f.protocol=CLogReceiver

if __name__ == "__main__":
	reactor.listenTCP(8887, f)
	reactor.run()
