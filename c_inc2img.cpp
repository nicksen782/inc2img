// Program Name: c_inc2img
// Name stands for: C .inc to .bmp
// Purpose: Convert gconvert .inc tileset files to a bitmap.
// Environment: Command-line application
// Notes: Designed/tested using Linux (Ubuntu). Will compile for Windows also.
// Author: Nickolas Andersen (Nicksen782)
// Original date: Saturday, March 15, 2014, 10:22:00 PM
// Resurection:   Tuesday, December 9, 2014, 22:00 PM
// Solid codebase: Friday, January 2, 2015, 2:46 AM
//
// FEATURES:
// Converts the specified tileset in the gconvert/TileStudio .inc file to a viewable .bmp/.png file.
//
// https://gcc.gnu.org/onlinedocs/cpp/Standard-Predefined-Macros.html
// Use: ./c_inc2img sprites.inc sprites1 3
#include "c_inc2img.h"

int main( int argc, char* argv[] ){
	// Program identification.
	printf("\n************************************************************************************************* \n");
	printf("Uzebox Tileset .inc to image Converter -- version %i.%i (Compiled on: %s, UTC%i)\n",VERSION_MAJ, VERSION_MIN, VERSION_BUILDTIME, UTC);
	printf("\tProgram Name: c_inc2img\n");
	printf("\tAuthor: Nickolas Andersen (nicksen782)\n");
	printf("\tPurpose: Convert gconvert .inc tileset files to a viewable bitmap/png image.\n");
	printf("\tName stands for: C .inc to image\n\n");

	// Display values that were passed to the program.
	int arg_count = 0;
	printf( "The following are the program arguments\n");
	printf( "\targc: %i \n", argc);
	for(arg_count = 0; arg_count < argc; arg_count++) {
		printf( "\targv[%i]: %s, \n", arg_count, argv[arg_count]);
	}

	// Check that an argument was passed to the program.
	if(argc==1){
		printf("Error: No input file provided.\n\n");
		printf("Example usage: c_inc2img <gconvert_output.inc> <name of tilesheet>\n");
		printf("We're sorry your call can not be completed as dialed Please check the number and dial again or call your operator to help you\n");
		exit(1);
	}

	// Open the file that was passed.
	FILE *incfile = fopen(argv[1], "r");
	if (!incfile){
		printf("Error: Unable to open the file:  %s\n", argv[1] );
		printf("Is this a gconvert output .inc file?\n");
		return false;
	}

	// Create the container that will hold the tileset definition line.
	char str1[12+170] = "const char ";	// "const char " is 12 chars including '\0'. The extra 50 is for the filename which will vary.
	strcat (str1, argv[2]); 			// Will combine the strings. The 80 is just a large number. It is safe to go too high.
	unsigned char arraySearchString_mallocsize = sizeof(unsigned char) + (12+170) ;
	char * arraySearchString = (char *) malloc(arraySearchString_mallocsize) ;

	// Get the tileset size.
	strncat(arraySearchString, str1, 280);
	printf("arraySearchString: %s \n", arraySearchString);
	unsigned int textArraySize = calcArraySize(incfile, arraySearchString);

	// Allocate RAM for the text tileset array.
	unsigned int mallocsize = ( sizeof( char) ) * (textArraySize);
	printf("Allocating %i bytes for TEXT tilesheet data array.\n", mallocsize);
	char * pre_c_array = ( char *) malloc(mallocsize) ;
	if ( pre_c_array == NULL ) { (void)fprintf(stderr, "ERROR: Malloc failed"); (void)exit(EXIT_FAILURE); }

	// Read the array text into the new local array.
	int numvaluesinarray = 0;		// Will be edited by the copyText2Array function.
	unsigned int xcount = copyText2Array(incfile, arraySearchString, textArraySize, pre_c_array, &numvaluesinarray);
	printf("\nNow allocating %i bytes for the final c array.\n", xcount);
	// (char*) realloc (pre_c_array, xcount * sizeof(char));

	// Allocate RAM for the final array.
	unsigned int mallocsize_final = (sizeof(unsigned char)) * ((1) * (xcount));
	printf("Allocating %i bytes for TEXT tilesheet data array.\n", mallocsize_final);
	unsigned char * c_array = (unsigned char *) malloc(mallocsize_final) ;
	if ( c_array == NULL ) { (void)fprintf(stderr, "ERROR: Malloc failed"); (void)exit(EXIT_FAILURE); }

	// Convert the individual text values into actual numbers.
	char * pEnd;			// This is used with strtol.
	int  c_array_i = 0 ;	// This is used as the converted strings array index
	pEnd = pre_c_array;
	while(c_array_i < numvaluesinarray){
		c_array[c_array_i] = strtol (pEnd, &pEnd, 16);
		c_array_i ++;
	}

	// Determine tile dimensions by observing the video mode.
	unsigned int tilewidthx  = 0 ;	// Will be passed to decodeinc.
	unsigned int tileheighty = 0 ;	// Will be passed to decodeinc.
	unsigned char vid = 0  ;	// Video mode argument
	pEnd = argv[3];						//
	vid = strtol(pEnd, &pEnd, 10);
	printf("vid equals: %i\n", vid);

	// This uses the video mode passed to determine the tile width/height.
	if(vid == 1 || vid == 2 || vid == 5){	tilewidthx = 6 ;	tileheighty = 8 ;}
	else if(vid == 3) { tilewidthx = 8 ;	tileheighty = 8 ;	}
	else if(vid == 4) { tilewidthx = 16 ;	tileheighty = 16 ;	}
	else if(vid == 10){ tilewidthx = 12 ;	tileheighty = 16 ;	}
	else{ printf("ERROR, specified video mode unavailable. PROGRAM ABORT\n"); exit(1);}

	// Send the new c array to the image creator.
	// It will also generate a .png file.
	// array of raw values, size of array, tilesheet name, tile width, tile height.
	decodeinc(
		c_array, numvaluesinarray,
		argv[2],
		tilewidthx, tileheighty
	);

	// Last, free the malloc'd arrays and close the file. We are done.
	free(pre_c_array);
	free(c_array);
	fclose(incfile);
}

unsigned int calcArraySize(FILE * fptr, char * arraySearchString) {
	// This counts ALL characters between the '{' and '}' including comments.
	// The RAM allocation does not need to be perfect. It needs to be big enough or bigger than enough.
	printf("\nSearching for: \t'%s'\n", arraySearchString);
	char thisline[400];			// This is read into from the file.
	char tl_inx = 0;			// This line index.
	char c;						// Holds 1 char from the file.
	unsigned int bytesintileset =0 ;

	// Blank the thisline array.
	int i; for(i=0; i<sizeof(thisline); i++){ thisline[i] = '\0'; } i =0;

	// Read one line at a time. Check the line for the arraySearchString.
	while( !feof(fptr) ){
		tl_inx = 0;
		c = fgetc(fptr);
		while( !feof(fptr) && c != 0x0A && tl_inx < sizeof(thisline) ) {
			thisline[tl_inx] = c; tl_inx++;
			c = fgetc(fptr);
			// If this char was the 0x0A then add it to thisline. Then break the while loop.
			if(c == 0x0A) {
				thisline[tl_inx] = c;
				break;
			}
		} tl_inx = 0;

		// If arraySearchString is found then skip ahead to the '{'.
		if( strstr(thisline, arraySearchString) ){
			// printf("Found the line with the size string: %s", thisline);
			while( !feof(fptr) && c != '}'){
				c = fgetc(fptr); bytesintileset++;
			}
		}
	}
	printf("\nChars between '{' and '}': %i\n", bytesintileset);
	return bytesintileset ;
}

unsigned int copyText2Array(FILE * fptr, char * arraySearchString, unsigned int textArraySize,  char * pre_c_array, int * numvaluesinarray){
	printf("\n**** copyText2Array: ****\nSearching for: \t'%s'\n", arraySearchString);
	char thisline[400];			// This is read into from the file.
	char tl_inx = 0;			// This line index.
	char c;						// Holds 1 char from the file.
	unsigned int bytesintileset =0 ;
	unsigned int ainx = 0 ;
	int i;
	char stringfound = false;

	// Blank the thisline array. Then rewind the file pointer.
	for(i=0; i<textArraySize; i++){ pre_c_array[i] = '\0'; } i=0;
	rewind (fptr);

	// Read one line at a time. Check the line for the arraySearchString.
	while( !feof(fptr) && stringfound != true ){
		for(i=0; i<sizeof(thisline); i++){ thisline[i] = '\0'; } i=0;
		tl_inx = 0;
		c = fgetc(fptr);
		while( !feof(fptr) && c != 0x0A && tl_inx < sizeof(thisline) ) {
			thisline[tl_inx] = c; tl_inx++;
			c = fgetc(fptr);
			// If this char was the 0x0A then add it to thisline. Then break the while loop.
			if(c == 0x0A) {
				thisline[tl_inx] = c;
				break;
			}
			if( strstr(thisline, arraySearchString) ){
				printf("Found the line with the size string: '%s'\n", thisline);
				stringfound = true;
				break;
			}
		} tl_inx = 0;
	}

	// Skip to the '{'.
	while( !feof(fptr) && c != '{'){
		c = fgetc(fptr);
	}
	c = fgetc(fptr); // Skips the '{'.

	// Read the data between the '{' and '}'.
	while( !feof(fptr) && c != '}'){
		c = fgetc(fptr);

		// If line break, spaces or tabs then skip this iteration.
		if(c==0x0A || c==0x20 || c==0x09) { continue; }

		// Skip comments and tabs
		if(c=='/'){
			c = fgetc(fptr) ;
			if(c == '/'){
				// Line comment detected. Skip chars until 0x0A.
				while( !feof(fptr) && c != 0x0A){
					c = fgetc(fptr) ;
					continue;
				}
			}
			else if(c == '*'){
				// Multi-line comment detected. Skip chars until '*' then '/'.
				c = fgetc(fptr) ;
				while( !feof(fptr) && c != '*'){
					c = fgetc(fptr) ;
				}
				c = fgetc(fptr) ;
				if(c != '/'){
					printf("ERROR with multi-line comment!\n");
					exit(1);
				}
					continue ;
			}
		}
		// Only store printable characters. ASCII space, comma, and letters and numbers.
		if( c == 0x20 || c == 0x2C || (c >= 0x30 && c <= 0x7D) ){
			pre_c_array[ainx] = c ;
			ainx++;
		}
	}

	// We have the array. Now remove the ',' and '}'.
	for(i=0; i<ainx; i++){
		if(pre_c_array[i] == ',' || pre_c_array[i] == '}'){
			pre_c_array[i] = ' ';
		}
	}

	// Count transitions from space to non-space to determine array length.
	*numvaluesinarray = 0;
	char prev_char = ' ';
	for(i=0; i<ainx; i++){
		if(prev_char == ' ' && pre_c_array[i] != ' '){
			(*numvaluesinarray)++;
		}
		prev_char = pre_c_array[i];
	}

	return ainx ;
}

void decodeinc(unsigned char  alltiles[], long int ALLTILES_SIZE, char * spritesheetname, unsigned int tilewidthx, unsigned int tileheighty){
	printf("\n**** decodeinc ****\nProcessing %li bytes for the tilesheet: %s\n", ALLTILES_SIZE, spritesheetname);
	// Variable init.
	unsigned int tilestartX = 0;	// Beginning X value for the current tile.
	unsigned int tilestartY = 0;	// Beginning Y value for the current tile.
	unsigned int x = 0; 			// Value added to tilestartX to draw the current pixel in the current tile's X coordinate.
	unsigned int y = 0; 			// Value added to tilestartY to draw the current pixel in the current tile's Y coordinate.
	unsigned int tpp = 0;			// Tile Pixel Position. Counter for the pixel number of a tile. Total of 64 per tile.
	unsigned int tilenumber = 0; 	// Current tile that is being drawn.
	unsigned int currentpixel = 0; 	// Value of the tile from tilesheet. This helps to keep code a bit shorter.
	unsigned int numpixelsintile =0;// The number of pixels per tile..
	unsigned int np_red   = 0 ;		// Holds the red value for the current pixel.
	unsigned int np_green = 0 ;		// Holds the red value for the current pixel.
	unsigned int np_blue  = 0 ;		// Holds the red value for the current pixel.
	unsigned int tilesPerRow ; 		// Number of tiles per row (horizontal)
	unsigned int tilesPerColumn = 0 ; 	// Number of tiles per column (vertical)
	unsigned int numberoftiles = 0 ; 	// Total number of tiles in tilesheet.
	unsigned int bmpwidth    = 0 ;		// Width of .bmp.
	unsigned int bmpheight   = 0 ;		// Height of .bmp.
	unsigned int thispixel_x = 0 ; 		// How many pixels away from the y start.
	unsigned int thispixel_y = 0 ;		// How many pixels away from the y start.

	numpixelsintile = tilewidthx * tileheighty ;
	numberoftiles = ALLTILES_SIZE/numpixelsintile ;
	printf("Total of %i tiles, \nnumpixelsintile: %i, \nTile width: %i, Tile height: %i \n\n", numberoftiles, numpixelsintile, tilewidthx, tileheighty);

	BMP Output;
	if(numberoftiles<64){ tilesPerRow = numberoftiles ; }
	else{ tilesPerRow = 64; }
	tilesPerColumn = (numberoftiles / 64) +1 ;
	bmpwidth    = (tilesPerRow    * tilewidthx) ;
	bmpheight   = (tilesPerColumn * tileheighty) ;
	thispixel_x = 0 ; thispixel_y = 0 ;

	Output.SetSize(bmpwidth, bmpheight);
	Output.SetBitDepth(24);	// 24-bit bitmaps are simple to use
	printf("Image dimensions: %d by %d pixels.\nImage will have %d by %d tiles\n", Output.TellWidth(), Output.TellHeight(), Output.TellWidth()/tilewidthx, Output.TellHeight()/tileheighty);
	printf("Check: %d * %d = %d\n", Output.TellWidth()/tilewidthx, Output.TellHeight()/tileheighty, Output.TellWidth()/tilewidthx * Output.TellHeight()/tileheighty);

	for(tilenumber=0; tilenumber < numberoftiles; tilenumber ++){
		// Skip this part if on the first tile.
		// This code prevents the first row from being blank.
		if(tilenumber != 0){
			// Is this the end of the row?.
			if   ( tilenumber % tilesPerRow == 0)	{
				tilestartY += tileheighty +0;
				tilestartX = 0 ;
			}
			// Next tile right.
			else	{ tilestartX += tilewidthx +0; }
		}
		// Draw out the tile pixel by pixel.
		for(y=0; y<tileheighty; y++){
			for(x=0; x<tilewidthx; x++){
				// Get the current byte in the tile and convert to the correct colors.
				// http://uzebox.org/forums/viewtopic.php?f=6&t=396#p5727
				currentpixel = alltiles[(tilenumber * numpixelsintile) + tpp];
				thispixel_x = tilestartX + x;
				thispixel_y = tilestartY + y;

				if(thispixel_x < bmpwidth && thispixel_y < bmpheight) {
					np_red   = (((currentpixel >> 0) & 0x07) * (255/7)) ; // 3 bits
					np_green = (((currentpixel >> 3) & 0x07) * (255/7)) ; // 3 bits
					np_blue  = (((currentpixel >> 6) & 0x03) * (255/3)) ; // 2 bits

					// Use the color bytes to set the color values of this pixel.
					Output(tilestartX + x, tilestartY + y) -> Red   = np_red;
					Output(tilestartX + x, tilestartY + y) -> Green = np_green;
					Output(tilestartX + x, tilestartY + y) -> Blue  = np_blue;
					Output(tilestartX + x, tilestartY + y) -> Alpha = 0 ;
				}
				else {
					printf("** PROGRAM ABORT: Tile #%i, In %s: Pixel out of bounds. \nBounds are %i by %i. Tried %i, %i\n", tilenumber, spritesheetname, bmpwidth, bmpheight, thispixel_x, thispixel_y);
					exit(1);
				}

				// Continue to next pixel until end of tile row
				tpp++;
			}
		}
		tpp=0; // Reset the Tile Pixel Position counter.
	}

	cout << "Outputting image to:" ;

	// Make the output file name have the same name as the sprite sheet.
	char filename1[50] ;
	char filename2[50] ;
	sprintf(filename1, "output/%s.bmp", spritesheetname);
	sprintf(filename2, "output/%s.png", spritesheetname);
	filename1[sizeof(filename1)] = '\0';
	filename2[sizeof(filename2)] = '\0';
	cout << filename1 << endl;
	cout << filename2 << endl;

	printf("\n");

	// Create the output directory if it doesn't already exist.
	struct stat st = {0};
	if (stat("output", &st) == -1) {
		// Proprocessor directives. mkdir works differently between Linux and Windows.
		#if defined (__WIN32__)
			mkdir("output");
		#else
			mkdir("output", 0777);
		#endif
	}

	// Delete the old file(s).
	remove(filename1);
	remove(filename2);

	// Now output the file.
	Output.WriteToFile(filename1);

	// Use the new .bmp to create a .png.
	convertbmp2png(filename1, filename2);
}
