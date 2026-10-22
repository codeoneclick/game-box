"""Original procedural pixel characters and a copyright-free PCM metronome."""
from pathlib import Path
import zlib,struct,wave,math
root=Path(__file__).resolve().parent.parent/'gbBundle'
w,h=256,32;pixels=bytearray(w*h*4)
def rect(cell,x,y,ww,hh,c):
    for yy in range(y,y+hh):
        for xx in range(x,x+ww):
            index=(yy*w+cell*32+xx)*4;pixels[index:index+4]=bytes(c)
colors=[(246,66,150,255),(30,210,234,255),(157,89,240,255),(238,177,43,255),(48,197,135,255),(228,76,58,255),(90,141,239,255),(244,244,250,255)]
for i,c in enumerate(colors):
    skin=(238,181-i*7,139-i*5,255);dark=(25,31,55,255)
    rect(i,11,3,10,9,dark);rect(i,12,5,8,7,skin);rect(i,12,3,8,3,c)
    rect(i,10,12,12,10,c);rect(i,7,13,3,8,skin);rect(i,22,11-i%3,3,9,skin)
    rect(i,11,22,4,6,dark);rect(i,18,22,4,6,dark);rect(i,9,27,6,2,c);rect(i,18,27,6,2,c)
    rect(i,13,7,2,2,dark);rect(i,18,7,2,2,dark);rect(i,14,19,4,2,(255,255,255,255))
    if i%2:rect(i,11,7,11,2,dark)
def chunk(tag,data):return struct.pack('>I',len(data))+tag+data+struct.pack('>I',zlib.crc32(tag+data)&0xffffffff)
raw=b''.join(b'\0'+pixels[y*w*4:(y+1)*w*4] for y in range(h))
(root/'Textures/rhythm_ship_characters.png').write_bytes(b'\x89PNG\r\n\x1a\n'+chunk(b'IHDR',struct.pack('>IIBBBBB',w,h,8,6,0,0,0))+chunk(b'IDAT',zlib.compress(raw))+chunk(b'IEND',b''))
out=root/'Sounds/rhythm_ship_metronome.wav';out.parent.mkdir(exist_ok=True)
data=bytearray(30*48000*2)
for i in range(64):
    start=round(i*60/128*48000)
    for j in range(2400):
        x=j/48000;v=round(math.sin(x*math.tau*(1100 if i%4 else 1700))*math.exp(-110*x)*16000);struct.pack_into('<h',data,(start+j)*2,v)
with wave.open(str(out),'wb') as f:f.setnchannels(1);f.setsampwidth(2);f.setframerate(48000);f.writeframes(data)
print('Generated original character atlas and 128 BPM metronome')
