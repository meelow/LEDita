from OSC import OSCServer,OSCClient, OSCMessage
import sys
from time import sleep
import types
#import requests
sys.path.insert(0, '/usr/lib/python2.7/bridge/') # make sure python can see the tcp module

from tcp import TCPJSONClient
													 
#set up the json client and the osc client and server. The server should have the ip of your Yun. 
#The client.connect should have the ip address of you phone. 
json = TCPJSONClient('127.0.0.1', 5700)
server = OSCServer( ("192.168.1.199", 8000) )
client = OSCClient()
#client.connect( ("192.168.1.142", 9000) )

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
		#put parameter to the bridge:
		json.send({'command':'put', 'key':'brightness', 'value':'%i' % (faderLightFeedback)})		
		print str(path) +" "+ str(faderLightFeedback)
		#create feedback for faders label:
		msg = OSCMessage("/1/label_light")		
		msg.insert(0, faderLightFeedback)
		client.connect( (source[0], 9000) )
		client.send(msg)
server.addMsgHandler( "/1/light",faderLight_callback) #register function for handling "fader" control

# Adding functionality for the "fps" fader:
def faderFPS_callback(path, tags, args, source):
	global faderFPSFeedback
	if path=="/1/fps":
		#extract parameter:
		faderFPSFeedback = int(args[0])	
		#put parameter to the bridge:	
		json.send({'command':'put', 'key':'fps', 'value':'%i' % (faderFPSFeedback)})	
		#create feedback for faders label:	
		msg = OSCMessage("/1/label_fps")
		msg.insert(0, faderFPSFeedback)
		client.connect( (source[0], 9000) )
		client.send(msg)
		print "received: " +repr(source)+ " " + str(path) 
server.addMsgHandler( "/1/fps",faderFPS_callback)

def multiselectState_callback(path, tags, args, source):
	print "received: " +repr(source)+ " " + str(path) 
	split = path.split("/")
	state = split.pop()
	state = split.pop()
	if args[0]==1:
		#requests.get("http://192.168.1.104/arduino/state/"+str(int(state)-1))
		json.send({'command':'put', 'key':'state', 'value':'%i' % int(int(state)-1)})
		msg = OSCMessage(path)
		msg.insert(0, 1)
		client.connect( (source[0], 9000) )
		client.send(msg)
for x in range(0,20):
	server.addMsgHandler("/1/state/"+str(x)+"/1", multiselectState_callback)

def multiselectPalette_callback(path, tags, args, source):
	print "received: " +repr(source)+ " " + str(path) 
	split = path.split("/")
	Palette = split.pop()
	Palette = split.pop()
	if args[0]==1:
		#requests.get("http://192.168.1.104/arduino/state/"+str(int(state)-1))
		json.send({'command':'put', 'key':'palette', 'value':'%i' % int(int(Palette)-1)})
		msg = OSCMessage(path)
		msg.insert(0, 1)
		client.connect( (source[0], 9000) )
		client.send(msg)
for x in range(0,20):
	server.addMsgHandler("/1/palette/"+str(x)+"/1", multiselectPalette_callback)

  
def rotary1_callback(path, tags, args, source):
	print str(path) + " " + str(args[0])
	json.send({'command':'put', 'key':'rotary1', 'value':'%i' % int(args[0])})
	#create feedback for faders label:	
	msg = OSCMessage("/1/label_rotary1")
	msg.insert(0, int(args[0]))
	client.connect( (source[0], 9000) )
	client.send(msg)
server.addMsgHandler("/1/rotary1/", rotary1_callback)
	
def xypad_callback(path, tags, args, source):
	print str(path) + " " + str(int(args[0]))+ " " +str(int(args[1]))
	json.send({'command':'put', 'key':'xypad1', 'value':'%i' % int(args[0])})
	json.send({'command':'put', 'key':'xypad2', 'value':'%i' % int(args[1])})	
server.addMsgHandler("/1/xypad/", xypad_callback)

def accell_callback(path, tags, args, source):
	print str(path) + " " + str(float(args[0]))+ " " +str(float(args[1])) + " " +str(float(args[2]))
	json.send({'command':'put', 'key':'accxyz', 'value':'%i' % int(args[0])})	
server.addMsgHandler("/accxyz/", accell_callback)

def push1_callback(path, tags, args, source):
	print str(path) 
	json.send({'command':'put', 'key':'push1', 'value':'255' })
server.addMsgHandler("/1/push1/", push1_callback)

def push2_callback(path, tags, args, source):
	print str(path) 
	json.send({'command':'put', 'key':'push2', 'value':'255' })
server.addMsgHandler("/1/push2/", push2_callback)

def push3_callback(path, tags, args, source):
	print str(path) 
	json.send({'command':'put', 'key':'push3', 'value':'255' })
server.addMsgHandler("/1/push3/", push3_callback)

while True:
	server.handle_request()

server.close()