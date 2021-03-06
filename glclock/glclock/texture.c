//
//  texture.c
//  glclock
//
//  Created by Peter Romov on 11/6/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//
//  For the source of LoadTextureRAW thanks to 
//  http://nullterminator.net

#include <stdio.h>
#include <stdlib.h>

#include "glut.h"
#include "texture.h"

// load a 256x256 RGB .RAW file as a texture
GLuint LoadTextureRAW( const char * filename, int wrap )
{
    GLuint texture;
    int width, height;
    char * data;
    FILE * file;
    
    // open texture data
    file = fopen( filename, "rb" );
    if ( file == NULL ) return 0;
    
    // allocate buffer
    width = 256;
    height = 256;
    data = malloc( width * height * 3 );
    
    // read texture data
    fread( data, width * height * 3, 1, file );
    fclose( file );
    
    // allocate a texture name
    glGenTextures( 1, &texture );
    
    // select our current texture
    glBindTexture( GL_TEXTURE_2D, texture );
    
    // select modulate to mix texture with color for shading
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    
    // when texture area is small, bilinear filter the closest mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR_MIPMAP_NEAREST );
    // when texture area is large, bilinear filter the first mipmap
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    
    // if wrap is true, the texture wraps over at the edges (repeat)
    //       ... false, the texture ends at the edges (clamp)
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,
                    wrap ? GL_REPEAT : GL_CLAMP );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,
                    wrap ? GL_REPEAT : GL_CLAMP );
    
    // build our texture mipmaps
    gluBuild2DMipmaps( GL_TEXTURE_2D, 3, width, height,
                      GL_RGB, GL_UNSIGNED_BYTE, data );
    
    // free buffer
    free( data );
    
    return texture;
}