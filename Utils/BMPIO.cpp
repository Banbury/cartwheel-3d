#include "BMPIO.h"
#include <string.h>
#include <Utils/Utils.h>

/**
	general methods for reading a short from a file - format: little endian 
*/
inline unsigned short getShort(FILE* fp){
	if (fp == NULL)
		return 0;
	unsigned short result;
	if (fread(&result,sizeof(result),1,fp) != 1)
		throwError("Unsuccesful read.");
	return result;
}

/**
	general methods for reading an int from a file - format: little endian 
*/
inline unsigned int getInt(FILE* fp){
	if (fp == NULL)
		return 0;
	unsigned int result;
	if (fread(&result,sizeof(result),1,fp) != 1)
		throwError("Unsuccesful read.");
	return result;
}

/**
	general methods for reading a byte from a file - format: little endian - but this one doesn't matter
*/
inline unsigned char getByte(FILE* fp){
	if (fp == NULL)
		return 0;
	unsigned char result;
	if (fread(&result,sizeof(result),1,fp) != 1)
		throwError("Unsuccesful read.");
	return result;
}


/**
	This is the method that is responsible with the reading of the BMP data.
	fp - file pointer where the data is read from
	img - the image that we are now populating
	infoHeader - holds important information about the bitmap that we are reading.
*/
void BMPIO::readBMPData(FILE* fp, Image* img, BMPInfoHeader* infoHeader){
	int i, j;
	unsigned char temp;
	unsigned char* pallete = infoHeader->getPallete();

	//go through each pixel, and read the value
	for (i=0; i<img->getHeight();i++){
		for (j=0; j<img->getWidth();j++){
			//if we are dealing with a 24 bit bitmap, we need to read the R, G and B components of the image.
			if (infoHeader->getNrBits() == 24){
				fread(&temp,sizeof(temp),1,fp);
				img->setBPixelAt(i,j,(byte)temp);
				fread(&temp,sizeof(temp),1,fp);
				img->setGPixelAt(i,j,(byte)temp);
				fread(&temp,sizeof(temp),1,fp);
				img->setRPixelAt(i,j,(byte)temp);
			}
			//if it's not a 24 bit bitmap, we'll only deal with bitmaps that are 8-bit and using a color pallete
			else if (pallete != NULL){
				fread(&temp,sizeof(temp),1,fp);
				img->setRPixelAt(i,j,(byte)(pallete[4*temp+0]));
				img->setGPixelAt(i,j,(byte)(pallete[4*temp+1]));
				img->setBPixelAt(i,j,(byte)(pallete[4*temp+2]));
			}
			else
				throwError("Unable to process this bitmap...");
		}
		//and skip the padding - that formula is meant to read all the padding that fills the data up to the closest 32-bit value
		//I'm sure there must be an easier way to do that...
		for (j=0; j<(4-(3*img->getWidth())%4)%4;j++)
			fread(&temp,sizeof(temp),1,fp);
	}
}


/**
	This method is used to allocate an image for the bitmap that is to be read from a file.
	infoHeader - contains information regarding the size of the bitmap, etc
	nrBits - indicates how many bits the image will have - default value is 24, but 32 bit (RGBA) values can also be read.
*/
Image* BMPIO::allocateImage(BMPInfoHeader *infoHeader, int nrBits){
	return new Image(nrBits/8,
					infoHeader->getWidth(),
					infoHeader->getHeight(),
					NULL);
}


/**
	This method is used to load an image from the current BMP file.
*/
Image* BMPIO::loadFromFile(int imageModel){
	if (fileName == NULL)
		throwError("NULL file name.");
	FILE* fp = fopen(fileName, "rb");
		
	if (fp == NULL)
		throwError("Unable to read file %s.", fileName);

	BMPHeader *header = new BMPHeader(fp);
	BMPInfoHeader *infoHeader = new BMPInfoHeader(fp);

	//create an image with the info from the infoHeader of the given bitmap
	Image* newImage;
	//create an RGBA image if requested
	if (imageModel == RGBA_MODEL)
		newImage = allocateImage(infoHeader, 32);
	else
		newImage = allocateImage(infoHeader);

	//and populate the new image
	readBMPData(fp,newImage,infoHeader);
	fclose(fp);
	delete header;
	delete infoHeader;

	return newImage;
}


/**
	this method is used to write the image that is passed in. It will always write 24 bit bmp files.
*/
void BMPIO::writeBMPData(FILE* fp, Image* img){
	int i,j;
	byte temp[3];
	//now write the image data...
	for (i=0; i<img->getHeight();i++){
		for (j=0; j<img->getWidth();j++){
			if (img->getNrBytes() == 3 || img->getNrBytes() == 4){
				temp[0] = (byte) img->getBPixelAt(i,j);
				temp[1] = (byte) img->getGPixelAt(i,j);
				temp[2] = (byte) img->getRPixelAt(i,j); 
			}
			else
				temp[0] = temp[1] = temp [2] = (byte) img->getPixelAt(i,j);
			fwrite(temp,sizeof(temp[0]),3,fp);
		}
		//and introduce the padding
		temp[0] = 0;

		//write the padding - again the complicated expression ...
		for (j=0; j<(4-(3*img->getWidth())%4)%4;j++)
			fwrite(temp,sizeof(temp[0]),1,fp);
	}
}

/**
	This method will save an image to the given BMP file.
*/
void BMPIO::writeToFile(Image* img){
	if (fileName == NULL)
		throwError("NULL file name...");
	if (img == NULL)
		throwError("NULL image...");
	FILE* fp = fopen(fileName, "wb");
	if (fp == NULL)
		throwError("Unable to create file.");

	//create the bitmap's header and header info objects
	BMPHeader *header = new BMPHeader(img);
	BMPInfoHeader *infoHeader = new BMPInfoHeader(img);

	//write the bmp header and info header.
	header->writeHeader(fp);
	infoHeader->writeInfoHeader(fp);

	writeBMPData(fp,img);
	
	fclose(fp);
	
	delete header;
	delete infoHeader;
}


/* -----------------------------------------> BMP HEADER methods <-------------------------------------- */

/**
	populate the BMP header from a file.
*/
BMPHeader::BMPHeader(FILE* fp){
	if (fp == NULL)
		throwError("Invalid file pointer.");
	if (fread(&id,sizeof(id),1,fp)!=1)
		throwError("Unexpected End Of File (BMPHeader).");
	if (id != (('M'<<8)+'B'))
		throwError("Not a BMP file.");
	if (fread(&fileSize,sizeof(fileSize),1,fp)!=1)
		throwError("Unexpected End Of File (BMPHeader).");
	if (fread(&reserved1,sizeof(reserved1),1,fp)!=1)
		throwError("Unexpected End Of File (BMPHeader).");
	if (fread(&reserved2,sizeof(reserved2),1,fp)!=1)
		throwError("Unexpected End Of File (BMPHeader).");
	if (fread(&offset,sizeof(offset),1,fp)!=1)
		throwError("Unexpected End Of File (BMPHeader).");
	/* done populating the info header from a file */
}

/**
	populate the header from an existing image.
*/
BMPHeader::BMPHeader(Image* img){
	if (img == NULL)
		throwError("Invalid image.");
	// note the little endian here...
	id = ('M' << 8) + 'B';
	fileSize = HEADERSIZE + HEADERINFOSIZE + img->getNrBytes()*img->getWidth()*img->getHeight() + 
		((img->getNrBytes()*img->getWidth())%4)*img->getHeight();	//this is the padding
	reserved1 = reserved2 = 0;
	offset = HEADERSIZE + HEADERINFOSIZE;
}


/**
	This method writes the bitmap's header to a file.
*/
void BMPHeader::writeHeader(FILE* fp){
	if (fp == NULL)
		throwError("Invalid file pointer");
	fwrite(&id,sizeof(id),1,fp);
	fwrite(&fileSize,sizeof(fileSize),1,fp);
	fwrite(&reserved1,sizeof(reserved1),1,fp);
	fwrite(&reserved2,sizeof(reserved2),1,fp);
	fwrite(&offset,sizeof(offset),1,fp);
}


/* -----------------------------------------> BMP INFO HEADER methods <-------------------------------------- */


/**
	Read the info header from a file.
*/
BMPInfoHeader::BMPInfoHeader(FILE* fp){
	if (fp == NULL)
		throwError("Invalid file pointer.");
	if (
		fread(&size,sizeof(size),1,fp)!=1 					|| 
		fread(&width,sizeof(width),1,fp)!=1 				||
		fread(&height,sizeof(height),1,fp)!=1 				||
		fread(&nrPlanes,sizeof(nrPlanes),1,fp)!=1 			||
		fread(&nrBits,sizeof(nrBits),1,fp)!=1 				||
		fread(&compression,sizeof(compression),1,fp)!=1		||
		fread(&imageSize,sizeof(imageSize),1,fp)!=1			||
		fread(&xRes,sizeof(xRes),1,fp)!=1					||
		fread(&yRes,sizeof(yRes),1,fp)!=1					||
		fread(&nrColours,sizeof(nrColours),1,fp)!=1			||
		fread(&importantColours,sizeof(importantColours),1,fp)!=1	
	)
		throwError("Unexpected End Of File (BMPInfoHeader).");
	if (size != HEADERINFOSIZE)
		throwError("Invalid BMP file.");
	if (nrPlanes != 1)
		throwError( "Unsupported number of bit planes.");
	if (nrBits != 8 && nrBits != 24)
		throwError( "Unsupported number of bits.");
	if (compression != 0)
		throwError( "Only uncompressed bmp files are supported.");
		
	//support pallete information...
	pallete = NULL;
	if (nrBits == 8){
		pallete = (unsigned char*)malloc(4 * sizeof(unsigned char) * ((nrColours==0)?256:nrColours));
		fread(pallete,sizeof(unsigned char),4*((nrColours==0)?256:nrColours),fp);
	}
	/* done populating the info header from a file */
}

/**
	Build the info header from an existing image.
*/
BMPInfoHeader::BMPInfoHeader(Image* img){
	if (img == NULL)
		throwError( "Invalid image.");
	size = 40;
	width = img->getWidth();
	height = img->getHeight();
	nrPlanes = 1;
	//always create 24 bit images
	nrBits = 24;
	compression = 0;
	imageSize = img->getNrBytes()*img->getWidth()*img->getHeight() + 
		((img->getNrBytes()*img->getWidth())%4)*img->getHeight();	//this is the padding;	
	xRes = yRes = 0;
	nrColours = 0;	//we won't use pallete info
	importantColours = 0;
	pallete = NULL;		
}

/**
	destructor - make sure we free the pallete if one was used.
*/
BMPInfoHeader::~BMPInfoHeader(){
	if (pallete != NULL)
		free(pallete);
}


/**
	write the info header to a file.
*/
void BMPInfoHeader::writeInfoHeader(FILE* fp){
	if (fp == NULL)
		throwError( "Invalid file pointer");

	if (pallete != NULL || nrColours != 0)
		throwError( "Can only write 8 bit gray scale or 24 bit bitmaps");
	fwrite(&size,sizeof(size),1,fp);
	fwrite(&width,sizeof(width),1,fp);
	fwrite(&height,sizeof(height),1,fp);
	fwrite(&nrPlanes,sizeof(nrPlanes),1,fp);
	fwrite(&nrBits,sizeof(nrBits),1,fp);
	fwrite(&compression,sizeof(compression),1,fp);
	fwrite(&imageSize,sizeof(imageSize),1,fp);
	fwrite(&xRes,sizeof(xRes),1,fp);
	fwrite(&yRes,sizeof(yRes),1,fp);
	fwrite(&nrColours,sizeof(nrColours),1,fp);
	fwrite(&importantColours,sizeof(importantColours),1,fp);
}



