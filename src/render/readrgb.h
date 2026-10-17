//%BEGINC_LIC
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
// Copyright 2010-2011 University of Utah.                                    //
// All rights reserved.                                                       //
//                                                                            //
// Redistribution and use in source and binary forms, with or without         //
// modification, are permitted provided that the following conditions are     //
// met:                                                                       //
//                                                                            //
//    1. Redistributions of source code must retain the above copyright       //
//       notice, this list of conditions and the following disclaimer.        //
//                                                                            //
//    2. Redistributions in binary form must reproduce the above copyright    //
//       notice, this list of conditions and the following disclaimer in the  //
//       documentation and/or other materials provided with the distribution. //
//                                                                            //
// THIS SOFTWARE IS PROVIDED BY UNIVERSITY OF UTAH ``AS IS'' AND ANY          //
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE          //
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR         //
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL UNIVERSITY OF UTAH OR            //
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,      //
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,        //
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR         //
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF     //
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING       //
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         //
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               //
//                                                                            //
// The views and conclusions contained in the software and documentation      //
// are those of the authors and should not be interpreted as representing     //
// official policies, either expressed or implied, of University of Utah.     //  
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
//%END_LIC

/*
 * read_texture() - read in an image file in SGI 'libimage' format
 * 	currently its very simple minded and converts all images
 *      to RGBA8 regardless of the input format and returns the
 *	original number of components in the appropriate parameter.
 *     
 *	the components are converted as follows
 *		L    -> LLL 1.0
 *		LA   -> LLL A
 *		RGB  -> RGB 1.0
 *		RGBA -> RGB A
 */
unsigned *read_texture(const char *name,
		       int *width,int *height,int *components);
