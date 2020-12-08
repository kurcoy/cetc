import CRCCCITT
import SerialPort
from HexConverter import Converter

index = 0
uid_len = 13
uid = 7300000000000 + index
psw = 11111100 + index
shell_len = 13
shell = uid
psw_len = 8

header = '444C'
type = '03'
content = ''
content_qty= 0
for i in range(10):
    uid = 7300000000000 + i +1
    psw = 11111100 + i +1 
    shell = uid 
    content_qty = content_qty +1+i
    content = content+'%02X' %((uid_len))+Converter.asciiTohex(str(uid))+'%02X' %((shell_len))+Converter.asciiTohex(str(shell))+'%02X' % (psw_len)+Converter.asciiTohex(str(psw))
    
content_len = int(len(content)/2)
buffer = type + '%02X' %content_qty + '%04X' %content_len + content
print(buffer)
chk_sum = CRCCCITT.CRCCCITT('FFFF').calculate(bytes(bytearray.fromhex(buffer)))
frame = header+buffer+'%04X' %chk_sum
print(frame)

SerialPort.show_all_com()

wcSerial=SerialPort.SerialPort('COM2', 9600)
wcSerial.send_data(bytes(bytearray.fromhex(frame)))
data = wcSerial.read_data(6)
print(Converter.byteTohex(data))

