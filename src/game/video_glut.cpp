/*
 * Video subsystem -- see header file
 */

#if defined _WIN32
	#include <gl\freeglut.h>
#elif defined __APPLE__
	#include <GLUT/freeglut.h>
#else
	#include <GL/freeglut.h>
#endif

#include "objects.h"
#include "video.h"

#define PRIV(T,x) T *x = (T *) data;

namespace Video {

using namespace std;

//------------------------------------------------------------------------------

void Initialize(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);
	
	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
}

//------------------------------------------------------------------------------

void Terminate()
{
	Window::windows.clear();
}

//------------------------------------------------------------------------------

void StartEventLoop()
{
	glutDisplayFunc(Window::display);
	
	glutMainLoop();
}

//------------------------------------------------------------------------------

void StopEventLoop()
{
	// Todo: only freeglut can do this, find documentation
}

//==============================================================================

std::set<Window *> Window::windows;

struct WindowData
{
	uword width, height;
};

//------------------------------------------------------------------------------

Window::Window(uword width, uword height, const char *title,
               word xpos, word ypos)
{
	data = (void *) new WindowData;
	
	PRIV(WindowData, wd);
	wd->width = width;
	wd->height = height;
	
	glutInitWindowPosition(xpos, ypos);
	glutInitWindowSize(width, height);
	glutCreateWindow(title);
	
	windows.insert(this);
}

//------------------------------------------------------------------------------

Window::~Window()
{
	windows.erase(this);
	
	PRIV(WindowData, wd)
	delete wd;
}

//------------------------------------------------------------------------------

void Window::render()
{
	PRIV(WindowData, wd)
	
	set<Viewport *>::iterator vit;
	for (vit = viewports.begin(); vit != viewports.end(); ++vit)
	{
		(*vit)->select(this);
		(*vit)->render();
	}
}

//------------------------------------------------------------------------------

void Window::display()
{
	set<Window *>::iterator wit;
	for (wit = windows.begin(); wit != windows.end(); ++wit)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		
		(*wit)->render();
			
		glutSwapBuffers();
	}
}

//==============================================================================

struct ViewportData
{
	double x, y;
	double w, h;
	double f, a;
	bool changed;
	Window *lastwin;
};

//------------------------------------------------------------------------------

Viewport::Viewport(double width, double height, double xpos, double ypos,
                   double fov)
{
	data = (void*) new ViewportData;
	
	PRIV(ViewportData, vd)
	
	vd->w = width;
	vd->h = height;
	vd->x = xpos;
	vd->y = ypos;
	vd->f = fov;
	vd->changed = true;
	vd->lastwin = NULL;
	
	if (vd->h < 1)
		vd->h = 1;
	
}

//------------------------------------------------------------------------------

Viewport::~Viewport()
{
	PRIV(ViewportData, vd)
	delete vd;
}

//------------------------------------------------------------------------------

void Viewport::move(double xpos, double ypos)
{
	PRIV(ViewportData, vd)
	vd->x = xpos;
	vd->y = ypos;
	vd->changed = true;
}

//------------------------------------------------------------------------------

void Viewport::resize(double width, double height)
{
	PRIV(ViewportData, vd)
	
	if ((width < 1) || (height < 1))
		return;
	
	vd->w = width;
	vd->h = height;
	vd->a = width / height;
	vd->changed = true;
}

//------------------------------------------------------------------------------

void Viewport::setfov(double fov)
{
	PRIV(ViewportData, vd)
	
	vd->f = fov;
	vd->changed = true;
}

//------------------------------------------------------------------------------

void Viewport::select(Window *w)
{
	PRIV(ViewportData, vd)
	if (!w) return;
	
	WindowData *wd = (WindowData *) w->data;
	
	vd->lastwin = w;
	
	glViewport((GLint) vd->x * wd->width, (GLint) vd->y * wd->height,
	           (GLint) vd->w * wd->width, (GLint) vd->h * wd->height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(vd->f, vd->a, 0.1f, 1000.0f);
	
	vd->changed = false;
}

//------------------------------------------------------------------------------

void Viewport::render()
{
	if (!world) return;
	PRIV(ViewportData, vd)
	
	// Reset viewport when parameters changed
	if (vd->changed)
		select(vd->lastwin);
	
	// Set up render
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity ();
	
	glClear(GL_DEPTH_BUFFER_BIT);
	
	// Set up camera
	// camera: Cam.origin(x,y,z) world center(x,y,z) Z-axis(x,y,z)
	//Point<double> p = cam.origin + (~cam.objective * Vector<double>(1,0,0));
	//gluLookAt(cam.origin.x, cam.origin.y, cam.origin.z, p.x, p.y, p.z, 0, 0, 1);
	
	// I might as well define the matrix myself...
	{
		 // Set up xyz axes how I like them
		double m1[16] = { 0,  0,  1,  0,
		                  1,  0,  0,  0,
					      0,  1,  0,  0,
					      0,  0,  0,  1};
		
		glMultMatrixd(m1);
		
		// Camera orientation (quaternion to rotation matrix)
		const Quaternion<double> &q = camera.objective;
		double aa, ab, ac, ad, bb, bc, bd, cc, cd, dd;
		aa = q.a*q.a; ab = q.a*q.b; ac = q.a*q.c; ad = q.a*q.d;
		              bb = q.b*q.b; bc = q.b*q.c; bd = q.b*q.d;
		                            cc = q.c*q.c; cd = q.c*q.d;
		                                          dd = q.d*q.d;
		
		double m2[16] =
			{aa+bb-cc-dd, 2.0*(bc-ad), 2.0*(bd+ac),           0,
			 2.0*(bc+ad), aa-bb+cc-dd, 2.0*(cd-ab),           0,
			 2.0*(bd-ac), 2.0*(cd+ab), aa-bb-cc+dd,           0,
			           0,           0,           0, aa+bb+cc+dd};
		
		glMultMatrixd(m2);
		
		// Camera position
		const Point<double> &o = camera.origin;
		glTranslated(-o.x, -o.y, -o.z);
	}
	
	// Set up lights
	/*if (scene.lights.size())
	{
		glEnable(GL_LIGHTING);
		
		std::vector<Light>::const_iterator it;
		int i = GL_LIGHT0;
		for (it = scene.lights.begin(); it != scene.lights.end(); ++it, ++i)
		{			
			GLfloat pos[]  = {it->origin.x, it->origin.y, it->origin.z, 1.0};
			GLfloat dir[]  = {it->direction.x, it->direction.y, it->direction.z};
				
			glLightfv(i, GL_AMBIENT, it->ambient.v);
			glLightfv(i, GL_DIFFUSE, it->diffuse.v);
			glLightfv(i, GL_SPECULAR, it->specular.v);
			glLightfv(i, GL_POSITION, pos);
			glLightfv(i, GL_SPOT_DIRECTION, dir);
			glLightf(i, GL_SPOT_EXPONENT, it->exponent);
			glLightf(i, GL_SPOT_CUTOFF, it->cutoff);
			glEnable(i);
		}
		
		for (last_light_count -= scene.lights.size(); last_light_count > 0;
			--last_light_count)
			glDisable(i++);
		
		last_light_count = scene.lights.size();
	}*/
	
	// Render objects
	std::set<ObjectHandle>::const_iterator it;
	for (it = world->children.begin(); it != world->children.end(); ++it)
		(*it)->render();
	
}

//------------------------------------------------------------------------------

} // namespace Video

//------------------------------------------------------------------------------
