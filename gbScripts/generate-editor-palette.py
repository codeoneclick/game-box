"""Generate the linear UNORM 24-level x 7-climate data lookup used by shaders."""
from PIL import Image
original=[(194,141,64),(113,132,31),(74,122,24),(41,108,21),
          (55,25,10),(77,83,83),(100,106,108),(128,134,138)]
tints=[(1,1,1),(1.10,1.03,.82),(.80,1.18,.79),(.72,.90,.85),
       (.86,.88,.78),(1,1.08,1.20),(1.30,1.03,.60)]
image=Image.new('RGBA',(24,7))
for climate in range(7):
 for column in range(24):
  level=column-8 if column<8 else column-7
  c=(177,163,118) if level<0 else original[level-1] if level<=8 else tuple(original[7][i]+(240-original[7][i])*(level-8)/8.0 for i in range(3))
  value=[]
  for i in range(3):
   v=min(1,c[i]/255.0*tints[climate][i])
   if climate==5 and level>0:v=v*.25+.72
   value.append(int(round(v*255)))
  # PNG loader reverses rows; retain row0=Temperate in the GPU texture.
  image.putpixel((column,6-climate),tuple(value+[255]))
image.save('gbBundle/Textures/editor_climate_palette.png')
print('Generated editor_climate_palette.png: 24 x 7 linear RGBA entries')
