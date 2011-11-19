//
//  figures.h
//  glclock
//
//  Created by Peter Romov on 11/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#ifndef glclock_figures_h
#define glclock_figures_h

void drawClock(GLUquadricObj *quadObj, GLfloat shell_angle);
void initTextures(GLUquadricObj *quadObj);
void body(GLUquadricObj *quadObj);
void shell(GLUquadricObj *quadObj);
void face(GLUquadricObj *quadObj);
void arrows(GLUquadricObj *quadObj);
void arrow_hrs(GLUquadricObj *quadObj);
void arrow_min(GLUquadricObj *quadObj);
void arrow_sec(GLUquadricObj *quadObj);
void marks(GLUquadricObj *quadObj);
void glass(GLUquadricObj *quadObj);;

#endif
