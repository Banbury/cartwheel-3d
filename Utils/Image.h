#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <Utils/UtilsDll.h>


#include <Utils/Utils.h>

typedef unsigned char byte;

/*================================================================================================================================*
 | This class implements a simple image class. The image is a bitmap of pixels, where each pixel can be represented, for instance |
 | by one (grayscale), three (RGB) or four bytes (RGBA). This image can be used as the object returned by reading an image file,  |
 | such as a BMP, and it can be used for texture mapping.                                                                         |
 *================================================================================================================================*/


#define WHITE 255
#define BLACK 0


/**
	This macro is used to get the location of a certain byte from a certain pixel of the image. It makes sure that we don't go past the boundaries of the image.
*/
#define IMAGELOCATION(image, x, y, whichByte) (													\
						((x)<0 || (y)<0 || (x)>=(image)->m || (y)>=(image)->n) ? 				\
						(-1) : 																	\
						(((image)->nrBytesPerPixel)*((x)*(image)->n+(y))+whichByte)				\
					      )

/**
	This macro returns the value of the byte at the given location in an image. Again, measures are taken to make sure that we don't go past the boundaries of the image
*/
#define GETIMAGEAT(image,location) (																						\
					(location < 0 || location >= (image)->m*(image)->n*(image)->nrBytesPerPixel || (image)->data == NULL)	\
					? (0): ((image)->data[location])																		\
				)

/**
	This macro is used to set the value at a specified location. Again, measures are taken to make sure that we don't go past the boundaries of the image
*/
#define SETIMAGEAT(image,location,newValue)	if ((location) >=0 && (location) < (image)->m*(image)->n*(image)->nrBytesPerPixel && image->data != NULL){	\
							image->data[location] = newValue;																							\
						}
				


class UTILS_DECLSPEC Image {
protected:
	//number of rows - height
	int m;
	//number of cols - width
	int n;		
	//we have an nxm image (but mxn matrix). Note that the axis are as follows:
	//   -------> Y (goes from 0 to n-1)
	//   |
	//   |
	//   |
	// X v
	// (goes from 0 to m-1)

	//this is the array where all the data will be stored - note that the values here are not constrained to be in the range 0..255 - but they can always be normalized.
	byte *data;

	//the nr of bytes used to represent a pixel - 1 for grayscale, 3 for RGB and 4 for RGBA
	int nrBytesPerPixel;	

	/**
		This method is used to create an exact copy of the current image.
	*/
	Image* duplicate();

public:
	/**
		This constructor creates a new image with the information available - makes a copy of the data!!! Throws exception on bad input.
		If imageData is null, then the resulting image is filled with 0's
	*/
	Image(int nrBytes, int cols, int rows, byte *imageData);
	/**
		This constructor makes a copy of the image that is passed in.
	*/
	Image(Image& img);

	/**
		destructor - frees up all memory
	*/
	~Image();
	
	/**
		a list of getters
	*/
	inline int getWidth() {return n;}
	inline int getHeight() {return m;}
	inline int getNrBytes() {return nrBytesPerPixel;}


	/**
		write a copy operator.
	*/
	Image& operator = (Image& img){
		//make sure we free up the space that we occupied before.
		if (data != NULL)
			free(data);
		this->n = img.n;
		this->m = img.m;
		this->nrBytesPerPixel = img.nrBytesPerPixel;
		data = (byte*)malloc(n*m*nrBytesPerPixel*sizeof(byte));
		if (data == NULL)
			throwError("Unable to allocate memory while copying image.");
		memcpy(data,img.data,n*m*nrBytesPerPixel*sizeof(byte));
		return *this;
	}


	/**
		gets the value of a given pixel - should be used for gray level images. If used for 24 bit images, this method will return the Red component
	*/
	byte getPixelAt(int x, int y);
	/**
		these four methods return RGBA values of a pixel - if the image is gray level (8-bit), these methods will do the same thing as getPixelAt.
		if the image is not an RGBA image, the getAPixel will return the same thing as the getPixelAt method.
	*/
	byte getRPixelAt(int x, int y);
	byte getGPixelAt(int x, int y);
	byte getBPixelAt(int x, int y);
	byte getAPixelAt(int x, int y);

	/**
		sets the value of a given pixel - should be used for gray level images
	*/
	void setPixelAt(int x, int y, byte newValue);
	/**
		sets the RGBA value of a given pixel - if used for a gray level image, it does the same thing that setPixelAt does.
		if the image is not an RGBA image, the setAPixel will do not do anything.
	*/
	void setRPixelAt(int x, int y, byte newValue);
	void setGPixelAt(int x, int y, byte newValue);
	void setBPixelAt(int x, int y, byte newValue);
	void setAPixelAt(int x, int y, byte newValue);


	/**
		converts this image to 8 bit, grayscale image
	*/
	void convertToGrayScale();

	/**
		this method returns a pointer to the image data
	*/
	byte* getDataPointer(){
		return data;
	}
};
