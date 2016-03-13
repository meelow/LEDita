from OSC import OSCServer,OSCClient, OSCMessage
import sys
from time import sleep
import types
#import requests
sys.path.insert(0, '/usr/lib/python2.7/bridge/') # make sure python can see the tcp module

													 
#set up the json client and the osc client and server. The server should have the ip of your Yun. 
#The client.connect should have the ip address of you phone. 
server = OSCServer( ("192.168.1.250", 8000) )
client = OSCClient()
client.connect( ("192.168.1.107", 8000) )

#waits for slider change
def handle_timeout(self):
	print ("Timeout")

server.handle_timeout = types.MethodType(handle_timeout, server)

# Adding functionality for the "light" fader:
def faderLight_callback(path, tags, args, source):
	global faderLightFeedback
	if path=="/1/light":
		#extract parameter:
		faderLightFeedback = int(args[0])
		print str(path) +" "+ str(faderLightFeedback)
		#create feedback for faders label:
		msg = OSCMessage("/1/light")		
		msg.insert(0, faderLightFeedback)
		client.send(msg)
server.addMsgHandler( "/1/light",faderLight_callback) #register function for handling "fader" control
while True:
	server.handle_request()

server.close()