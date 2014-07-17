/////////////////////////////////////////////////////////////////////////////////////
//                                                                                 //
// Copyright 2011-2013 NYU Polytechnic School of Engineering.                      //
// Copyright 2010-2011 University of Utah.                                         //
//                                                                                 //
// Permission is hereby granted, free of charge, to any person obtaining a copy    //
// of this software and associated documentation files (the "Software"), to dea    //
// in the Software without restriction, including without limitation the rights    //
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell       //
// copies of the Software, and to permit persons to whom the Software is           //
// furnished to do so, subject to the following conditions:		           //
// 										   //
// The above copyright notice and this permission notice shall be included in	   //
// all copies or substantial portions of the Software.			           //
// 										   //
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR	   //
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,	   //
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE     //
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER          // 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,   //
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN       //
// THE SOFTWARE.                                                                   //
//                                                                                 // 
/////////////////////////////////////////////////////////////////////////////////////

#include "Texture.hpp"
#include "FreeImage.h"

#include<sstream>

using namespace std;

/***********
 * Texture *
 ***********/

Texture::Texture() :
    width(0),
    height(0),
    pitch(0),
    bitsPerPixel(0),
    bytesPerPixel(0),
    loaded(false)
{}

// Texture::Texture(GLuint texture_id, unsigned w, unsigned h, unsigned bpp)
// {
//     this->texture_id = texture_id;
//     width = w;
//     height = h;
//     bitsPerPixel = bpp;
//     bytesPerPixel = (bitsPerPixel-1)/8 + 1;
//     pitch = w * bytesPerPixel;    
//     loaded = false;
// }

// Texture::Texture(string filename)
// {
//     	//image format
// 	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
// 	//pointer to the image, once loaded
// 	FIBITMAP *dib(0);
// 	//pointer to the image data
// 	BYTE* bits(0);
// 	//image width and height
// 	unsigned int width(0), height(0);
// 	//OpenGL's image ID to map to
// 	//GLuint textureID;
	
// 	//check the file signature and deduce its format
// 	fif = FreeImage_GetFileType(filename, 0);
// 	//if still unknown, try to guess the file format from the file extension
// 	if(fif == FIF_UNKNOWN) 
// 	    fif = FreeImage_GetFIFFromFilename(filename);
// 	//if still unkown, return failure
// 	if(fif == FIF_UNKNOWN)
// 	    exit(1);
	
// 	//check that the plugin has reading capabilities and load the file
// 	if(FreeImage_FIFSupportsReading(fif))
// 	    dib = FreeImage_Load(fif, filename);
// 	//if the image failed to load, return failure
// 	if(!dib)
// 	    exit(1);
	
// 	//retrieve the image data
// 	bits = FreeImage_GetBits(dib);

// 	//cout << "   Bits " << bits << endl;
	
// 	//get the image width and height
// 	width = FreeImage_GetWidth(dib);
// 	cout << "   Width " << width << endl;
	
// 	height = FreeImage_GetHeight(dib);
// 	cout << "   Height " << height << endl;
// 	//if this somehow one of these failed (they shouldn't), return failure
// 	if((bits == 0) || (width == 0) || (height == 0))
// 	    exit(1);

// 	//generate an OpenGL texture ID for this texture
// 	//glGenTextures(1, &textureId);

// 	//glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

// 	//bind to the new texture ID
// 	glBindTexture(GL_TEXTURE_2D, texID);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, 
// 			GL_NEAREST);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, 
// 			GL_NEAREST);

// 	//store the texture data for OpenGL use
// 	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height,
// 		     0, GL_BGR, GL_UNSIGNED_BYTE, bits);

// 	//Free FreeImage's copy of the data
// 	FreeImage_Unload(dib);




//     this->loaded = true;

// }

Texture::~Texture(){
    this->release();
}

GLuint
Texture::getId(){
    return this->texture_id;
}

unsigned 
Texture::getWidth(){
    return this->width;
}

unsigned 
Texture::getHeight(){
    return this->height;
}

unsigned 
Texture::getBitsPerPixel(){
    return this->bitsPerPixel;
}

unsigned 
Texture::getBytesPerPixel(){
    return this->bytesPerPixel;
}

bool     
Texture::isLoaded(){
    return this->loaded;
}

void     
Texture::release(){
    // cout << "Texture::release()" << endl;
    if(this->isLoaded()){
	// cout << "   releasing texture id: " << this->texture_id << endl;
	glDeleteTextures(1, &(this->texture_id));    
	this->loaded = false;
    }
}

void 
Texture::bind(){
    glBindTexture(GL_TEXTURE_2D, this->texture_id);
}

void     
Texture::load(string filename)
{
    
    //image format
    FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
    //pointer to the image, once loaded
    FIBITMAP *dib(0);
    //pointer to the image data
    BYTE* bits(0);
    //image width and height
    // unsigned int width(0), height(0);
    //OpenGL's image ID to map to
    //GLuint textureID;
    
    //check the file signature and deduce its format
    fif = FreeImage_GetFileType(filename.c_str(), 0);
    //if still unknown, try to guess the file format from the file extension
    if(fif == FIF_UNKNOWN) 
	fif = FreeImage_GetFIFFromFilename(filename.c_str());
    //if still unkown, return failure
    if(fif == FIF_UNKNOWN){
	this->texture_id = 0;//exit(1);
	return;
    }
    
    //check that the plugin has reading capabilities and load the file
    if(FreeImage_FIFSupportsReading(fif))
	dib = FreeImage_Load(fif, filename.c_str());
    //if the image failed to load, return failure
    if(!dib){
	this->texture_id = 0;//exit(1);
	return;
    }

    // convert to RGB: 24 bits
    dib = FreeImage_ConvertTo24Bits(dib);

    //
    FREE_IMAGE_TYPE image_type = FreeImage_GetImageType(dib);

    //retrieve the image data
    bits          = FreeImage_GetBits(dib);
    width         = FreeImage_GetWidth(dib);
    height        = FreeImage_GetHeight(dib);
    pitch         = FreeImage_GetPitch(dib);
    bitsPerPixel  = FreeImage_GetBPP(dib);
    bytesPerPixel = (bitsPerPixel-1)/8 + 1;    

    // cout << "   Width "          << width << endl;
    // cout << "   Height "         << height << endl;
    // cout << "   Pitch "          << pitch << endl;
    // cout << "   BitsPerPixel "   << bitsPerPixel << endl;
    // cout << "   BytesPerPixel "  << bytesPerPixel << endl;
    // cout << "   image_type "     << image_type << endl;

    //if this somehow one of these failed (they shouldn't), return failure
    if((bits == 0) || (width == 0) || (height == 0)){
	cout << "   (WARNING) loading problem: bits == 0 or width == 0 or height == 0" << endl;
	this->texture_id = 0;//exit(1);
	return;
    }

    // this is critical
    BYTE* new_bits(0);
    int index = 0;
    new_bits = new BYTE[width * height * bytesPerPixel]; //(BYTE*) malloc(width * height * bytesPerPixel);
    BYTE* previous(0);
    for(int i = 0; i < height; i++) {
	for(int j = 0; j < width * bytesPerPixel; j+=bytesPerPixel) {
	    new_bits[index++] = bits[j+2]; // freeimage stores BGR instead of RGB
	    new_bits[index++] = bits[j+1];
	    new_bits[index++] = bits[j+0];
	}
	// next line
	bits += pitch;
    }


    // for(int i = 0; i < height; i++) {
    // 	// FIRGBF *bits = (FIRGBF *)FreeImage_GetScanLine(dib, i);
    //     BYTE *bits = (BYTE *)FreeImage_GetScanLine(dib, i);

    // 	if (previous != 0)
    // 	{
    // 	    int diff = ((unsigned long) bits) - ((unsigned long) previous);
    // 	    printf("   ---> diff = %d   pitch = %d \n",diff, pitch);
    // 	}
    // 	previous = bits;

    //     for(int j = 0; j < width; j++) {
    // 	    for (int k=0;k<bytesPerPixel;k++)
    // 		new_bits[index++] = bits[bytesPerPixel * j + k];
	    
    //       // new_bits[index++] = bits[j].red;
    //       // new_bits[index++] = bits[j].green;
    //       // new_bits[index++] = bits[j].blue;
    //     }
    // }

    // int bit_index = 0;
    // for (int i=0;i<height;i++)
    // {
    // 	printf(" ");
    // 	for (int j=0;j<pitch;j++)
    // 	{
    // 	    printf("%02x ",bits[bit_index++]);
    // 	    if (j % 3 == 2)
    // 		printf("| ");
    // 	}
    // 	cout << endl;
    // }

    // int bit_index = 0;
    // for (int i=0;i<height;i++)
    // {
    // 	printf(" ");
    // 	for (int j=0;j<width * bytesPerPixel;j++)
    // 	{
    // 	    printf("%02x ",new_bits[bit_index++]);
    // 	    if (j % 3 == 2)
    // 		printf("| ");
    // 	}
    // 	cout << endl;
    // }

	
    // // test pixel access avoiding scanline calculations 
    // // to speed-up the image processing
    // if(image_type == FIT_RGBF) {
    // 	BYTE *bits = (BYTE*)FreeImage_GetBits(dib);
    // 	for(y = 0; y < height; y++) {
    // 	    FIRGBF *pixel = (FIRGBF*)bits;
    // 	    for(x = 0; x < width; x++) {
    // 		pixel[x].red = 128;
    // 		pixel[x].green = 128;
    // 		pixel[x].blue = 128;
    // 	    }
    // 	    // next line
    // 	    bits += pitch;
    // 	}
    // }
    // else if((image_type == FIT_BITMAP) && (FreeImage_GetBPP(dib) == 24)) {
    // 	BYTE *bits = (BYTE*)FreeImage_GetBits(dib);
    // 	for(y = 0; y < height; y++) {
    // 	    BYTE *pixel = (BYTE*)bits;
    // 	    for(x = 0; x < width; x++) {
    // 		pixel[FI_RGBA_RED]   = 128;
    // 		pixel[FI_RGBA_GREEN] = 128;
    // 		pixel[FI_RGBA_BLUE]  = 128;
    // 		pixel += 3;
    // 	    }
    // 	    // next line
    // 	    bits += pitch;
    // 	}
    // }
    
    //generate an OpenGL texture ID for this texture
    glGenTextures(1, &this->texture_id);                               // opengl

    //bind to the new texture ID
    glBindTexture(GL_TEXTURE_2D, texture_id);                          // opengl
    // glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE );    // opengl
    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);     // opengl
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_CLAMP_TO_EDGE);   // opengl
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_CLAMP_TO_EDGE);   // opengl
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);  // opengl
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);  // opengl

    // this is key to align the thing correctly
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // cout << "   Allocate GL texture: " << texture_id << endl;
	    
    GLint internalFormat, image_format;

    // switch (bitsPerPixel) {
    // case 8:
    // 	internalFormat = GL_INTENSITY;
    // 	image_format   = GL_LUMINANCE;
    // 	break;
    // case 24:

    internalFormat = GL_RGB;
    image_format   = GL_RGB;
    // image_format   = GL_RGB;

    // break;
    // case 32:
    // 	internalFormat = GL_RGBA;
    // 	image_format   = GL_BGRA;
    // 	break;
    // default:
    // 	abort();
    // }

    // BYTE* new_bits(0);
    // new_bits = new BYTE[width * height * bytesPerPixel]; //(BYTE*) malloc(width * height * bytesPerPixel);
    // int index = 0;
    // for (int i=0;i<height;i++)
    // {
    // 	for (int j=0;j<width;j++)
    // 	{
    // 	    RGBQUAD rgbquad;
    // 	    FreeImage_GetPixelColor(dib, j, i, &rgbquad);
    // 	    BYTE color[4] = {rgbquad.rgbRed,rgbquad.rgbGreen,rgbquad.rgbBlue,rgbquad.rgbReserved};
    // 	    for (int k=0;k<bytesPerPixel;k++)
    // 	    {
    // 		new_bits[index++] = color[k];
    // 	    }
    // 	}
    // }

    // if (pitch != bytesPerPixel * width) //  && texture_id != 1)
    // {
    // 	cout << "   Adjusting bits for " << filename << "..." << endl;

    // 	// not sure if can write on the bits vector but 
    // 	// might not be a problem
	
    // 	//pointer to the image data
    // 	for (int i=0;i<height;i++)
    // 	{
    // 	    for (int j=0;j<width;j++)
    // 	    {
    // 		for (int k=0;k<bytesPerPixel;k++)
    // 		{
    // 		    int write_index = i * (bytesPerPixel * width) + bytesPerPixel * j + k;
    // 		    int read_index  = i * pitch + bytesPerPixel * j + k;
    // 		    // cout << "   bits[" << write_index << "] = bits["<< read_index << "]" << endl;
    // 		    new_bits[write_index] = bits[read_index];
    // 		}
    // 	    }
    // 	}
    // } // handle case where pitch is not aligned

    // if (new_bits != 0)
    // {

    // glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height,
    // 		 0, image_format, GL_UNSIGNED_BYTE, new_bits);       // opengl
    // 	delete [] new_bits;
    // }
    // else
    // {
    glTexImage2D(GL_TEXTURE_2D, 
		 0, 
		 internalFormat, 
		 width, 
		 height,
		 0, 
		 image_format, 
		 GL_UNSIGNED_BYTE, 
		 new_bits);           // opengl

    delete [] new_bits;
    // }

    FreeImage_Unload(dib);

    this->loaded = true;
    //cout << this->str();
}

string Texture::str(){
    stringstream ss;

    if(!this->isLoaded())
	ss<< "Texture not loaded" << endl;
    else{
 	ss << "Texture id = " << this->texture_id << endl;
	ss << "Width = " << this->width << endl;
	ss << "Height = " << this->height << endl;
	ss << "bitsPerPixel = " << this->bitsPerPixel << endl;
	ss << "bytesPerPixel = " << this->bytesPerPixel << endl;
    }

    return ss.str();
}

