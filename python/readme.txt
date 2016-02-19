This python script are run on the ARM side of the Yun.
It implements a OSC Server that will accept following OSC packages.
The yun keystore (http://your-yun/keystore_manager_example/ will keep the values under the described key.

Prerequisites:
- Install pyosc (python-pyosc) by going to the Yun config page and clicking 'configure -> advanced -> system -> software -> Available packages'
- autorun on yun startup by adding following line before "exit 0" in 'configure -> advanced -> system -> Startup -> Local Startup"
  (sleep 10;python /root/LEDita_parseOSC.py)&

OSC Packages:
- /1/light/<0-255>
  key: brightness
  changes overall brightness.
  
- /1/state/<0-7>/1
  key: state
  change the background animation to <first-number>. 
  Only one state can be active   
  
- /1/fps/<0-255>
  key: fps
  change painting speed (debug feature)
  
- /1/filters/<1-5>/1 <0-1>
  key: filters
  NOT IMPLEMENTED on uC
  sets the filter <first-number> to on/off <second-number>.
  multiple filters can be on/off

- /1/palette/<0-8>/1
  key: palette
  sets the current color palette (=color scheme)
  used in palette animation and might be used in more animations
  
- /1/rotary1 <0-120>
  key: rotary1
  general rotary, often used for speed (60=normal) 

- /1/xypad/ <0-255> <0-255>
  key: xypad1 <first> , xypad2 <second>
  not used yet.

- /1/accxyz/
  key: accxyz
  not used

- /1/push1/
  key: push1
  not used
  
- /1/push2/
  key: push2
  not used

- /1/push2/
  key: push2
  not used
  