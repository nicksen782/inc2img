# inc2img
PURPOSE: Converts Uzebox Gconvert '.inc' files back into actual graphics. This is a sort of reverse of what Gconvert does.

ENVIRONMENTS: Intended to be run from Linux (Ubuntu) but does compile for at least Windows 7.

WHY?: I wanted to create an Uzebox tile editor which would have simple pixel-based editing. It seemed easier if we could take the Gconvert .inc output and directly edit it. That means that it must be able to see it as the graphical data it represents. Additionally, a program like that would be able to better identify tile id numbers. 

HOW TO USE: 

ENGLISH: Invoke the program name, specify the .inc file (doesn't actually NEED to end in .inc), the tileset name, and the video mode number. 

CMD EXAMPLE: $ ./c_inc2img input/sokoban_tiles.inc sokoban_tiles 10

EXPECTED OUTPUT: 
************************************************************************************************* 
Uzebox Tileset .inc to img Converter -- version 1.0 (Compiled on: Jun  3 2016 _ 04:07:03, UTC-5)
        Program Name: c_inc2img
        Author: Nickolas Andersen (nicksen782)
        Purpose: Convert gconvert .inc tileset files to a viewable bitmap/png image.
        Name stands for: C .inc to .img

The following are the program arguments
        argc: 4 
        argv[0]: ./c_inc2img, 
        argv[1]: input/sokoban_tiles.inc, 
        argv[2]: sokoban_tiles, 
        argv[3]: 10, 
arraySearchString: const char sokoban_tiles 

Searching for:  'const char sokoban_tiles'

Chars between '{' and '}': 26589
Allocating 26589 bytes for TEXT tilesheet data array.

**** copyText2Array: ****
Searching for:  'const char sokoban_tiles'
Found the line with the size string: 'const char sokoban_tiles'

Now allocating 24961 bytes for the final c array.
Allocating 24961 bytes for TEXT tilesheet data array.
vid equals: 10

**** decodeinc ****
Processing 4992 bytes for the tilesheet: sokoban_tiles
Total of 26 tiles, 
numpixelsintile: 192, 
Tile width: 12, Tile height: 16 

Image dimensions: 312 by 16 pixels.
Image will have 26 by 1 tiles
Check: 26 * 1 = 26
Outputting image to:output/sokoban_tiles.bmp
output/sokoban_tiles.png
