# -*-coding: utf-8 -*-

import serial
import serial.tools.list_ports
import threading
import time
#列出所有当前的com口
port_list = []
port_list_name = []

class SerialPort(object):
    def __init__(self,port="COM1",baund=115200):
        show_all_com()
        if port in port_list_name:
            self.port = serial.Serial(port,baund)
            self.port.close()
            if not self.port.isOpen():
                self.port.open()
        else:
            print("invalid comport")

    def port_open(self):
        if not self.port.isOpen():
            self.port.open()

    def port_close(self):
        self.port.close()

    def send_data(self, data):
        print("send:",data)
        self.port.write(data)
        time.sleep(1)

    def read_data(self,  readbyte):
        i = 0 
        while i<readbyte:
            count = self.port.inWaiting()
            if count > 0:
                readData = self.port.read(count)
                print("receive:",readData)
                i = count
                
        return  readData       

def show_all_com():
    port_list = list(serial.tools.list_ports.comports())
    if len(port_list) <= 0:
        print("the serial port can't find!")
    else:
        for itms in port_list:
            port_list_name.append(itms.device)
        print(port_list_name)
