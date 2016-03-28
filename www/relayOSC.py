from OSC import OSCServer,OSCClient, OSCMessage
import sys
from time import sleep
import types

# get to know my own ip adress:
from tools import get_ip
													 
#set up the OSC server to listen for the UI device 
server = OSCServer( (get_ip("wlan0"), 10000) )

# list of clients to relay the messages to:
clients = [
#		   [OSCClient(), ("192.168.1.104", 8000)],
		   [OSCClient(), ("192.168.1.105", 8000)],
		   [OSCClient(), ("192.168.1.107", 8000)]
		   #, 
#		   [OSCClient(), ("192.168.1.199", 8000)]
		  ]
for client in clients:
	client[0].connect( client[1] )


# waiting for data to relay:
def handle_timeout(self):
	print ("Waiting for data...")

server.handle_timeout = types.MethodType(handle_timeout, server)

# this function will be called for every received message
def relayMessageCallback(path, tags, args, source):
	global faderLightFeedback
#	print "--- received:"
#	print "path="+str(path)
#	print "tags="+str(tags)
#	print "args="+str(args)
#	print "source="+str(source)
	for client in clients:
		msg = OSCMessage(path)
		msg.extend(args)
		client[0].send(msg)
		print "relaying to " + str(client[1]) + " : " + str(msg)
		
server.addMsgHandler( "/1/light/",relayMessageCallback) 
server.addMsgHandler( "/1/fps/",relayMessageCallback) 
server.addMsgHandler( "/1/rotary1/",relayMessageCallback) 
for x in range(0,20):
	server.addMsgHandler("/1/state/"+str(x)+"/1", relayMessageCallback)
for x in range(0,20):
	server.addMsgHandler("/1/palette/"+str(x)+"/1", relayMessageCallback)
server.addMsgHandler("/1/xypad/", relayMessageCallback)
server.addMsgHandler("/1/push1/", relayMessageCallback)
	
while True:
	server.handle_request()

server.close()