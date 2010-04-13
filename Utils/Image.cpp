#include "Image.h"

/**
	This constructor creates a new image with the information available - makes a copy of the data!!! Throws exception on bad input.
	If imageData is null, then the resulting image is filled with 0's
*/
Image::Image(int nrBytes, int cols, int rows, byte* imageData){
	if (nrBytes!=1 && nrBytes!=3 && nrBytes!=4)
		throwError("Unsupported number of bytes.");
	if (cols<=0 || rows<=0)
		throwError("Illegal image dimension");

	nrBytesPerPixel = nrBytes;
	m = rows;
	n = cols;
	data = (byte*)malloc(n*m*nrBytesPerPixel*sizeof(byte));
	if (data == NULL)
		throwError("Unable to allocate memory.");
	if (imageData!=NULL)
		memcpy(data,imageData,n*m*nrBytesPerPixel*sizeof(byte));
	else
		memset(data,0,n*m*nrBytesPerPixel*sizeof(byte));
}

/**
	This constructor makes a copy of the image that is passed in.
*/
Image::Image(Image& img){
	*this = img;
}

/**
	destructor - frees up all memory
*/
Image::~Image(){
	free(data);
}

/**
	gets the value of a given pixel - should be used for gray level images. If used for 24 bit images, this method will return the Red component
*/
byte Image::getPixelAt(int x, int y){
	int location = IMAGELOCATION(this,x,y,0);
	return GETIMAGEAT(this,location);
}

/**
	these four methods return RGBA values of a pixel - if the image is gray level (8-bit), these methods will do the same thing as getPixelAt.
	if the image is not an RGBA image, the getAPixel will return the same thing as the getPixelAt method.
*/
byte Image::getRPixelAt(int x, int y){
	int location = IMAGELOCATION(this,x,y,((nrBytesPerPixel==1)?(0):(0)));
	return GETIMAGEAT(this,location);
}

byte Image::getGPixelAt(int x, int y){
	int location = IMAGELOCATION(this,x,y,((nrBytesPerPixel==1)?(0):(1)));
	return GETIMAGEAT(this,location);
}

byte Image::getBPixelAt(int x, int y){
	int location = IMAGELOCATION(this,x,y,((nrBytesPerPixel==1)?(0):(2)));
	return GETIMAGEAT(this,location);
}



/**
	sets the value of a given pixel - should be used for gray level images
*/
void Image::setPixelAt(int x, int y, byte newValue){
	int location = IMAGELOCATION(this,x,y,0);
	SETIMAGEAT(this,location,newValue);
}

/**
	sets the RGBA value of a given pixel - if used for a gray level image, it does the same thing that setPixelAt does.
	if the image is not an RGBA image, the setAPixel will do not do anything.
*/
void Image::setRPixelAt(int x, int y, byte newValue){
	int location = IMAGELOCATION(this,x,y,((nrBytesPerPixel==1)?(0):(0)));
	SETIMAGEAT(this,location,newValue);
}

void Image::setGPixelAt(int x, int y, byte newValue){
	int location = IMAGELOCATION(this,x,y,((nrBytesPerPixel==1)?(0):(1)));
	SETIMAGEAT(this,location,newValue);
}

void Image::setBPixelAt(int x, int y, byte newValue){
	int location = IMAGELOCATION(this,x,y,((nrBytesPerPixel==1)?(0):(2)));
	SETIMAGEAT(this,location,newValue);
}

/**
	converts this image to 8 bit, grayscale image
*/
void Image::convertToGrayScale(){
	byte newValue;
	if (this->nrBytesPerPixel!=3 && this->nrBytesPerPixel!=4)
		return;
	for (int i=0;i<m;i++)
		for(int j=0;j<n;j++){
			//average out the RGB values
			newValue = (GETIMAGEAT(this,nrBytesPerPixel*(i*n+j) + 0) + GETIMAGEAT(this,nrBytesPerPixel*(i*n+j) + 1) + GETIMAGEAT(this,nrBytesPerPixel*(i*n+j) + 2))/3;
			SETIMAGEAT(this,i*n+j,newValue);
		}
	this->nrBytesPerPixel = 1;
    this->data = (byte*)realloc(data,n*m*nrBytesPerPixel*sizeof(byte));
}

/**
	This method is used to create an exact copy of the current image.
*/
Image* Image::duplicate(){
	return new Image(*this);
}






