# gct (Gcode Tool)
gct is a tool to change the scale and offset of a Gcode file
it's a very simple command line tool, but fast and effective

# compilation
make

# usage
./gct source.gcode 
-> return the minimum and maximum position on all axes (XYZ)

./gct source.gcode destination.gcode
-> create a destination.gcode identical to the source.gcode

./gct source.gcode destination.gcode [arguments]
-> alter the source.gcode and create a destination.gcode according to the modification provided as arguments

#arguments 
ScaleX float(x) : rescale the X values by a factor of x
ScaleY float(x) : rescale the Y values by a factor of x
ScaleZ float(x) : rescale the Z values by a factor of x
OffsetX float(x) : add an offset of value x to all X coordinate
OffsetY float(x) : add an offset of value x to all Y coordinate
OffsetZ float(x) : add an offset of value x to all Z coordinate
OffsetX : remove the offset of the X axes so the minimum X value is O
OffsetY : remove the offset of the Y axes so the minimum Y value is O
OffsetZ : remove the offset of the Z axes so the minimum Z value is O
SizeX float(x): rescale the X values by a factor of x/Xmax, so the maximum X size is x
SizeY float(x): rescale the Y values by a factor of x/Ymax, so the maximum Y size is x
SizeZ float(x): rescale the Z values by a factor of x/Zmax, so the maximum Z size is x  

multiples arguments are possible but they can interfere with each other     

#example
./gct oldfile.gcode newfile.gcode SizeX 10 SizeY 10
	will rescale the project to be 10 unit in X and Y

./gct oldfile.gcode newfile.gcode offsetX 2 sizeX 20
	will result of the X axes to be scaled between 2 and 22

./gct oldfile.gcode newfile.gcode ScaleX 2 SizeX 10
	the Scale and Size re incompatible, only the last one is used
