/*
 * Video subsystem -- see header file
 */

#if defined _WIN32
	#include <gl\freeglut.h>
#elif defined __APPLE__
	#include <GL/freeglut.h>
#else
	#include <GL/freeglut.h>
#endif

#include <set>

#include "objects.h"
#include "video.h"

#define CALL(x, ...) { if (x) (x)(__VA_ARGS__); }

#define PRIV(T,x) T *x = (T *) data;

namespace Video {

using namespace std;
using namespace Base::Alias;

void(*OnFrame) () = NULL;

//------------------------------------------------------------------------------

void Initialize(int argc, char *argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH | GLUT_MULTISAMPLE);
	glutSetKeyRepeat(GLUT_KEY_REPEAT_ON);
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
	glutReshapeFunc(Window::resize);
	glutTimerFunc(16, Window::timer, 0);

	glutMainLoop();
}

//------------------------------------------------------------------------------

void StopEventLoop()
{
	glutLeaveMainLoop();
}

//==============================================================================

std::set<Window *> Window::windows;

struct WindowData
{
	uword width, height;
	double aspect;
	int wid;
};

//------------------------------------------------------------------------------

Window::Window(uword width, uword height, const char *title,
               word xpos, word ypos)
{
	data = (void *) new WindowData;

	PRIV(WindowData, wd);
	wd->width = width;
	wd->height = height;
	wd->aspect = (double) width / (double) height;

	glutInitWindowPosition(xpos, ypos);
	glutInitWindowSize(width, height);
	glutCreateWindow(title);
	wd->wid = glutGetWindow();

	glClearColor(0, 0, 0, 0);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);

	glEnable(GL_LIGHTING);
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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

void Window::select()
{
	PRIV(WindowData, wd)
	glutSetWindow(wd->wid);
}

//------------------------------------------------------------------------------

void Window::render()
{
	PRIV(WindowData, wd)

	select();

	glClear(GL_COLOR_BUFFER_BIT);

	vector<Viewport *>::iterator vit;
	for (vit = viewports.begin(); vit != viewports.end(); ++vit)
	{
		(*vit)->select(this);
		(*vit)->render();
	}

	glutSwapBuffers();
}

//------------------------------------------------------------------------------

void Window::size(uword &width, uword &height)
{
	PRIV(WindowData, wd)

	select();

	if (width || height)
	{
		if (!width)  width = wd->width;
		if (!height) height = wd->height;
		glutReshapeWindow(width, height);
		return;
	}

	width = wd->width;
	height = wd->height;
}

//------------------------------------------------------------------------------

void Window::display()
{
	set<Window *>::iterator wit;
	for (wit = windows.begin(); wit != windows.end(); ++wit)
		(*wit)->render();
}

//------------------------------------------------------------------------------

void Window::resize(int width, int height)
{
	// Currently only sets the first window
	if (windows.empty()) return;

	void *data = (*windows.begin())->data;
	PRIV(WindowData, wd)

	wd->width = width;
	wd->height = height;
	wd->aspect = (double) width / (double) height;
	(*windows.begin())->render();
}

//------------------------------------------------------------------------------

void Window::timer(int state)
{
	CALL(OnFrame);

	glutTimerFunc(16, Window::timer, 0);
}

//==============================================================================

void Camera::lookAt(const Point<double> &target)
{
	//const Vd def = Vd(0,1,0);
	//Vd dir = ~Vd(target - origin);
	//double dot = dir ^ def;

	//if (dot == 1.0)
		//objective = Rd(0.0, def);
	//else if (dot == -1.0)
		//objective = Rd(Pi, Vd(0,0,1));
	//else
		//objective = Rd(Vd(0,1,0), dir);
    Vd dir = ~Vd(target-origin);
    double len = sqrt(dir.x * dir.x + dir.y * dir.y);
    double pitch = atan2(dir.z, len);
    double pan = atan2(dir.x, dir.y);
    objective = Qd(Rd(-pitch, Vd(1, 0, 0))) * Qd(Rd(pan, Vd(0, 0, 1)));

}

//==============================================================================

struct ViewportData
{
	double x, y;
	double w, h;
	double f, a;
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
	vd->a = width / height;

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
}

//------------------------------------------------------------------------------

void Viewport::setfov(double fov)
{
	PRIV(ViewportData, vd)

	vd->f = fov;
}

//------------------------------------------------------------------------------

void Viewport::select(Window *w)
{
	PRIV(ViewportData, vd)
	if (!w) return;

	WindowData *wd = (WindowData *) w->data;

	glViewport((GLint) vd->x * wd->width, (GLint) vd->y * wd->height,
	           (GLint) vd->w * wd->width, (GLint) vd->h * wd->height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(vd->f, vd->a * wd->aspect, 0.1f, 1000.0f);
}

//------------------------------------------------------------------------------

void Viewport::render()
{
	if (!world) return;
	PRIV(ViewportData, vd)

	// Set up render
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glClear(GL_DEPTH_BUFFER_BIT);

	// Set up camera
	// camera: Cam.origin(x,y,z) world center(x,y,z) Z-axis(x,y,z)
	//Point<double> p = cam.origin + (~cam.objective * Vector<double>(1,0,0));
	//gluLookAt(cam.origin.x, cam.origin.y, cam.origin.z, p.x, p.y, p.z, 0, 0, 1);

	// I might as well define the matrix myself...
	{
		 // Set up xyz axes how I like them
		double m1[16] = { 1,  0,  0,  0,
		                  0,  0, -1,  0,
					      0,  1,  0,  0,
					      0,  0,  0,  1};

		glMultMatrixd(m1);

		// Camera orientation (quaternion to rotation matrix)
		const Quaternion<double> &q = -camera.objective;
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

	// Enable lighting
	glEnable(GL_LIGHT0);

	// Render the world and everything in it
	world->render();
}

//==============================================================================

double FPS::operator()()
{
	frames++;

	int ct = glutGet(GLUT_ELAPSED_TIME);
	int dt = ct - time;
	if (dt > 1000)
	{
		fps = ((double)frames / (double)dt) * 1000.0;
		time = ct;
		frames = 0;
	}

	return fps;
}

//------------------------------------------------------------------------------

} // namespace Video

//------------------------------------------------------------------------------
