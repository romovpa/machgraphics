//
//  main.c
//  glclock
//
//  Created by Peter Romov on 11/10/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "glut.h"
#include "trackball.h"
#include "figures.h"

/* FUNCTIONS */

void init(void);
void display(void);
void reshape(int width, int height);
void keyboard(unsigned char key, int x, int y);
void mouse(int button, int state, int x, int y);
void motion(int x, int y);
void animate(void);
void render(void);

/* DATA */

#define WINDOW_WIDTH 400
#define WINDOW_HEIGHT 400

GLfloat scale = 0.3;

GLfloat light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
GLfloat light_diffuse[] = { 0.7f, 0.7f, 0.7f, 1.0f };
GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
GLfloat light_pos1[] = { -5.0f, 0.0f, 10.0f, 1.0f };
GLfloat light_pos2[] = { 5.0f, 0.0f, 10.0f, 1.0f };
GLfloat shadow1[4][4];
GLfloat shadow2[4][4];
GLfloat ground_plane[4] = { 0.0f, 1.0f, 0.0f, 0.0f };

GLUquadricObj *quadObj;

GLfloat shell_angle = 0.0;
GLfloat shell_step = 1;
int shell_action = 0;
int action_idle = CLOCKS_PER_SEC / 50;
time_t action_start = 0;

int fullscreen = GL_FALSE;
int antialiasing = GL_FALSE;

/* IMPLEMENTATION */

void init(void)
{
    tbInit();
    
    //glClearColor(0.52, 0.8, 0.97, 0.0); /* blue sky */
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glClearDepth(1.0);
    glClearStencil(0x0);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    
    //glEnable(GL_CULL_FACE);
    //glFrontFace(GL_CCW);
    
    glEnable(GL_LIGHTING);
    glShadeModel(GL_SMOOTH);
    
    glEnable(GL_LIGHT0);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos1);
    
    glEnable(GL_LIGHT1);
    glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
    glLightfv(GL_LIGHT0, GL_POSITION, light_pos2);
    
    quadObj = gluNewQuadric();
    gluQuadricDrawStyle(quadObj, GLU_FILL);
    
    initTextures(quadObj);
    
    tbInit();
}

void render(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();
    tbMatrix();
    glScalef(scale, scale, scale);
    drawClock(quadObj, shell_angle);
    glPopMatrix();  
}

void display(void)
{
    glClear(GL_ACCUM_BUFFER_BIT);
    
    if (antialiasing) {
        GLfloat delta = 0.002;
        GLfloat shiftX[] = {-1, 1, 0, 0};
        GLfloat shiftY[] = {0, 0, -1, 1};
        int i;
        for (i = 0; i < 4; ++i) {
            glPushMatrix();
            glTranslatef(delta*shiftX[i], delta*shiftY[i], 0.0f);
            render();
            glAccum(GL_ACCUM, 1.0/4);
            glPopMatrix();
        }
        glAccum(GL_RETURN, 1.0f);
    }
    else 
        render();

    glutSwapBuffers();
}

void animate(void)
{
    // wait for action_idle
    if (clock() - action_start < action_idle)
        return;
    else
        action_start = clock();
        
    
    if (shell_action > 0) {
        shell_angle += shell_step;
        if (shell_angle >= 90)
            shell_action = 0;
    }
    if (shell_action < 0) {
        shell_angle -= shell_step;
        if (shell_angle <= 0)
            shell_action = 0;
    }
    
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    if (state == GLUT_DOWN && button == GLUT_LEFT_BUTTON)
        tbStartMotion(x, y, button, glutGet(GLUT_ELAPSED_TIME));
    else if (state == GLUT_UP && button == GLUT_LEFT_BUTTON)
        tbStopMotion(button, glutGet(GLUT_ELAPSED_TIME));
}

void motion(int x, int y)
{
    tbMotion(x, y);
}

void reshape(int width, int height)
{
    glViewport(0, 0, width, height);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    GLdouble aspect = (GLdouble)height/width;
    if (width < height)
        glOrtho(-1.0, 1.0, -aspect, aspect, -1.0, 1.0);
    else
        glOrtho(-1.0/aspect, 1.0/aspect, -1.0, 1.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    tbReshape(width, height);
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 'q')
        exit(0);
    
    if (key == 'u')
        scale *= 0.9;
    if (key == 'd')
        scale *= 1.1;
    
    if (key == ' ') {
        if (shell_action)
            shell_action *= -1;
        else
            shell_action = (shell_angle <= 0 ? 1 : -1);
    }
    
    if (key == 'f') {
        if (!fullscreen) {
            glutFullScreen();
            fullscreen = GL_TRUE;
        }
        else {
            glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
            fullscreen = GL_FALSE;
        }
    }
    
    if (key == 'a') {
        antialiasing = !antialiasing;
    }
    
    glutPostRedisplay();
}

int main (int argc, char *argv[])
{
    glutInit(&argc, argv);
    
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_ACCUM | GLUT_MULTISAMPLE | GLUT_DEPTH | GLUT_STENCIL);
    glutCreateWindow("OpenGL Clock");
    glutReshapeWindow(WINDOW_WIDTH, WINDOW_HEIGHT);
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutIdleFunc(animate);
    
    init();
    
    glutMainLoop();
    return 0;
}

