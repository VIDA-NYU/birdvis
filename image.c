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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <string.h>

#include "image.h"
#include "readrgb.h"

////////////////////////////////////////////////////////////////////////////////
// util
////////////////////////////////////////////////////////////////////////////////

void util_invert_buffer(byte *buffer, int n)
{
    int i;
    for (i=0;i<n/2;i++)
    {
	byte aux = buffer[i];
	buffer[i] = buffer[n-i-1];
	buffer[n-i-1] = aux;
    }
}

void util_invert_and_dump(byte *buffer, int n, FILE *output)
{
    util_invert_buffer(buffer,n);
    fwrite(buffer,1,n,output);   
}

////////////////////////////////////////////////////////////////////////////////
// Image
////////////////////////////////////////////////////////////////////////////////

Image *Image_new(int width, int height)
{
    Image *img  = (Image *) malloc(sizeof(Image));
    img->buffer  = (byte *) malloc(sizeof(byte) * width * height);
    img->width  = width;
    img->height = height;
    int i;
    for (i=0;i<width*height;i++)
	img->buffer[i] = 0;
    return img;
}

void Image_free(Image *img, char self)
{
    free(img->buffer);
    if (self)
	free(img);
}

byte Image_get_value(Image *img, int row, int col)
{
    return img->buffer[row * img->width + col];
}

void Image_set_value(Image *img, int row, int col, byte value)
{
    img->buffer[row * img->width + col] = value;
}    

void Image_copy(Image *source, Image *target, int xoff, int yoff)
{
    int i,j;
    for (i=0;i<source->height;i++)
    {
	for (j=0;j<source->width;j++)
	{
	    int ii = i + yoff;
	    int jj = j + xoff;
	    Image_set_value(target,ii,jj,Image_get_value(source,i,j));
	}
    }
}

void Image_save_as_sgi(Image *g, const char *filename, byte invert_y)
{
    int  i, j;

    printf("Image_save_as_sgi %p %s %d\n",g,filename,invert_y);
    FILE *f = fopen(filename,"wb");

    // sgi header
    byte buffer[512];
    for (i=0;i<512;i++)
	buffer[i] = 0;

    unsigned int *i_ptr = (unsigned int*) buffer;
    int count;

    int num_bytes[] = {
	2,   // magic number 474
	1,   // storage format: 0 uncompressed 1 compressed
	1,   // bytes per pixel channel: 0,1
	2,   // dimension 1,2,3
	2,   // XSIZE
	2,   // YSIZE
        2,   // num channels
        4,   // PIXMIN value
        4,   // PIXMAX value
        4,   // DUMMY
	80,  // image name (has to be null terminated)
        4,   // colormap (0=normal mode)
        404, // DUMMY
    };

    int index=0;
    byte invert=1;

    *i_ptr = 474; // sgi code
    util_invert_and_dump(buffer,num_bytes[index++],f);

    *i_ptr = 0; // uncompressed
    util_invert_and_dump(buffer,num_bytes[index++],f);

    *i_ptr = 1; // one byte per channel
    util_invert_and_dump(buffer,num_bytes[index++],f);

    *i_ptr = 2; // two dimensional image
    util_invert_and_dump(buffer,num_bytes[index++],f);
    
    *i_ptr = g->width; // width
    util_invert_and_dump(buffer,num_bytes[index++],f);
    
    *i_ptr = g->height; // height
    util_invert_and_dump(buffer,num_bytes[index++],f);

    *i_ptr = 1; // number of channels
    util_invert_and_dump(buffer,num_bytes[index++],f);

    *i_ptr = 0; // min pixel value
    util_invert_and_dump(buffer,num_bytes[index++],f);
    
    *i_ptr = 0xFF; // max pixel value
    util_invert_and_dump(buffer,num_bytes[index++],f);

    *i_ptr = 0; // dummy
    util_invert_and_dump(buffer,num_bytes[index++],f);

    *i_ptr = 0; // image name
    sprintf((char *)buffer,"image.sgi");
    fwrite(buffer,1,num_bytes[index++],f);

    *i_ptr = 0; // color map id
    util_invert_and_dump(buffer,num_bytes[index++],f);

    *i_ptr = 0; // empty space to complete 512 bytes
    util_invert_and_dump(buffer,num_bytes[index++],f);

     // 2 + 1 + 1 + 2 + 2 + 2 + 2 + 4 + 4 + 4 + 80 + 404 

    // write the buffer bytes
    for ( i = 0; i < g->height; i++ )
    {
	for ( j = 0; j < g->width; j++ ) 
	{
	    // revert the height
	    if (invert_y)
		*i_ptr = Image_get_value(g,g->height-1-i,j);
	    else
		*i_ptr = Image_get_value(g,i,j);
	    // printf("i_ptr = %d\n",*i_ptr);
	    fwrite(buffer,1,1,f);
	}
    }

    fclose(f);
}

Image *Image_read_sgi(const char *filename)
{
    int width;
    int height;
    int components;

    byte *rgba = (byte *) read_texture(filename, 
				       &width, 
				       &height, 
				       &components);
    
    //
    int i,j;
    int offset = 1;
    int n = width * height;
    for (i=1;i<n;i++)
    {
	rgba[offset++] = rgba[4*i];
    }
    rgba = (byte*) realloc(rgba,n);

    //
    Image *img = Image_new(width,height);
    memcpy(img->buffer,rgba,n);

    free(rgba);

    return img;
}
