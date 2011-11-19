//
//  figures.c
//  glclock
//
//  Created by Peter Romov on 11/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <time.h>

#include "glut.h"
#include "texture.h"
#include "figures.h"

GLuint texWood;

GLdouble rTop = 1.0;
GLdouble rBottom = 1.5;
GLdouble hBody = 0.8;
GLdouble hFace = 0.35;

void initTextures(GLUquadricObj *quadObj)
{
    texWood = LoadTextureRAW("wood4.raw", GL_TRUE);
    gluQuadricTexture(quadObj, GL_TRUE);
}

void drawClock(GLUquadricObj *quadObj, GLfloat shell_angle)
{
    GLfloat spec1[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec1);
    glColor3f(0.8f, 0.1f, 0.1f);
    
    body(quadObj);
    
    GLfloat spec2[] = { 0.8f, 0.2f, 0.2f, 1.0f };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec2);
    glColor3f(1.0f, 1.0f, 1.0f);
    glPushMatrix();
        glTranslatef(1.0f, 0.0f, 0.0f);
        glRotatef(shell_angle, 0.0f, 1.0f, 0.0f);
        glTranslatef(-1.0f, 0.0f, 0.0f);
        shell(quadObj);
    glPopMatrix();
    
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, -hFace);
        face(quadObj);        
    glPopMatrix();
    
    glass(quadObj);
}

void body(GLUquadricObj *quadObj)
{
    glPushMatrix();
        glTranslated(0.0, 0.0, -hBody);
        glEnable(GL_TEXTURE_2D);
        gluCylinder(quadObj, rBottom, rTop, hBody, 50, 50);
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
    
    glPushMatrix();
        glTranslated(0.0, 0.0, -hBody);
        glRotated(180, 1.0, 0.0, 0.0);
        glEnable(GL_TEXTURE_2D);
        gluDisk(quadObj, 0.0, rBottom, 50, 50);
        glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void shell(GLUquadricObj *quadObj)
{
    GLdouble plane[] = { 0.0, 0.0, 1.0, 0.0 };
    glClipPlane(GL_CLIP_PLANE0, plane);
 
    glEnable(GL_CLIP_PLANE0);
    glEnable(GL_TEXTURE_2D);
    gluSphere(quadObj, 1.0, 50, 50);
    glDisable(GL_CLIP_PLANE0);
}

void face(GLUquadricObj *quadObj)
{
    glPushMatrix();
        glEnable(GL_TEXTURE_2D);
        gluCylinder(quadObj, rTop, rTop, hFace, 50, 50);
        gluDisk(quadObj, 0.0, rTop, 50, 50);
        glDisable(GL_TEXTURE_2D);
        arrows(quadObj);
        marks(quadObj);
    glPopMatrix();
}

void arrows(GLUquadricObj *quadObj)
{
    GLfloat rStick = 0.05;
    GLfloat height = 0.25;
    
    time_t t = time(0);
    struct tm *cur_time = localtime(&t);
    GLfloat angle;

    // hours
    angle = -(float)cur_time->tm_hour / 12 * 360;
    glPushMatrix();
        glRotatef(angle, 0.0f, 0.0f, 1.0f);
        arrow_hrs(quadObj);
    glPopMatrix();    
    
    // minutes
    angle = -(float)cur_time->tm_min / 60 * 360;
    glPushMatrix();
        glRotatef(angle, 0.0f, 0.0f, 1.0f);
        arrow_min(quadObj);
    glPopMatrix();
    
    // seconds
    angle = -(float)cur_time->tm_sec / 60 * 360;
    glPushMatrix();
        glRotatef(angle, 0.0f, 0.0f, 1.0f);
        arrow_sec(quadObj);
    glPopMatrix();
    
    // stick
    glEnable(GL_TEXTURE_2D);
    gluCylinder(quadObj, rStick, rStick, height, 20, 20);
    glPushMatrix();
        glTranslatef(0.0f, 0.0f, height);
        gluDisk(quadObj, 0, rStick, 20, 20);
    glPopMatrix();
    glDisable(GL_TEXTURE_2D);
}   

void arrow_hrs(GLUquadricObj *quadObj)
{
    GLfloat width = 0.2;
    GLfloat height1 = 0.6;
    GLfloat height2 = 0.3;
    
    glColor3f(0.0f, 0.0f, 1.0f);
    
    glTranslatef(0.0f, 0.0f, 0.1f);
    
    glBegin(GL_QUADS);
        glVertex2f(0, -height2);
        glVertex2f(width/2, 0);
        glVertex2f(0, height1);
        glVertex2f(-width/2, 0);
    glEnd();
}

void arrow_min(GLUquadricObj *quadObj)
{
    GLfloat width = 0.12;
    GLfloat height1 = 0.7;
    GLfloat height2 = 0.4;
    
    glColor3f(0.0f, 1.0f, 0.0f);
    
    glTranslatef(0.0f, 0.0f, 0.15f);
    
    glBegin(GL_QUADS);
        glVertex2f(0, -height2);
        glVertex2f(width/2, 0);
        glVertex2f(0, height1);
        glVertex2f(-width/2, 0);
    glEnd();    
}

void arrow_sec(GLUquadricObj *quadObj)
{
    GLfloat inner = 0.05;
    GLfloat outer = 0.1;
    GLfloat width = 0.03;
    GLfloat width2 = 0.06;
    GLfloat len1 = 0.7;
    GLfloat len2 = 0.4;
    GLfloat len3 = 0.2;
    
    glColor3f(1.0f, 0.0f, 0.0f);
    
    glTranslatef(0.0f, 0.0f, 0.2f);
    gluDisk(quadObj, inner, outer, 30, 30);
    
    glBegin(GL_QUADS);
        glVertex2f(-width/2, inner);
        glVertex2f(width/2, inner);
        glVertex2f(width/2, len1);
        glVertex2f(-width/2, len1);
    
        glVertex2f(-width/2, -inner);
        glVertex2f(-width/2, -len2);
        glVertex2f(width/2, -len2);
        glVertex2f(width/2, -inner);

        glVertex2f(-width2/2, -len2);
        glVertex2f(-width2/2, -len2-len3);
        glVertex2f(width2/2, -len2-len3);
        glVertex2f(width2/2, -len2);
    glEnd();
}

void marks(GLUquadricObj *quadObj)
{
    // big marks
    int d;
    for (d = 0; d < 12; ++d) {
        GLfloat angle = -(float)d / 12 * 360;
        glPushMatrix();
            glRotatef(angle, 0.0f, 0.0f, 1.0f);
            glTranslatef(0.0f, 0.8f, 0.05f);
            glScalef(0.1f, 0.1f, 0.1f);
            if (d == 0)
                glColor3f(0.0f, 1.0f, 1.0f);
            else
                glColor3f(1.0f, 1.0f, 0.0f);
            glutSolidTetrahedron();
        glPopMatrix();
    }
}

void glass(GLUquadricObj *quadObj)
{
    glPushAttrib(GL_COLOR_MATERIAL);
    
    GLfloat spec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
    
    glVer
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 128);
    glColor4f(1, 0.7, 1, 0.3);
    
    gluDisk(quadObj, 0, rTop, 50, 50);
    glPopAttrib();
}


