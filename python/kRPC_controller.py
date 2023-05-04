import krpc
import serial
# import argparse
import time
from threading import Thread
# import logging
byteDelay = 0.02
abort = False
conn = krpc.connect(name='Launch into orbit')
vessel = conn.space_center.active_vessel
def calc_color():
    sit = vessel.situation
    print(sit.name)
    if abort:
        return (1,0,0)
    elif sit.name=="pre_launch":
        return (0,1,1)
    elif sit.name=="flying":
        return (0,0,1)
    elif sit.name=="sub_orbital":
        return (0,1,1)
    elif sit.name=="orbiting" or sit.name=="escaping":
        return (0,1,0)
    elif sit.name=="landed" or sit.name=="splashed" or sit.name=="docked":
        return (1,0,1)
    
    else:
        return (1,1,1)
    
def write_data(ser):
    while True:
        ctr = vessel.control
        color = calc_color()
        ser.write(bytes([0x80]))
        time.sleep(byteDelay)
        ser.write(bytes([ctr.gear+2*ctr.brakes+4*ctr.parachutes+8*ctr.lights+16*ctr.get_action_group(1)+32*ctr.get_action_group(2)+64*(not abort)]))
        time.sleep(byteDelay)
        ser.write(bytes([0x81]))
        time.sleep(byteDelay)
        ser.write(bytes([vessel.control.sas+2*vessel.control.rcs+4*color[0]+8*color[1]+16*color[2]]))
        time.sleep(byteDelay)


with serial.Serial('COM3', 115200,timeout=1,) as ser:
    last_state = [-1,-1,-1,-1]
    write_data_thread = Thread(target=write_data,args=[ser],daemon=True)
    write_data_thread.start()
    while True:
        data = ser.read_until(bytes([255]))
        while len(data) <4:
            data += ser.read_until(bytes([255]))
        vessel.control.throttle = min(1,data[2]/124)
        if not vessel.control.abort:
            abort = False
        if not (last_state[1]&2) and data[1]&2:
            vessel.control.rcs = not vessel.control.rcs
        if not (last_state[1]&1) and data[1]&1:
            vessel.control.sas = not vessel.control.sas
        if not (last_state[1]&4) and data[1]&4:
            vessel.control.activate_next_stage()
        if not (last_state[1]&8) and data[1]&8:
            abort = True
            vessel.control.abort = True
        if not (last_state[0]&1) and data[0]&1:
            vessel.control.gear = not vessel.control.gear
        if not (last_state[0]&2) and data[0]&2:
            vessel.control.brakes = not vessel.control.brakes
        if not (last_state[0]&4) and data[0]&4:
            vessel.control.parachutes = True
        if not (last_state[0]&8) and data[0]&8:
            vessel.control.lights = not vessel.control.lights
        if not (last_state[0]&16) and data[0]&16:
            vessel.control.toggle_action_group(1)
        if not (last_state[0]&32) and data[0]&32:
            vessel.control.toggle_action_group(2)
        
        
        last_state = data

        

                
  