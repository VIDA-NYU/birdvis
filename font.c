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

#include "font.h"

///////////////////////////////////////////////////////////////////////
// Font
///////////////////////////////////////////////////////////////////////

Font *Font_load(const char *description_file, const char *texture_file)
{
    
    FILE *f_descr = fopen(description_file,"rt");

    if(f_descr == NULL){	
	printf("FILE %s IS NULL\n", description_file);
    }

    char str[500];
    int header = 1;
    int i;

    Font *font = (Font *) malloc(sizeof(Font));

    while (fgets(str, 500,f_descr) != NULL)
    {
	if (str[0] == '#') 
	    continue;

	if (header)
	{
	    sscanf(str,"%d %d %d %d %d %d %d",
		   &font->pt_size, 
		   &font->dpi, 
		   &font->max_code,
		   &font->width, 
		   &font->height, 
		   &font->min_px_y,
		   &font->max_px_y,
		   &font->margin);
	    font->table = (Character **) 
		calloc(sizeof(Character*) * font->max_code+1,1);
	    /* sprintf(buffer,"#pt_size dpi max_code width height max_px_y min_px_y\n"); */
	    header = 0;
 	}

	else // record
	{
	    Character *c = (Character *) calloc(sizeof(Character),1);
	    sscanf(str,"%d %d %d %d %d %d %d %d",
		   &c->code, 
		   &c->xoff, 
		   &c->yoff,
		   &c->width, 
		   &c->height, 
		   &c->left,
		   &c->top,
		   &c->advance);
	    /* sprintf(buffer,"#code xoff yoff glyph_width glyph_height left top advance\n"); */
	    font->table[c->code] = c;
	}
    }

    // initialize pointers on characters
    Character *previous = NULL;
    int num_characters = 0;
    for (i=0;i<=font->max_code;i++)
    {
	Character *c = font->table[i];
	if (c == NULL)
	    continue;

	c->next = NULL;
	if (previous!=NULL)
	    previous->next = c;
	else
	    font->first = c;
	previous = c;
	num_characters++;
    }
    font->num_characters = num_characters;

    // the associated font texture
    font->texture = Image_read_sgi(texture_file);
    
    //
    return font;

}

void Font_print_description(Font *f)
{
    Character *c = f->first;
    while (c != NULL)
    {
	printf("Character   %c\n",(char) c->code);
	printf("   code:            %4d\n",c->code);
	printf("   xoff:            %4d\n",c->xoff);
	printf("   yoff:            %4d\n",c->yoff);
	printf("   width:           %4d\n",c->width);
	printf("   height:          %4d\n",c->height);
	printf("   left:            %4d\n",c->left);
	printf("   top:             %4d\n",c->top);
	printf("   advance:         %4d\n",c->advance);
	c = c->next;
    }
    printf("Font\n");
    printf("   pt_size:         %4d\n",f->pt_size);
    printf("   dpi:             %4d\n",f->dpi);
    printf("   width:           %4d\n",f->width);
    printf("   height:          %4d\n",f->height);
    printf("   min_px_y:        %4d\n",f->min_px_y);
    printf("   max_px_y:        %4d\n",f->max_px_y);
    printf("   max_code:        %4d\n",f->max_code);
    printf("   num_characters:  %4d\n",f->num_characters);
    printf("   margin:          %4d\n",f->num_characters);
    
}

FontTextIterator *Font_text_iterator(Font *font, 
				     const char *text, 
				     float size,
				     float x,
				     float y)
{
    FontTextIterator *fti = 
	(FontTextIterator *) malloc(sizeof(FontTextIterator));
    fti->text = text;
    fti->size = size;
    fti->font = font;
    fti->x = x;
    fti->y = y;
    fti->index = 0;
    fti->text_length = strlen(text);

    fti->base_x = x;
    fti->base_y = y;

    //
    // measure units:
    //    font->size   is in  "pt"
    //    font->dpi    is in  "dots"/"inch"
    //    text->size   is in  "u"
    //    texture      is in  "dots" 
    //
    // unit conversions:
    //    72 "pt"                                 / 1 "inch"
    //    font->dpi * (font->size / 72.0) "dots"  / 1 "pt"
    //    font->size / text->size "pt"            / 1 "u"

    // to convert into user units "u"
    // we multiply "dot" texture coordinates
    // by "dot2u".
    float font_dpi      = (float) fti->font->dpi;     // "dot/inch"
    float font_size_pt  = (float) fti->font->pt_size; // "pt"
    float text_size_u   = (float) fti->size;          // "u"

    float pt2inch = 1.0/72.0;                 // "inch/pt"
    float pt2dot  = font_dpi * pt2inch;       // "dot/pt"
    float u2pt    = font_size_pt/text_size_u; // "pt/u"
    float u2dot   = u2pt * pt2dot;            // "u/dot"
    float dot2u   = 1.0 / u2dot;

    fti->dot2u      = dot2u;   // "u/dot"

    return fti;
}

void FontTextIterator_reset(FontTextIterator *fti)
{
    fti->index = 0;
    fti->x = fti->base_x;
    fti->y = fti->base_y;
}

void FontTextIterator_free(FontTextIterator *fti)
{
    free(fti);
}

void FontTextIterator_set_text_position(FontTextIterator *fti, float x, float y)
{
    fti->base_x = x;
    fti->base_y = y;
    FontTextIterator_reset(fti);
}

// return bottom left corner based
void FontTextIterator_bb(FontTextIterator *fti,
			 float *bb_x,
			 float *bb_y,
			 float *bb_w,
			 float *bb_h)
{
    FontTextIterator_reset(fti);

    // write something
    float x_u,y_u,w_u,h_u;
    float x_tx, y_tx, w_tx, h_tx;
    
    float bb[4] = {fti->base_x,fti->base_y,fti->base_x,fti->base_y};

    while (FontTextIterator_next(fti, &x_u,&y_u,&w_u,&h_u,
				 &x_tx, &y_tx, &w_tx, &h_tx))
    {
	y_u = y_u - h_u;

	if (bb[0] > x_u)
	    bb[0] = x_u;

	if (bb[1] > y_u)
	    bb[1] = y_u;

	if (bb[2] < x_u + w_u)
	    bb[2] = x_u + w_u;

	if (bb[3] < y_u + h_u)
	    bb[3] = y_u + h_u;
    }


    *bb_y = bb[1];
    *bb_h = bb[3]  - bb[1];

    *bb_x = bb[0];
    *bb_w = bb[2] - bb[0];

    /* *bb_y = bb[1]; */
    /* *bb_h = bb[3] - bb[1]; */

    FontTextIterator_reset(fti);
}

///////////////////////////////////////////////////////////////////////
// FontTextIterator_next
///////////////////////////////////////////////////////////////////////

int FontTextIterator_next(FontTextIterator *fti,
			  float *x_u, // left
			  float *y_u, // top
			  float *w_u, // width
			  float *h_u, // height
			  float *x_tx,
			  float *y_tx,
			  float *w_tx,
			  float *h_tx)
{
    // did it finish?
    if (fti->index == fti->text_length)
	return 0;

    int margin = fti->font->margin;

    int code = (int)fti->text[fti->index];
    if (code > fti->font->max_code || code < 0)
    {
	code = 0x20;
    }

    Character *c = fti->font->table[code];
    
    /* printf("character before = %p\n",c); */

    // grab character metrics
    if (c == NULL)
    {
	code = 0x20;
	c = fti->font->table[code];
    }

    /* printf("character after = %p\n",c); */

    /* int code; */
    /* int xoff; */
    /* int yoff; */
    /* int width; */
    /* int height; */
    /* int left; */
    /* int top; */
    /* int advance; */
    /* struct _Character *next; */


    /* float ll = c->left - margin; */
    /* float tt = c->top  + margin; */
    /* float xx = c->xoff - margin; */
    /* float yy = c->yoff - margin; */
    /* float ww = c->width  + 2 * margin; */
    /* float hh = c->height + 2 * margin; */

    float ll = c->left;
    float tt = c->top ;
    float xx = c->xoff;
    float yy = c->yoff;
    float ww = c->width  ;
    float hh = c->height ;


    // x_u, y_u, w_u, h_u are the top-left coordinates and
    // the width and height sizes (all in the "u" model 
    // coordiante system)
    float dot2u = fti->dot2u;
    *x_u = fti->x + ll * dot2u; 
    *y_u = fti->y + tt * dot2u; 
    *w_u =          ww * dot2u;
    *h_u =          hh * dot2u;
    
    // texture coordinates

    float tw = (float) fti->font->texture->width;
    float th = (float) fti->font->texture->height;

    *x_tx = xx/tw;
    *y_tx = 1.0 - yy/th;
    *w_tx = ww/tw;
    *h_tx = hh/th; //c->height;

    // advance
    fti->x += c->advance * dot2u;
    fti->index++;

    return 1;

}
