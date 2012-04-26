/*
	OGO 2.3	Game project

*/

// Include header files according to platform
//Using freeglut instead of glu
#if defined _WIN32
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <gl\gl.h>
# include <gl\glu.h>
# include <gl\glut.h>
#elif defined __APPLE__
# include <OpenGL/gl.h>
# include <OpenGL/glu.h>
# include <GLUT/freeglut.h>
#else
#include <GL/glew.h>
# include <GL/freeglut.h>
#endif



#include <stdlib.h>
#include <stdio.h>
#include <sstream>
#include <string>
#include <time.h>
#include <vector>

#include <string.h>
#include "dsmodel.cpp"

#include <math.h>       // trigonometry support
#include <cmath>	// float mod
#ifndef M_PI
# define M_PI 3.14159265358979323846
#endif
#ifndef M_E
# define M_E 2.7182818284
#endif
#ifndef NULL
# define NULL ((void *)(0))
#endif

#include <string.h>
#include "dsmodel.cpp" 

#define MOUSEWHEEL_UP 3
#define MOUSEWHEEL_DOWN 4

//Global vars
int winx,winy = 500;
Object *obj;

//Includes van classes


/*  Display function.
 *  Renders the picture that is going to be displayed  */
void display (void) {
	//clear image- and depth buffer to get a clean canvas
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glClearColor(1.0, 0.0, 0.0, 0.0);

	obj->Draw();


	//Swap buffers to display result
	glutSwapBuffers();
}

/*  Reshape function.
    activated when the position/size of the glut window changes (w, h window dims)*/
void reshape (int w, int h) {
    winx=w;
    winy=h;

    // Set projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    // Set up a perspective camera at point (0,0,0), looking in negative Z-direction
    // Angle of view is 90 degrees. Near and far clipping planes are 10 and 1000
    gluPerspective(90.0, 1.0, 1.0, 1000);
    // set up geometric transformations
    glMatrixMode(GL_MODELVIEW);
    // Set up viewport
    glViewport(0, 0, w, h);
}

/*  Idle function,
 *  activated when the event queue is empty (all the time)  */
void idle (void) {
   glutPostRedisplay();
}



/*  Mouse callback function
 *  x and y are the mouse coordinates when the click occured  */
void mouse (int button, int state, int x, int y) {
	if(state==GLUT_DOWN && button == GLUT_LEFT_BUTTON){
        // TODO: Left mouse
	}
	if(state==GLUT_DOWN && button == GLUT_RIGHT_BUTTON){
        // TODO: Right mouse
	}
	/* Mousewheel */
    if(button == MOUSEWHEEL_UP){
		// TODO: Mousewheel up
	}else if(button == MOUSEWHEEL_DOWN){
		// TODO: Mousewheel down
	}
}


/*  The keyboard callback function
 *  x and y are the mouse coordinates when the character was typed  */
void keyboard (unsigned char key, int x, int y) {
    switch(key){
    	
	}
}


/*  The menu callback function */
void menu(int entry) {
	keyboard(entry, 0, 0);
}

/*  Main function:
 *  program entry point  */
int main (int argc, char **argv) {
    //Initializing GLUT
    glutInit(&argc, argv);
    // we want:     double buffering, rgb color and Z-buffer
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    // initialize window with size and name

#ifdef FULLSCREEN
    glutGameModeString("1440x900:32@60");
    glutEnterGameMode();
#else
    glutInitWindowSize(winx, winy);
    glutCreateWindow("OpenGL/glut skeleton. Press right mouse button !");
#endif

   	// set up callback functions
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    glutDisplayFunc(display);
    glutIdleFunc(idle);


	obj = new Object("balk.3ds");

    // enter main loop (will cause OpenGL to begin calling the callbacks)
    glutMainLoop();
    return 0;
}
