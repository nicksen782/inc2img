
// C Library Includes
#include <stdio.h>					// http://www.cplusplus.com/reference/cstdio/
#include <stdlib.h> 				// exit, malloc, free, strtol
#include <sys/stat.h>				// stat, mkdir
#include <cerrno>
#include <unistd.h>					// Needed by LodePNG.

// 3rd Party Includes
#include "EasyBMP/EasyBMP.h"		// To create .bmp images.
#include "bmp2png.cpp"

using namespace std;

// Defines
#define VERSION_MAJ 1
#define VERSION_MIN 1
#define VERSION_BUILDTIME __DATE__ " _ " __TIME__
#define EST (-5)

// Function Prototypes

// Runs through the .inc file, finds the array, then counts how many values the array will need. 
unsigned int calcArraySize(
	FILE * filepointer, 
	char * arraySearchString
) ;

// Copies the .inc tileset array to a malloc'd array. Converts text to the value represented by the text.
unsigned int copyText2Array(
	FILE * fptr, 
	char * arraySearchString, 
	unsigned int textArraySize, 
	char * pre_c_array, 
	int * numvaluesinarray
);

// Accepts the converted .inc file array. Uses that data to create a .bmp file. 
void decodeinc(
	unsigned char alltiles[], 	// 
	long int ALLTILES_SIZE, 	// 
	char * spritesheetname,		// 
	unsigned int tilewidthx,	// The width of each tile.
	unsigned int tileheighty	// The height of each tile. 
);

// Converts a .bmp file to .png. 
unsigned decodeBMP(
	std::vector<unsigned char>& image, 
	unsigned& w, 
	unsigned& h, 
	const std::vector<unsigned char>& bmp
);