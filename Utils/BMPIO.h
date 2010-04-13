#pragma once 

#include <stdlib.h>
#include <stdio.h>

#include <Utils/UtilsDll.h>

#include <Utils/ImageIO.h>
#include <Utils/Image.h>

class BMPInfoHeader;


/*====================================================================================================================================================================*
 | This class extends the ImageIO class, and implementes the methods needed to write/load an image from a BMP file.                                                   |
 *====================================================================================================================================================================*/
class UTILS_DECLSPEC BMPIO : public ImageIO{
protected:
	/**
		This is the method that is responsible with the reading of the BMP data.
	*/
	void readBMPData(FILE* fp, Image* img, BMPInfoHeader* infoHeader);
	/**
		This method is used to allocate an image. This default method loads either an 8-bit or a 24-bit image, but it can easily be modified to
		load 32-bit images (RGBA) as well. The image size, nr bits, etc is loaded from the bitmap header that is passed in as a parameter.
	*/
	virtual Image* allocateImage(BMPInfoHeader *infoHeader, int nrBits = 24);
	/**
		This method is used to write an image to the file whose pointer is passed in as a parameter.
	*/
	void writeBMPData(FILE* fp, Image* img);
public:
	/**
		Default constructor.
	*/
	BMPIO(char* fileName): ImageIO(fileName){}
	/**
		Default destructor.
	*/
	virtual ~BMPIO(){}

	/**
		This method will load an image from the given BMP file.
	*/
	virtual Image* loadFromFile(int imageModel = RGB_MODEL);
	/**
		This method will save an image to the given BMP file.
	*/
	virtual void writeToFile(Image* img);
};


/**
	The following two defines correspond to the default header and headerinfo block sizes in a bitmap.
*/
#define HEADERSIZE 14
#define HEADERINFOSIZE 40

/**
	This class acts as a container for a bitmap header.
*/
class UTILS_DECLSPEC BMPHeader {
private:
	//id should always be 'B''M'
	unsigned short id;
	//this is the size of the file.
	unsigned int fileSize;
	//we won't be using these variables
	unsigned short int reserved1, reserved2;	
	//this is where the image data can be found - relative to the start of the file.
	unsigned int offset;
public:
	/**
		constructor that populates the data from a file.
	*/
	BMPHeader(FILE* fp);
	/**
		constructor that populates the data from an Image object
	*/
	BMPHeader(Image* img);
	/**
		Default destructor.
	*/
	~BMPHeader(){}
	/**
		This method writes the header of the BMP to a file whose pointer is passed in as a parameter.
	*/
	void writeHeader(FILE* fp);
};

/**
	This class acts as a container for a BMP HeaderInfo object.
*/
class UTILS_DECLSPEC BMPInfoHeader {
private:
	//header size in bytes - should be 40
	unsigned int size;
	//width and height of the image
	int width, height;
	//number of colour planes used
	unsigned short nrPlanes;
	//number of bits per pixel  - only accepting 8 and 24
	unsigned short nrBits;
	//indicates wether the data is compressed or not
	unsigned int compression;
	//this is the image size in bytes
	unsigned int imageSize;
	//pixels per meter - not sure what it's used for
	int xRes, yRes;
	//the number of colours used if dealing with palette info
	unsigned int nrColours;
	//number of important colours.
	unsigned int importantColours;
	//this is the pallete (if used). Note that the BMP's that will be written will not be using palette indexing.
	unsigned char *pallete;
public:
	/**
		constructor that populates the data from a file.
	*/
	BMPInfoHeader(FILE* fp);
    /**
		constructor that populates the data from an Image object
	*/
	BMPInfoHeader(Image* img);
	/**
		Default destructor.
	*/
	~BMPInfoHeader();
	
	/**
		this method writes the header of the BMP to a file.
	*/
	void writeInfoHeader(FILE* fp);
	
	//and a list of getters - nothing fancy here
	inline unsigned int getWidth(){ return this->width; }
	inline unsigned int getHeight(){ return this->height; }
	inline unsigned short getNrBits(){ return this->nrBits; }
	inline unsigned char* getPallete(){ return this->pallete; }
};


