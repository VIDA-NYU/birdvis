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

#ifndef FONT_H
#define FONT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "image.h"

///////////////////////////////////////////////////////////////////////
// Character
///////////////////////////////////////////////////////////////////////

typedef struct _Character 
{
    int code;
    int xoff;
    int yoff;
    int width;
    int height;
    int left;
    int top;
    int advance;
    struct _Character *next;
} Character;

///////////////////////////////////////////////////////////////////////
// Font
///////////////////////////////////////////////////////////////////////

typedef struct _Font 
{
    Character **table;
    Character *first;
    Image     *texture;
    int       pt_size;
    int       dpi;
    int       min_px_y;
    int       max_px_y;
    int       width;
    int       height;
    int       max_code;
    int       num_characters;
    int       margin;
} Font;

/*!
 * Font_load
 */
Font *Font_load(const char *description_file, const char *texture_file);

///////////////////////////////////////////////////////////////////////
// FontTextIterator
///////////////////////////////////////////////////////////////////////

typedef struct _FontTextIterator {
    Font  *font;
    float size;

    const char  *text;
    int    index;
    int    text_length;

    float x;
    float y;

    float base_x;
    float base_y;

    float dot2u;

} FontTextIterator;


FontTextIterator *Font_text_iterator(Font *font, 
				     const char *text, 
				     float size,
				     float x,
				     float y);

int FontTextIterator_next(FontTextIterator *fti,
			  float *x_u, // left
			  float *y_u, // top
			  float *w_u, // width
			  float *h_u, // height
			  float *x_tx,
			  float *y_tx,
			  float *w_tx,
			  float *h_tx);

void FontTextIterator_bb(FontTextIterator *fti, 
			 float *bb_x,
			 float *bb_y,
			 float *bb_w,
			 float *bb_h);

void FontTextIterator_reset(FontTextIterator *fti);
void FontTextIterator_free(FontTextIterator *fti);
void FontTextIterator_set_text_position(FontTextIterator *fti, float x, float y);



/* FontTextIterator *Font_text_iterator(Font *font,  */
/* 				 char *text,  */
/* 				 float size, */
/* 				 float x, */
/* 				     float y); */

/* int FontTextIterator_next(FontTextIterator *fti, */
/* 			  float *x_u, // left */
/* 			  float *y_u, // top */
/* 			  float *w_u, // width */
/* 			  float *h_u, // height */
/* 			  int    *x_tx, */
/* 			  int    *y_tx, */
/* 			  int    *w_tx, */
/* 			  int    *h_tx); */

#ifdef __cplusplus
}
#endif

#endif
