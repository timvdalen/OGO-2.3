/*
 * Video subsystem
 *
 * Date: 19-05-12 15:47
 *
 * Description: Wrapper for hardware accelerated graphics rendering.
 *              Currently uses OpenGL
 *
 */

#ifndef _VIDEO_H
#define _VIDEO_H

#if defined _WIN32
	#define WIN32_LEAN_AND_MEAN
	#include <windows.h>
	#include <gl\gl.h>
	#include <gl\glu.h>
	#include <gl\freeglut.h>
	#include <gl\glext.h>
#elif defined __APPLE__
	#include <OpenGL/gl.h>
	#include <OpenGL/glu.h>
	#include <GLUT/freeglut.h>
#else
	#include <GL/gl.h>
	#include <GL/glu.h>
	#include <GL/freeglut.h>
#endif

#endif // _VIDEO_H

//------------------------------------------------------------------------------
