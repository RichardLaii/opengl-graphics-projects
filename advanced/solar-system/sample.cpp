#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define GLM_FORCE_RADIANS
#include "glm/vec2.hpp"
#include "glm/vec3.hpp"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/matrix_inverse.hpp"
#include "glm/gtc/type_ptr.hpp"

#ifndef F_PI
#define F_PI		((float)(M_PI))
#define F_2_PI		((float)(2.f*F_PI))
#define F_PI_2		((float)(F_PI/2.f))
#endif


#ifdef WIN32
#include <windows.h>
#pragma warning(disable:4996)
#endif

#include "glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "glut.h"


//	This is a sample OpenGL / GLUT program
//
//	The objective is to draw a 3d object and change the color of the axes
//		with a glut menu
//
//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//
//	Author:			Richard Lai

// title of these windows:

const char *WINDOWTITLE = "OpenGL / GLUT Sample -- Richard Lai";
const char *GLUITITLE   = "User Interface Window";

// what the glui package defines as true and false:

const int GLUITRUE  = true;
const int GLUIFALSE = false;

// the escape key:

const int ESCAPE = 0x1b;

// initial window size:

const int INIT_WINDOW_SIZE = 600;

// size of the 3d box to be drawn:

const float BOXSIZE = 2.f;

// multiplication factors for input interaction:
//  (these are known from previous experience)

const float ANGFACT = 1.f;
const float SCLFACT = 0.005f;

// minimum allowable scale factor:

const float MINSCALE = 0.05f;

// scroll wheel button values:

const int SCROLL_WHEEL_UP   = 3;
const int SCROLL_WHEEL_DOWN = 4;

// equivalent mouse movement when we click the scroll wheel:

const float SCROLL_WHEEL_CLICK_FACTOR = 5.f;

// active mouse buttons (or them together):

const int LEFT   = 4;
const int MIDDLE = 2;
const int RIGHT  = 1;

const float EYEDISTFACTOR = 0.75f;
const float ZFAR = 1000000.0f;
const float FOVDEG = 90.0f;

const int MAXIMUM_TIME_SECONDS = 10 * 60; //  10 minutes
const int MAXIMUM_TIME_MILLISECONDS = 1000 * MAXIMUM_TIME_SECONDS;
const float ONE_FULL_TURN = 2.f * F_PI; // this is 2£k instead of 360¢X because glm uses radians

// time scale wrt days (bigger creates slower):
const float TS = 1.08f / (24.f * 60.f * 60.f);
// this creates a 1 second on the screen = 1 day in the real solar system
// planet radius scale:
const float PRS = 0.1f / 3964.f;
// sun radius scale:
const float SRS = PRS / 40.f;
// earth orbital radius scale:
const float EORS = 1.f / 93000000.f;
// moon orbital radius scale:
const float MORS = 60.f * EORS;
const float SUN_RADIUS_MILES = SRS * 432690.f;
const float SUN_SPIN_TIME_DAYS_EQUATOR = TS * 25.f;
const float SUN_SPIN_TIME_HOURS_EQUATOR = SUN_SPIN_TIME_DAYS_EQUATOR * 24.f;
const float SUN_SPIN_TIME_SECONDS_EQUATOR = SUN_SPIN_TIME_HOURS_EQUATOR * 60.f * 60.f;
const float SUN_SPIN_TIME_DAYS_POLES = TS * 35.f;
const float SUN_SPIN_TIME_HOURS_POLES = SUN_SPIN_TIME_DAYS_POLES * 24.f;
const float SUN_SPIN_TIME_SECONDS_POLES = SUN_SPIN_TIME_HOURS_POLES * 60.f * 60.f;

// MERCURY VENUS EARTH MARS JUPITER SATURN URANUS NEPTUNE 

const float MERCURY_TILT_ANGLE_DEG = 0.f;
const float MERCURY_ORBITAL_RADIUS_MILES = EORS * 92900000.f * 0.3871 + 2;
const float MERCURY_ORBIT_TIME_DAYS = TS * 88.f;
const float MERCURY_ORBIT_TIME_HOURS = MERCURY_ORBIT_TIME_DAYS * 24.f;
const float MERCURY_ORBIT_TIME_SECONDS = MERCURY_ORBIT_TIME_HOURS * 60.f * 60.f;
const float MERCURY_SPIN_TIME_DAYS = TS * 0.9971f * 59;
const float MERCURY_SPIN_TIME_HOURS = MERCURY_SPIN_TIME_DAYS * 24.f;
const float MERCURY_SPIN_TIME_SECONDS = MERCURY_SPIN_TIME_HOURS * 60.f * 60.f;

const float VENUS_TILT_ANGLE_DEG = 2.6f;
const float VENUS_ORBITAL_RADIUS_MILES = EORS * 92900000.f * 0.7233 + 2;
const float VENUS_ORBIT_TIME_DAYS = TS * 225.f;
const float VENUS_ORBIT_TIME_HOURS = VENUS_ORBIT_TIME_DAYS * 24.f;
const float VENUS_ORBIT_TIME_SECONDS = VENUS_ORBIT_TIME_HOURS * 60.f * 60.f;
const float VENUS_SPIN_TIME_DAYS = TS * 0.9971f * 243;
const float VENUS_SPIN_TIME_HOURS = VENUS_SPIN_TIME_DAYS * 24.f;
const float VENUS_SPIN_TIME_SECONDS = VENUS_SPIN_TIME_HOURS * 60.f * 60.f;

const float EARTH_RADIUS_MILES = PRS * 3964.19f;
const float EARTH_TILT_ANGLE_DEG = 23.44f;
const float EARTH_ORBITAL_RADIUS_MILES = EORS * 92900000.f + 2;
const float EARTH_ORBIT_TIME_DAYS = TS * 365.3f;
const float EARTH_ORBIT_TIME_HOURS = EARTH_ORBIT_TIME_DAYS * 24.f;
const float EARTH_ORBIT_TIME_SECONDS = EARTH_ORBIT_TIME_HOURS * 60.f * 60.f;
const float EARTH_SPIN_TIME_DAYS = TS * 0.9971f;
const float EARTH_SPIN_TIME_HOURS = EARTH_SPIN_TIME_DAYS * 24.f;
const float EARTH_SPIN_TIME_SECONDS = EARTH_SPIN_TIME_HOURS * 60.f * 60.f;

const float MERCURY_RADIUS_MILES = EARTH_RADIUS_MILES * 0.383;

const float MOON_RADIUS_MILES = PRS * 1079.6f;
const float MOON_ORBITAL_RADIUS_MILES = MORS * 238900.f;
const float MOON_ORBITAL_INCLINATION_DEG = 5.14f;
const float MOON_ORBIT_TIME_DAYS = TS * 27.3f;
const float MOON_ORBIT_TIME_HOURS = MOON_ORBIT_TIME_DAYS * 24.f;
const float MOON_ORBIT_TIME_SECONDS = MOON_ORBIT_TIME_HOURS * 60.f * 60.f;
const float MOON_SPIN_TIME_DAYS = MOON_ORBIT_TIME_DAYS;
const float MOON_SPIN_TIME_HOURS = MOON_SPIN_TIME_DAYS * 24.f;
const float MOON_SPIN_TIME_SECONDS = MOON_SPIN_TIME_HOURS * 60.f * 60.f;

const float MARS_RADIUS_MILES = EARTH_RADIUS_MILES * 0.53;
const float MARS_TILT_ANGLE_DEG = 25.2f;
const float MARS_ORBITAL_RADIUS_MILES = EORS * 92900000.f * 1.5237 + 2;
const float MARS_ORBIT_TIME_DAYS = TS * 687.f;
const float MARS_ORBIT_TIME_HOURS = MARS_ORBIT_TIME_DAYS * 24.f;
const float MARS_ORBIT_TIME_SECONDS = MARS_ORBIT_TIME_HOURS * 60.f * 60.f;
const float MARS_SPIN_TIME_DAYS = TS * 1.0178f;
const float MARS_SPIN_TIME_HOURS = MARS_SPIN_TIME_DAYS * 24.f;
const float MARS_SPIN_TIME_SECONDS = MARS_SPIN_TIME_HOURS * 60.f * 60.f;

const float JUPITER_TILT_ANGLE_DEG = 3.1f;
const float JUPITER_ORBITAL_RADIUS_MILES = EORS * 92900000.f * 5.2028 + 2;
const float JUPITER_ORBIT_TIME_DAYS = TS * 365.3f * 11.9;
const float JUPITER_ORBIT_TIME_HOURS = JUPITER_ORBIT_TIME_DAYS * 24.f;
const float JUPITER_ORBIT_TIME_SECONDS = JUPITER_ORBIT_TIME_HOURS * 60.f * 60.f;
const float JUPITER_SPIN_TIME_DAYS = TS * 0.4154f;
const float JUPITER_SPIN_TIME_HOURS = JUPITER_SPIN_TIME_DAYS * 24.f;
const float JUPITER_SPIN_TIME_SECONDS = JUPITER_SPIN_TIME_HOURS * 60.f * 60.f;

const float SATURN_TILT_ANGLE_DEG = 26.7f;
const float SATURN_ORBITAL_RADIUS_MILES = EORS * 92900000.f * 9.5388 + 2;
const float SATURN_ORBIT_TIME_DAYS = TS * 365.3f * 29.5;
const float SATURN_ORBIT_TIME_HOURS = SATURN_ORBIT_TIME_DAYS * 24.f;
const float SATURN_ORBIT_TIME_SECONDS = SATURN_ORBIT_TIME_HOURS * 60.f * 60.f;
const float SATURN_SPIN_TIME_DAYS = TS * 0.4258f;
const float SATURN_SPIN_TIME_HOURS = SATURN_SPIN_TIME_DAYS * 24.f;
const float SATURN_SPIN_TIME_SECONDS = SATURN_SPIN_TIME_HOURS * 60.f * 60.f;

const float URANUS_TILT_ANGLE_DEG = 97.8f;
const float URANUS_ORBITAL_RADIUS_MILES = EORS * 92900000.f * 19.1914 + 2;
const float URANUS_ORBIT_TIME_DAYS = TS * 365.3f * 84;
const float URANUS_ORBIT_TIME_HOURS = URANUS_ORBIT_TIME_DAYS * 24.f;
const float URANUS_ORBIT_TIME_SECONDS = URANUS_ORBIT_TIME_HOURS * 60.f * 60.f;
const float URANUS_SPIN_TIME_DAYS = TS * 0.7062f;
const float URANUS_SPIN_TIME_HOURS = URANUS_SPIN_TIME_DAYS * 24.f;
const float URANUS_SPIN_TIME_SECONDS = URANUS_SPIN_TIME_HOURS * 60.f * 60.f;

const float NEPTUNE_TILT_ANGLE_DEG = 28.8f;
const float NEPTUNE_ORBITAL_RADIUS_MILES = EORS * 92900000.f * 30.0611 + 2;
const float NEPTUNE_ORBIT_TIME_DAYS = TS * 365.3f * 164.8;
const float NEPTUNE_ORBIT_TIME_HOURS = NEPTUNE_ORBIT_TIME_DAYS * 24.f;
const float NEPTUNE_ORBIT_TIME_SECONDS = NEPTUNE_ORBIT_TIME_HOURS * 60.f * 60.f;
const float NEPTUNE_SPIN_TIME_DAYS = TS * 0.6647f;
const float NEPTUNE_SPIN_TIME_HOURS = NEPTUNE_SPIN_TIME_DAYS * 24.f;
const float NEPTUNE_SPIN_TIME_SECONDS = NEPTUNE_SPIN_TIME_HOURS * 60.f * 60.f;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
};

enum views
{
	OUTSIDEVIEW1,
	OUTSIDEVIEW2,
	OUTSIDEVIEW3,
	EARTHVIEW, 
	MOONVIEW,
	MARSVIEW,
	MERVIEW,
	VENUSVIEW,
	JVIEW,
	NVIEW,
	UVIEW,
	SVIEW
};

enum views NowView;

enum Planet
{
	VENUS,
	EARTH,
	MARS,
	JUP,
	SAT,
	URA,
	NEP
};

// which button:

enum ButtonVals
{
	RESET,
	QUIT
};

// window background color (rgba):

const GLfloat BACKCOLOR[ ] = { 0., 0., 0., 1. };

// line width for the axes:

const GLfloat AXES_WIDTH   = 3.;

// the color numbers:
// this order must match the radio button order, which must match the order of the color names,
// 	which must match the order of the color RGB values

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta"
};

// the color definitions:
// this order must match the menu order

const GLfloat Colors[ ][3] = 
{
	{ 1., 0., 0. },		// red
	{ 1., 1., 0. },		// yellow
	{ 0., 1., 0. },		// green
	{ 0., 1., 1. },		// cyan
	{ 0., 0., 1. },		// blue
	{ 1., 0., 1. },		// magenta
};

// fog parameters:

const GLfloat FOGCOLOR[4] = { .0f, .0f, .0f, 1.f };
const GLenum  FOGMODE     = GL_LINEAR;
const GLfloat FOGDENSITY  = 0.30f;
const GLfloat FOGSTART    = 1.5f;
const GLfloat FOGEND      = 4.f;

// for lighting:

const float	WHITE[ ] = { 1.,1.,1.,1. };

// for animation:

const int MS_PER_CYCLE = 10000;		// 10000 milliseconds = 10 seconds


// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
int		SphereDL;				// object display list
int		S_RingDL;
int		OrbitDL;
int		MorbitDL, VorbitDL, EorbitDL, MarsorbitDL, JorbitDL, SorbitDL, UorbitDL, NorbitDL;
int		StarDL, SunDL, MercuryDL, VenusDL, EarthDL, MarsDL, JupiterDL, SaturnDL, UranusDL, NeptuneDL, MoonDL;
GLuint	StarTex, SunTex, MercuryTex, VenusTex, EarthTex, MarsTex, JupiterTex, SaturnTex, UranusTex, NeptuneTex, MoonTex;
GLuint	S_RingTex;
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
int		MainWindow;				// window id for main graphics window
int		NowColor;				// index into Colors[ ]
int		NowProjection;		// ORTHO or PERSP
int		NowPlanet;
int		NowMode = 0;
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
float	Time;					// used for animation, this has a value between 0. and 1.
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees
int		TimeScale = 1;

glm::vec3
LatLngToXYZ(float lat, float lng, float rad)
{
	lat = glm::radians(lat);
	lng = glm::radians(lng);
	glm::vec3 xyz;
	float xz = cosf(lat);
	xyz.x = -rad * xz * cosf(lng);
	xyz.y = rad * sinf(lat);
	xyz.z = rad * xz * sinf(lng);
	return xyz;
}



// function prototypes:

void	Animate( );
void	Display( );
void	DoAxesMenu( int );
void	DoColorMenu( int );
void	DoDepthBufferMenu( int );
void	DoDepthFightingMenu( int );
void	DoDepthMenu( int );
void	DoDebugMenu( int );
void	DoMainMenu( int );
void	DoProjectMenu( int );
void	DoRasterString( float, float, float, char * );
void	DoStrokeString( float, float, float, float, char * );
float	ElapsedSeconds( );
void	InitGraphics( );
void	InitLists( );
void	InitMenus( );
void	Keyboard( unsigned char, int, int );
void	MouseButton( int, int, int, int );
void	MouseMotion( int, int );
void	Reset( );
void	Resize( int, int );
void	Visibility( int );
void	Circle(float xc, float zc, float r, int numsegs) //TEST
{
	float dang = 2.f * F_PI / (float)numsegs;
	float ang = 0.;
	glBegin(GL_LINE_STRIP);
	glColor3f(1., 1., 1.);
	//glVertex3f(xc, 0., zc);
	for (int i = 0; i <= numsegs; i++)
	{
		float x = xc + r * cosf(ang);
		float z = zc + r * sinf(ang);
		glVertex3f(r * cosf(ang), 0., r * sinf(ang));
		ang += dang;
	}
	glEnd();
}

void
SetViewingFromLatLng(float eyeLat, float eyeLng, float lookLat, float lookLng, float rad, glm::vec4* eyep, glm::vec4* lookp, glm::vec4* upp, float* znearp)
{
	const glm::vec3 center = glm::vec3(0., 0., 0.);// center of planet
	glm::vec3 eye = LatLngToXYZ(eyeLat, eyeLng, rad);
	glm::vec3 look = LatLngToXYZ(lookLat, lookLng, rad);
	glm::vec3 upVec = glm::normalize(look - center);// perpendicular to the globe at the look position
	glm::vec3 eyeToLook = look - eye;// vector from eye to look
	float znear;// how far in front of eye to near clipping plane
	float T = tanf(glm::radians(FOVDEG / 2.f));
	float distToEye = EYEDISTFACTOR * rad; // distance from look to eye'
	glm::vec3 perpComponent = glm::dot(eyeToLook, upVec) * upVec;
	glm::vec3 tangentComponent = eyeToLook - perpComponent;
	glm::vec3 eyeprime = look - distToEye * glm::normalize(tangentComponent);
	float A = pow(T, 2) + 1.f;
	float B = -2.f * rad * T - 2.f * distToEye;
	float C = pow(distToEye, 2);
	float disc = sqrtf(pow(B, 2) - 4.f * A * C);
	znear = (-B + disc) / (2.f * A);
	float n2 = (-B - disc) / (2.f * A);
	if (n2 < znear)
		znear = n2; 
	*eyep = glm::vec4(eyeprime, 1.); 
	*lookp = glm::vec4(look, 1.); 
	*upp = glm::vec4(upVec, 0.); 
	*znearp = znear;
}


void			Axes( float );
void			HsvRgb( float[3], float [3] );
void			Cross(float[3], float[3], float[3]);
float			Dot(float [3], float [3]);
float			Unit(float [3], float [3]);
float			Unit(float [3]);


// utility to create an array from 3 separate values:
// MERCURY VENUS EARTH MARS JUPITER SATURN URANUS NEPTUNE 

glm::mat4
MakeSunMatrix()
{
	const float sunSpinAngle = ONE_FULL_TURN * Time * TimeScale / SUN_SPIN_TIME_SECONDS_EQUATOR;
	//const float mercuryOrbitAngle = ONE_FULL_TURN * Time / MERCURY_ORBIT_TIME_SECONDS;
	const glm::mat4 identity = glm::mat4(1.);
	const glm::vec3 yaxis = glm::vec3(0., 1., 0.);
	const glm::vec3 zaxis = glm::vec3(0., 0., 1.);
	// note: because the earth is tilted, we cannot play the translate+rotate trick -- the tilt must always be in the same direction// instead we must translate the earth to where it really belongs:
	//glm::vec3 morbitPosition = glm::vec3(MERCURY_ORBITAL_RADIUS_MILES * cosf(mercuryOrbitAngle), 0., -MERCURY_ORBITAL_RADIUS_MILES * sinf(mercuryOrbitAngle));
	// glm::mat4 mtransx = glm::translate(identity, morbitPosition);
	// glm::mat4 mtilt = glm::rotate(identity, glm::radians(MERCURY_TILT_ANGLE_DEG), zaxis);
	/* 1. */ glm::mat4 mrspiny = glm::rotate(identity, sunSpinAngle , yaxis);// the orbit angle also does the spin
	return  mrspiny;// 3 * 2 * 1
}

glm::mat4
MakeMercuryMatrix()
{
	const float mercurySpinAngle = ONE_FULL_TURN * Time * TimeScale / MERCURY_SPIN_TIME_SECONDS;
	const float mercuryOrbitAngle = ONE_FULL_TURN * Time * TimeScale / MERCURY_ORBIT_TIME_SECONDS;
	const glm::mat4 identity = glm::mat4(1.);
	const glm::vec3 yaxis = glm::vec3(0., 1., 0.);
	const glm::vec3 zaxis = glm::vec3(0., 0., 1.);
	// note: because the earth is tilted, we cannot play the translate+rotate trick -- the tilt must always be in the same direction// instead we must translate the earth to where it really belongs:
	glm::vec3 morbitPosition = glm::vec3(MERCURY_ORBITAL_RADIUS_MILES * cosf(mercuryOrbitAngle), 0., -MERCURY_ORBITAL_RADIUS_MILES * sinf(mercuryOrbitAngle));
	/* 3. */ glm::mat4 mtransx = glm::translate(identity, morbitPosition);
	/* 2. */ glm::mat4 mtilt = glm::rotate(identity, glm::radians(MERCURY_TILT_ANGLE_DEG), zaxis);
	/* 1. */ glm::mat4 mrspiny = glm::rotate(identity, mercurySpinAngle - mercuryOrbitAngle, yaxis);// the orbit angle also does the spin
	return mtransx * mtilt * mrspiny;// 3 * 2 * 1
}

glm::mat4
MakeVenusMatrix()
{
	const float venusSpinAngle = ONE_FULL_TURN * Time * TimeScale / VENUS_SPIN_TIME_SECONDS;
	const float venusOrbitAngle = ONE_FULL_TURN * Time * TimeScale / VENUS_ORBIT_TIME_SECONDS;
	const glm::mat4 identity = glm::mat4(1.);
	const glm::vec3 yaxis = glm::vec3(0., 1., 0.);
	const glm::vec3 zaxis = glm::vec3(0., 0., 1.);
	// note: because the earth is tilted, we cannot play the translate+rotate trick -- the tilt must always be in the same direction// instead we must translate the earth to where it really belongs:
	glm::vec3 vorbitPosition = glm::vec3(VENUS_ORBITAL_RADIUS_MILES * cosf(venusOrbitAngle), 0., -VENUS_ORBITAL_RADIUS_MILES * sinf(venusOrbitAngle));
	/* 3. */ glm::mat4 vtransx = glm::translate(identity, vorbitPosition);
	/* 2. */ glm::mat4 vtilt = glm::rotate(identity, glm::radians(VENUS_TILT_ANGLE_DEG), zaxis);
	/* 1. */ glm::mat4 vrspiny = glm::rotate(identity, venusSpinAngle - venusOrbitAngle, -yaxis);// the orbit angle also does the spin
	return vtransx * vtilt * vrspiny;// 3 * 2 * 1
}

glm::mat4
MakeEarthMatrix()
{
	const float earthSpinAngle = ONE_FULL_TURN * Time * TimeScale / EARTH_SPIN_TIME_SECONDS;
	const float earthOrbitAngle = ONE_FULL_TURN * Time * TimeScale / EARTH_ORBIT_TIME_SECONDS;
	const glm::mat4 identity = glm::mat4(1.);
	const glm::vec3 yaxis = glm::vec3(0., 1., 0.);
	const glm::vec3 zaxis = glm::vec3(0., 0., 1.);
	// note: because the earth is tilted, we cannot play the translate+rotate trick -- the tilt must always be in the same direction// instead we must translate the earth to where it really belongs:
	glm::vec3 eorbitPosition = glm::vec3(EARTH_ORBITAL_RADIUS_MILES * cosf(earthOrbitAngle), 0., -EARTH_ORBITAL_RADIUS_MILES * sinf(earthOrbitAngle)); 
	/* 3. */ glm::mat4 etransx = glm::translate(identity, eorbitPosition);
	/* 2. */ glm::mat4 etilt = glm::rotate(identity, glm::radians(EARTH_TILT_ANGLE_DEG), zaxis);
	/* 1. */ glm::mat4 erspiny = glm::rotate(identity, earthSpinAngle - earthOrbitAngle, yaxis);// the orbit angle also does the spin
	return etransx * etilt * erspiny;// 3 * 2 * 1
}

glm::mat4
MakeMoonMatrix()
{
	float moonSpinAngle = ONE_FULL_TURN * Time * TimeScale / MOON_SPIN_TIME_SECONDS;
	float moonOrbitAngle = ONE_FULL_TURN * Time * TimeScale / MOON_ORBIT_TIME_SECONDS;
	float earthOrbitAngle = ONE_FULL_TURN * Time * TimeScale / EARTH_ORBIT_TIME_SECONDS;
	glm::mat4 identity = glm::mat4(1.);
	glm::vec3 yaxis = glm::vec3(0., 1., 0.);
	glm::vec3 zaxis = glm::vec3(0., 0., 1.);
	/* 6. */ glm::mat4 erorbity = glm::rotate(identity, earthOrbitAngle, yaxis);
	/* 5. */ glm::mat4 etransx = glm::translate(identity, glm::vec3(EARTH_ORBITAL_RADIUS_MILES, 0., 0.));
	/* 4. */ glm::mat4 mrinclinez = glm::rotate(identity, glm::radians(MOON_ORBITAL_INCLINATION_DEG), zaxis);
	/* 3. */ glm::mat4 mrorbity = glm::rotate(identity, moonOrbitAngle, yaxis);
	/* 2. */ glm::mat4 mtransx = glm::translate(identity, glm::vec3(MOON_ORBITAL_RADIUS_MILES, 0., 0.));
	/* 1. */ glm::mat4 mrspiny = glm::rotate(identity, moonSpinAngle - moonOrbitAngle, yaxis); // the orbit angle also does the spin
	return erorbity * etransx * mrinclinez * mrorbity * mtransx * mrspiny; // 6 * 5 * 4 * 3 * 2 * 1
}

glm::mat4
MakeMarsMatrix()
{
	const float marsSpinAngle = ONE_FULL_TURN * Time * TimeScale / MARS_SPIN_TIME_SECONDS;
	const float marsOrbitAngle = ONE_FULL_TURN * Time * TimeScale / MARS_ORBIT_TIME_SECONDS;
	const glm::mat4 identity = glm::mat4(1.);
	const glm::vec3 yaxis = glm::vec3(0., 1., 0.);
	const glm::vec3 zaxis = glm::vec3(0., 0., 1.);
	glm::vec3 morbitPosition = glm::vec3(MARS_ORBITAL_RADIUS_MILES * cosf(marsOrbitAngle), 0., -MARS_ORBITAL_RADIUS_MILES * sinf(marsOrbitAngle));
	glm::mat4 mtransx = glm::translate(identity, morbitPosition);
	glm::mat4 mtilt = glm::rotate(identity, glm::radians(MARS_TILT_ANGLE_DEG), zaxis);
	glm::mat4 mrspiny = glm::rotate(identity, marsSpinAngle - marsOrbitAngle, yaxis);
	return mtransx * mtilt * mrspiny;// 3 * 2 * 1
}

glm::mat4
MakeJupiterMatrix()
{
	const float jupiterSpinAngle = ONE_FULL_TURN * Time * TimeScale / JUPITER_SPIN_TIME_SECONDS;
	const float jupiterOrbitAngle = ONE_FULL_TURN * Time * TimeScale / JUPITER_ORBIT_TIME_SECONDS;
	const glm::mat4 identity = glm::mat4(1.);
	const glm::vec3 yaxis = glm::vec3(0., 1., 0.);
	const glm::vec3 zaxis = glm::vec3(0., 0., 1.);
	// note: because the earth is tilted, we cannot play the translate+rotate trick -- the tilt must always be in the same direction// instead we must translate the earth to where it really belongs:
	glm::vec3 jorbitPosition = glm::vec3(JUPITER_ORBITAL_RADIUS_MILES * cosf(jupiterOrbitAngle), 0., -JUPITER_ORBITAL_RADIUS_MILES * sinf(jupiterOrbitAngle));
	/* 3. */ glm::mat4 jtransx = glm::translate(identity, jorbitPosition);
	/* 2. */ glm::mat4 jtilt = glm::rotate(identity, glm::radians(JUPITER_TILT_ANGLE_DEG), zaxis);
	/* 1. */ glm::mat4 jrspiny = glm::rotate(identity, jupiterSpinAngle - jupiterOrbitAngle, yaxis);// the orbit angle also does the spin
	return jtransx * jtilt * jrspiny;// 3 * 2 * 1
}

glm::mat4
MakeSaturnMatrix()
{
	const float saturnSpinAngle = ONE_FULL_TURN * Time * TimeScale / SATURN_SPIN_TIME_SECONDS;
	const float saturnOrbitAngle = ONE_FULL_TURN * Time * TimeScale / SATURN_ORBIT_TIME_SECONDS;
	const glm::mat4 identity = glm::mat4(1.);
	const glm::vec3 yaxis = glm::vec3(0., 1., 0.);
	const glm::vec3 zaxis = glm::vec3(0., 0., 1.);
	// note: because the earth is tilted, we cannot play the translate+rotate trick -- the tilt must always be in the same direction// instead we must translate the earth to where it really belongs:
	glm::vec3 sorbitPosition = glm::vec3(SATURN_ORBITAL_RADIUS_MILES * cosf(saturnOrbitAngle), 0., -SATURN_ORBITAL_RADIUS_MILES * sinf(saturnOrbitAngle));
	/* 3. */ glm::mat4 stransx = glm::translate(identity, sorbitPosition);
	/* 2. */ glm::mat4 stilt = glm::rotate(identity, glm::radians(SATURN_TILT_ANGLE_DEG), zaxis);
	/* 1. */ glm::mat4 srspiny = glm::rotate(identity, saturnSpinAngle - saturnOrbitAngle, yaxis);// the orbit angle also does the spin
	return stransx * stilt * srspiny;// 3 * 2 * 1
}

glm::mat4
MakeSaturn_RingMatrix()
{
	const float saturnSpinAngle = ONE_FULL_TURN * Time * TimeScale / SATURN_SPIN_TIME_SECONDS;
	const float saturnOrbitAngle = ONE_FULL_TURN * Time * TimeScale / SATURN_ORBIT_TIME_SECONDS;
	const glm::mat4 identity = glm::mat4(1.);
	const glm::vec3 yaxis = glm::vec3(0., 1., 0.);
	const glm::vec3 zaxis = glm::vec3(0., 0., 1.);
	// note: because the earth is tilted, we cannot play the translate+rotate trick -- the tilt must always be in the same direction// instead we must translate the earth to where it really belongs:
	glm::vec3 sorbitPosition = glm::vec3(SATURN_ORBITAL_RADIUS_MILES * cosf(saturnOrbitAngle), 0., -SATURN_ORBITAL_RADIUS_MILES * sinf(saturnOrbitAngle));
	/* 3. */ glm::mat4 stransx = glm::translate(identity, sorbitPosition);
	/* 2. */ glm::mat4 stilt = glm::rotate(identity, glm::radians(SATURN_TILT_ANGLE_DEG), zaxis);
	/* 1. */ glm::mat4 srspiny = glm::rotate(identity, saturnSpinAngle - saturnOrbitAngle, yaxis);// the orbit angle also does the spin
	return stransx * stilt *srspiny;// 3 * 2 * 1
}

glm::mat4
MakeUranusMatrix()
{
	const float uranusSpinAngle = ONE_FULL_TURN * Time * TimeScale / URANUS_SPIN_TIME_SECONDS;
	const float uranusOrbitAngle = ONE_FULL_TURN * Time * TimeScale / URANUS_ORBIT_TIME_SECONDS;
	const glm::mat4 identity = glm::mat4(1.);
	const glm::vec3 yaxis = glm::vec3(0., 1., 0.);
	const glm::vec3 zaxis = glm::vec3(0., 0., 1.);
	// note: because the earth is tilted, we cannot play the translate+rotate trick -- the tilt must always be in the same direction// instead we must translate the earth to where it really belongs:
	glm::vec3 uorbitPosition = glm::vec3(URANUS_ORBITAL_RADIUS_MILES * cosf(uranusOrbitAngle), 0., -URANUS_ORBITAL_RADIUS_MILES * sinf(uranusOrbitAngle));
	/* 3. */ glm::mat4 utransx = glm::translate(identity, uorbitPosition);
	/* 2. */ glm::mat4 utilt = glm::rotate(identity, glm::radians(URANUS_TILT_ANGLE_DEG), zaxis);
	/* 1. */ glm::mat4 urspiny = glm::rotate(identity, uranusSpinAngle - uranusOrbitAngle, yaxis);// the orbit angle also does the spin
	return utransx * utilt * urspiny;// 3 * 2 * 1
}

glm::mat4
MakeNeptuneMatrix()
{
	const float neptuneSpinAngle = ONE_FULL_TURN * Time * TimeScale / NEPTUNE_SPIN_TIME_SECONDS;
	const float neptuneOrbitAngle = ONE_FULL_TURN * Time * TimeScale / NEPTUNE_ORBIT_TIME_SECONDS;
	const glm::mat4 identity = glm::mat4(1.);
	const glm::vec3 yaxis = glm::vec3(0., 1., 0.);
	const glm::vec3 zaxis = glm::vec3(0., 0., 1.);
	// note: because the earth is tilted, we cannot play the translate+rotate trick -- the tilt must always be in the same direction// instead we must translate the earth to where it really belongs:
	glm::vec3 norbitPosition = glm::vec3(NEPTUNE_ORBITAL_RADIUS_MILES * cosf(neptuneOrbitAngle), 0., -NEPTUNE_ORBITAL_RADIUS_MILES * sinf(neptuneOrbitAngle));
	/* 3. */ glm::mat4 ntransx = glm::translate(identity, norbitPosition);
	/* 2. */ glm::mat4 ntilt = glm::rotate(identity, glm::radians(NEPTUNE_TILT_ANGLE_DEG), zaxis);
	/* 1. */ glm::mat4 nrspiny = glm::rotate(identity, neptuneSpinAngle - neptuneOrbitAngle, yaxis);// the orbit angle also does the spin
	return ntransx * ntilt * nrspiny;// 3 * 2 * 1
}


float *
Array3( float a, float b, float c )
{
	static float array[4];

	array[0] = a;
	array[1] = b;
	array[2] = c;
	array[3] = 1.;
	return array;
}

// utility to create an array from a multiplier and an array:

float *
MulArray3( float factor, float array0[ ] )
{
	static float array[4];

	array[0] = factor * array0[0];
	array[1] = factor * array0[1];
	array[2] = factor * array0[2];
	array[3] = 1.;
	return array;
}


float *
MulArray3(float factor, float a, float b, float c )
{
	static float array[4];

	float* abc = Array3(a, b, c);
	array[0] = factor * abc[0];
	array[1] = factor * abc[1];
	array[2] = factor * abc[2];
	array[3] = 1.;
	return array;
}

// these are here for when you need them -- just uncomment the ones you need:

#include "setmaterial.cpp"
#include "setlight.cpp"
#include "osusphere.cpp"
#include "osucone.cpp"
#include "osutorus.cpp"
#include "bmptotexture.cpp"
#include "loadobjfile.cpp"
#include "keytime.cpp"
#include "glslprogram.cpp"


// main program:

int
main( int argc, char *argv[ ] )
{
	// turn on the glut package:
	// (do this before checking argc and argv since glutInit might
	// pull some command line arguments out)

	glutInit( &argc, argv );

	// setup all the graphics stuff:

	InitGraphics( );

	// create the display lists that **will not change**:

	InitLists( );

	// init all the global variables used by Display( ):
	// this will also post a redisplay

	Reset( );

	// setup all the user interface stuff:

	InitMenus( );

	// draw the scene once and wait for some interaction:
	// (this will never return)

	glutSetWindow( MainWindow );
	glutMainLoop( );

	// glutMainLoop( ) never actually returns
	// the following line is here to make the compiler happy:

	return 0;
}


// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutPostRedisplay( ) do it

void
Animate( )
{
	// put animation stuff in here -- change some global variables for Display( ) to find:

	int ms = glutGet(GLUT_ELAPSED_TIME); // milliseconds
	ms %= MAXIMUM_TIME_MILLISECONDS; // [ 0, MAXIMUM_TIME_MILLISECONDS-1 ]
	Time = (float)ms / 1000.f; // seconds

	// for example, if you wanted to spin an object in Display( ), you might call: glRotatef( 360.f*Time,   0., 1., 0. );

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// draw the complete scene:

void
Display( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting Display.\n");

	// set which window we want to do the graphics into:
	glutSetWindow( MainWindow );

	// erase the background:
	glDrawBuffer( GL_BACK );
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	glEnable( GL_DEPTH_TEST );
#ifdef DEMO_DEPTH_BUFFER
	if( DepthBufferOn == 0 )
		glDisable( GL_DEPTH_TEST );
#endif


	// specify shading to be flat:

	glShadeModel( GL_FLAT );

	// set the viewport to be a square centered in the window:

	GLsizei vx = glutGet( GLUT_WINDOW_WIDTH );
	GLsizei vy = glutGet( GLUT_WINDOW_HEIGHT );
	GLsizei v = vx < vy ? vx : vy;			// minimum dimension
	GLint xl = ( vx - v ) / 2;
	GLint yb = ( vy - v ) / 2;
	glViewport( xl, yb,  v, v );


	// set the viewing volume:
	// remember that the Z clipping  values are given as DISTANCES IN FRONT OF THE EYE
	// USE gluOrtho2D( ) IF YOU ARE DOING 2D !

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	if( NowProjection == ORTHO )
		glOrtho( -2.f, 2.f,     -2.f, 2.f,     0.1f, 1000.f );
	else
		gluPerspective( 70.f, 1.f,	0.1f, 1000.f );

	// place the objects into the scene:

	glm::mat4 sun = MakeSunMatrix();
	glm::mat4 m = MakeMercuryMatrix();
	glm::mat4 ve = MakeVenusMatrix();
	glm::mat4 e = MakeEarthMatrix();
	glm::mat4 mars = MakeMarsMatrix();
	glm::mat4 jup = MakeJupiterMatrix();
	glm::mat4 sat = MakeSaturnMatrix();
	glm::mat4 sat_r = MakeSaturn_RingMatrix();
	glm::mat4 u = MakeUranusMatrix();
	glm::mat4 n = MakeNeptuneMatrix();
	glm::mat4 moon = MakeMoonMatrix();

	glm::vec4 eyePos = glm::vec4(0., 0., 0., 1.); // a position
	glm::vec4 lookPos = glm::vec4(0., 0., 0., 1.); // a position
	glm::vec4 upVec = glm::vec4(0., 0., 0., 0.); // a vector, so doesn¡¦t get translations applied
	float znear = 0.1f;

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	// set the eye position, look-at position, and up-vector:
	//gluLookAt(8.f, 4.f, 15.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);

	switch (NowView)
	{
	case OUTSIDEVIEW1: // 1st way to set gluLookAt( )
		gluLookAt(2.f, 2.f, 5.f, 1.f, 0.f, 2.f, 0.f, 1.f, 0.f);
		glRotatef((GLfloat)Yrot, 0., 1., 0.);
		glRotatef((GLfloat)Xrot, 1., 0., 0.);
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		glScalef(Scale, Scale, Scale);
		break;

	case OUTSIDEVIEW2:
		gluLookAt(35.f, 2.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
		glRotatef((GLfloat)Yrot, 0., 1., 0.);
		glRotatef((GLfloat)Xrot, 1., 0., 0.);
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		glScalef(Scale, Scale, Scale);
		break;

	case OUTSIDEVIEW3:
		gluLookAt(2.f, 35.f, 0.f, 1.f, 0.f, 2.f, 0.f, 1.f, 0.f);
		glRotatef((GLfloat)Yrot, 0., 1., 0.);
		glRotatef((GLfloat)Xrot, 1., 0., 0.);
		if (Scale < MINSCALE)
			Scale = MINSCALE;
		glScalef(Scale, Scale, Scale);
		break;

	case EARTHVIEW: 
		SetViewingFromLatLng(0.f, 120.f, 0.f, 110.f, EARTH_RADIUS_MILES, &eyePos, &lookPos, &upVec, &znear);
		eyePos = e * eyePos;
		lookPos = e * lookPos;
		upVec = e * upVec;
		gluLookAt(eyePos.x, eyePos.y, eyePos.z, lookPos.x, lookPos.y, lookPos.z, upVec.x, upVec.y, upVec.z);
		break;

	case MERVIEW:
		SetViewingFromLatLng(0, 30.f, 20, 0.f, MERCURY_RADIUS_MILES, &eyePos, &lookPos, &upVec, &znear);
		eyePos = m * eyePos;
		lookPos = m * lookPos;
		upVec = m * upVec;
		gluLookAt(eyePos.x, eyePos.y, eyePos.z, lookPos.x, lookPos.y, lookPos.z, upVec.x, upVec.y, upVec.z);
		break;

	case MARSVIEW:
		SetViewingFromLatLng(0.f, 60.f, 0.f, 70.f, MARS_RADIUS_MILES, &eyePos, &lookPos, &upVec, &znear);
		eyePos = mars * eyePos;
		lookPos = mars * lookPos;
		upVec = mars * upVec;
		gluLookAt(eyePos.x, eyePos.y, eyePos.z, lookPos.x, lookPos.y, lookPos.z, upVec.x, upVec.y, upVec.z);
		break;

	case SVIEW:
		SetViewingFromLatLng(0.f, 70.f, 0.f, 60.f, EARTH_RADIUS_MILES * 9.45, &eyePos, &lookPos, &upVec, &znear);
		eyePos = sat * eyePos;
		lookPos = sat * lookPos;
		upVec = sat * upVec;
		gluLookAt(eyePos.x, eyePos.y, eyePos.z, lookPos.x, lookPos.y, lookPos.z, upVec.x, upVec.y, upVec.z);
		break;

	case MOONVIEW: 
		SetViewingFromLatLng(0.f, 175.f, 0.f, 170.f, MOON_RADIUS_MILES, &eyePos, &lookPos, &upVec, &znear);
		eyePos = moon * eyePos;
		lookPos = moon * lookPos;
		upVec = moon * upVec;
		gluLookAt(eyePos.x, eyePos.y, eyePos.z, lookPos.x, lookPos.y, lookPos.z, upVec.x, upVec.y, upVec.z);
		break;
	}

	/*
	if (NowMode == 0)
		gluLookAt(2.f, 2.f, 5.f, 1.f, 0.f, 2.f, 0.f, 1.f, 0.f);
	else if (NowMode == 1)
		gluLookAt(35.f, 2.f, 0.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);
	else if (NowMode == 2)
		gluLookAt(2.f, 35.f, 0.f, 1.f, 0.f, 2.f, 0.f, 1.f, 0.f);
	*/
	// rotate the scene:

	//glRotatef( (GLfloat)Yrot, 0.f, 1.f, 0.f );
	//glRotatef( (GLfloat)Xrot, 1.f, 0.f, 0.f );

	//float xlight = 5 * sin(9 * (2. * F_PI * Time));
	//float ylight = 5 * sin(4 * (7. * F_PI * Time));
	//float zlight = 9 * sin(9 * (2. * F_PI * Time));

	SetPointLight(GL_LIGHT0, SUN_RADIUS_MILES, 0, SUN_RADIUS_MILES, 1., 1., 1.);
	SetPointLight(GL_LIGHT1, SUN_RADIUS_MILES, 0, 0, 1., 1., 1.);
	SetPointLight(GL_LIGHT2, 0, 0, SUN_RADIUS_MILES, 1., 1., 1.);
	SetPointLight(GL_LIGHT3, -SUN_RADIUS_MILES, 0, -SUN_RADIUS_MILES, 1., 1., 1.);
	SetPointLight(GL_LIGHT4, -SUN_RADIUS_MILES, 0, 0, 1., 1., 1.);
	SetPointLight(GL_LIGHT5, 0, 0, -SUN_RADIUS_MILES, 1., 1., 1.);
	SetPointLight(GL_LIGHT6, -SUN_RADIUS_MILES, 0, SUN_RADIUS_MILES, 1., 1., 1.);

	// uniformly scale the scene:

	//if( Scale < MINSCALE )
	//	Scale = MINSCALE;
	//glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );

	// set the fog parameters:

	if( DepthCueOn != 0 )
	{
		glFogi( GL_FOG_MODE, FOGMODE );
		glFogfv( GL_FOG_COLOR, FOGCOLOR );
		glFogf( GL_FOG_DENSITY, FOGDENSITY );
		glFogf( GL_FOG_START, FOGSTART );
		glFogf( GL_FOG_END, FOGEND );
		glEnable( GL_FOG );
	}
	else
	{
		glDisable( GL_FOG );
	}

	// possibly draw the axes:

	if( AxesOn != 0 )
	{
		glColor3fv( &Colors[NowColor][0] );
		glCallList( AxesList );
	}

	// since we are using glScalef( ), be sure the normals get unitized:

	glEnable( GL_NORMALIZE );

	// draw the box object by calling up its display list:
	

	glEnable( GL_TEXTURE_2D );
	//else if (NowMode == 1)
	//	glDisable( GL_TEXTURE_2D );

	glEnable( GL_LIGHTING );
	glEnable( GL_LIGHT0 );
	glEnable( GL_LIGHT1 );
	glEnable( GL_LIGHT2 );
	glEnable( GL_LIGHT3 );
	glEnable( GL_LIGHT4 );
	glEnable( GL_LIGHT5 );
	glEnable( GL_LIGHT6 );
	glLightfv( GL_LIGHT0, GL_AMBIENT, Array3( 1., 1., 1. ) );
	
	/*
	if (NowMode == 0)
		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
	else if (NowMode == 1)
		glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
	*/

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glCallList(StarDL);

	glPushMatrix();
		glMultMatrixf(glm::value_ptr(sun));
		glCallList(SunDL);
	glPopMatrix();
	// 0.4 0.7 1 1.5 5.2 9.5 19.2 30.1

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glPushMatrix();
		glMultMatrixf(glm::value_ptr(m));
		glCallList(MercuryDL);
	glPopMatrix();

	glPushMatrix();
		glMultMatrixf(glm::value_ptr(ve));
		glCallList(VenusDL);
	glPopMatrix();

	glPushMatrix();
		glMultMatrixf(glm::value_ptr(e));
		glCallList(EarthDL);
	glPopMatrix();

	glPushMatrix();
		glMultMatrixf(glm::value_ptr(moon));
		glCallList(MoonDL);
	glPopMatrix();

	glPushMatrix();
		glMultMatrixf(glm::value_ptr(mars));
		glCallList(MarsDL);
	glPopMatrix();

	glPushMatrix();
		glMultMatrixf(glm::value_ptr(jup));
		glCallList(JupiterDL);
	glPopMatrix();

	glPushMatrix();
		glMultMatrixf(glm::value_ptr(sat));
		glCallList(SaturnDL);
	glPopMatrix();

	
	glPushMatrix();
		glMultMatrixf(glm::value_ptr(sat_r));
		glCallList(S_RingDL);
	glPopMatrix();
	

	glPushMatrix();
		glMultMatrixf(glm::value_ptr(u));
		glCallList(UranusDL);
	glPopMatrix();

	glPushMatrix();
		glMultMatrixf(glm::value_ptr(n));
		glCallList(NeptuneDL);
	glPopMatrix();

	
	glDisable( GL_TEXTURE_2D );
	glDisable( GL_LIGHTING );

	glCallList(MorbitDL);
	glCallList(VorbitDL);
	glCallList(EorbitDL);
	glCallList(MarsorbitDL);
	glCallList(JorbitDL);
	glCallList(SorbitDL);
	glCallList(UorbitDL);
	glCallList(NorbitDL);


#ifdef DEMO_Z_FIGHTING
	if( DepthFightingOn != 0 )
	{
		glPushMatrix( );
			glRotatef( 90.f,   0.f, 1.f, 0.f );
			glCallList( BoxList );
		glPopMatrix( );
	}
#endif


	// draw some gratuitous text that just rotates on top of the scene:
	// i commented out the actual text-drawing calls -- put them back in if you have a use for them
	// a good use for thefirst one might be to have your name on the screen
	// a good use for the second one might be to have vertex numbers on the screen alongside each vertex

	glDisable( GL_DEPTH_TEST );
	glColor3f( 1.f, 1.f, 0.f );
	//DoRasterString( 0.f, 1.f, 0.f, (char *)"Roger System" );


	// draw some gratuitous text that is fixed on the screen:
	//
	// the projection matrix is reset to define a scene whose
	// world coordinate system goes from 0-100 in each axis
	//
	// this is called "percent units", and is just a convenience
	//
	// the modelview matrix is reset to identity as we don't
	// want to transform these coordinates

	glDisable( GL_DEPTH_TEST );
	glMatrixMode( GL_PROJECTION );
	glLoadIdentity( );
	gluOrtho2D( 0.f, 100.f,     0.f, 100.f );
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );
	glColor3f( 1.f, 1.f, 1.f );
	//DoRasterString( 5.f, 5.f, 0.f, (char *)"Text That Doesn't" );

	// swap the double-buffered framebuffers:

	glutSwapBuffers( );

	// be sure the graphics buffer has been sent:
	// note: be sure to use glFlush( ) here, not glFinish( ) !

	glFlush( );
}


void
DoAxesMenu( int id )
{
	AxesOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoColorMenu( int id )
{
	NowColor = id - RED;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDebugMenu( int id )
{
	DebugOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthBufferMenu( int id )
{
	DepthBufferOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthFightingMenu( int id )
{
	DepthFightingOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoDepthMenu( int id )
{
	DepthCueOn = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// main menu callback:

void
DoMainMenu( int id )
{
	switch( id )
	{
		case RESET:
			Reset( );
			break;

		case QUIT:
			// gracefully close out the graphics:
			// gracefully close the graphics window:
			// gracefully exit the program:
			glutSetWindow( MainWindow );
			glFinish( );
			glutDestroyWindow( MainWindow );
			exit( 0 );
			break;

		default:
			fprintf( stderr, "Don't know what to do with Main Menu ID %d\n", id );
	}

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


void
DoProjectMenu( int id )
{
	NowProjection = id;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// use glut to display a string of characters using a raster font:

void
DoRasterString( float x, float y, float z, char *s )
{
	glRasterPos3f( (GLfloat)x, (GLfloat)y, (GLfloat)z );

	char c;			// one character to print
	for( ; ( c = *s ) != '\0'; s++ )
	{
		glutBitmapCharacter( GLUT_BITMAP_TIMES_ROMAN_24, c );
	}
}


// use glut to display a string of characters using a stroke font:

void
DoStrokeString( float x, float y, float z, float ht, char *s )
{
	glPushMatrix( );
		glTranslatef( (GLfloat)x, (GLfloat)y, (GLfloat)z );
		float sf = ht / ( 119.05f + 33.33f );
		glScalef( (GLfloat)sf, (GLfloat)sf, (GLfloat)sf );
		char c;			// one character to print
		for( ; ( c = *s ) != '\0'; s++ )
		{
			glutStrokeCharacter( GLUT_STROKE_ROMAN, c );
		}
	glPopMatrix( );
}


// return the number of seconds since the start of the program:

float
ElapsedSeconds( )
{
	// get # of milliseconds since the start of the program:

	int ms = glutGet( GLUT_ELAPSED_TIME );

	// convert it to seconds:

	return (float)ms / 1000.f;
}


// initialize the glui window:

void
InitMenus( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitMenus.\n");

	glutSetWindow( MainWindow );

	int numColors = sizeof( Colors ) / ( 3*sizeof(float) );
	int colormenu = glutCreateMenu( DoColorMenu );
	for( int i = 0; i < numColors; i++ )
	{
		glutAddMenuEntry( ColorNames[i], i );
	}

	int axesmenu = glutCreateMenu( DoAxesMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthcuemenu = glutCreateMenu( DoDepthMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthbuffermenu = glutCreateMenu( DoDepthBufferMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int depthfightingmenu = glutCreateMenu( DoDepthFightingMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int debugmenu = glutCreateMenu( DoDebugMenu );
	glutAddMenuEntry( "Off",  0 );
	glutAddMenuEntry( "On",   1 );

	int projmenu = glutCreateMenu( DoProjectMenu );
	glutAddMenuEntry( "Orthographic",  ORTHO );
	glutAddMenuEntry( "Perspective",   PERSP );

	int mainmenu = glutCreateMenu( DoMainMenu );
	glutAddSubMenu(   "Axes",          axesmenu);
	glutAddSubMenu(   "Axis Colors",   colormenu);

#ifdef DEMO_DEPTH_BUFFER
	glutAddSubMenu(   "Depth Buffer",  depthbuffermenu);
#endif

#ifdef DEMO_Z_FIGHTING
	glutAddSubMenu(   "Depth Fighting",depthfightingmenu);
#endif

	glutAddSubMenu(   "Depth Cue",     depthcuemenu);
	glutAddSubMenu(   "Projection",    projmenu );
	glutAddMenuEntry( "Reset",         RESET );
	glutAddSubMenu(   "Debug",         debugmenu);
	glutAddMenuEntry( "Quit",          QUIT );

// attach the pop-up menu to the right mouse button:

	glutAttachMenu( GLUT_RIGHT_BUTTON );
}



// initialize the glut and OpenGL libraries:
//	also setup callback functions

void
InitGraphics( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitGraphics.\n");

	// request the display modes:
	// ask for red-green-blue-alpha color, double-buffering, and z-buffering:

	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH );

	// set the initial window configuration:

	glutInitWindowPosition( 0, 0 );
	glutInitWindowSize( INIT_WINDOW_SIZE, INIT_WINDOW_SIZE );

	// open the window and set its title:

	MainWindow = glutCreateWindow( WINDOWTITLE );
	glutSetWindowTitle( WINDOWTITLE );

	// set the framebuffer clear values:

	glClearColor( BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3] );

	// setup the callback functions:
	// DisplayFunc -- redraw the window
	// ReshapeFunc -- handle the user resizing the window
	// KeyboardFunc -- handle a keyboard input
	// MouseFunc -- handle the mouse button going down or up
	// MotionFunc -- handle the mouse moving with a button down
	// PassiveMotionFunc -- handle the mouse moving with a button up
	// VisibilityFunc -- handle a change in window visibility
	// EntryFunc	-- handle the cursor entering or leaving the window
	// SpecialFunc -- handle special keys on the keyboard
	// SpaceballMotionFunc -- handle spaceball translation
	// SpaceballRotateFunc -- handle spaceball rotation
	// SpaceballButtonFunc -- handle spaceball button hits
	// ButtonBoxFunc -- handle button box hits
	// DialsFunc -- handle dial rotations
	// TabletMotionFunc -- handle digitizing tablet motion
	// TabletButtonFunc -- handle digitizing tablet button hits
	// MenuStateFunc -- declare when a pop-up menu is in use
	// TimerFunc -- trigger something to happen a certain time from now
	// IdleFunc -- what to do when nothing else is going on

	glutSetWindow( MainWindow );
	glutDisplayFunc( Display );
	glutReshapeFunc( Resize );
	glutKeyboardFunc( Keyboard );
	glutMouseFunc( MouseButton );
	glutMotionFunc( MouseMotion );
	glutPassiveMotionFunc(MouseMotion);
	//glutPassiveMotionFunc( NULL );
	glutVisibilityFunc( Visibility );
	glutEntryFunc( NULL );
	glutSpecialFunc( NULL );
	glutSpaceballMotionFunc( NULL );
	glutSpaceballRotateFunc( NULL );
	glutSpaceballButtonFunc( NULL );
	glutButtonBoxFunc( NULL );
	glutDialsFunc( NULL );
	glutTabletMotionFunc( NULL );
	glutTabletButtonFunc( NULL );
	glutMenuStateFunc( NULL );
	glutTimerFunc( -1, NULL, 0 );

	// setup glut to call Animate( ) every time it has
	// 	nothing it needs to respond to (which is most of the time)
	// we don't need to do this for this program, and really should set the argument to NULL
	// but, this sets us up nicely for doing animation

	glutIdleFunc( Animate );

	// init the glew package (a window must be open to do this):

#ifdef WIN32
	GLenum err = glewInit( );
	if( err != GLEW_OK )
	{
		fprintf( stderr, "glewInit Error\n" );
	}
	else
		fprintf( stderr, "GLEW initialized OK\n" );
	fprintf( stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	// all other setups go here, such as GLSLProgram and KeyTime setups:
	int widthu, heightu, width0, height0, widthM, heightM, width, height, width1, height1, width2, height2, width3, height3, width4, height4, width5, height5, width6, height6;
	int widths_r, heights_r, widthmoon, heightmoon;

	char* fileu = (char*)"hipp8.bmp";
	char *file0 = (char*)"sun.bmp";
	char* fileM = (char*)"mercury.bmp";
	char *file = (char *)"mars.bmp";
	char *file1 = (char *)"earth.bmp";
	char *file2 = (char *)"venus.bmp";
	char *file3 = (char *)"jupiter.bmp";
	char *file4 = (char *)"saturn.bmp";
	char *file5 = (char *)"uranus.bmp";
	char *file6 = (char *)"2k_neptune.bmp";
	char* files_r = (char *)"Ring.bmp";
	char* file7 = (char*)"moon.bmp";
	
	/*
	char* fileu = (char*)"hipp8.bmp";
	char* file0 = (char*)"profile-yipeng-song.bmp";
	char* fileM = (char*)"profile-yipeng-song.bmp";
	char* file = (char*)"profile-yipeng-song.bmp";
	char* file1 = (char*)"profile-yipeng-song.bmp";
	char* file2 = (char*)"profile-yipeng-song.bmp";
	char* file3 = (char*)"profile-yipeng-song.bmp";
	char* file4 = (char*)"profile-yipeng-song.bmp";
	char* file5 = (char*)"profile-yipeng-song.bmp";
	char* file6 = (char*)"profile-yipeng-song.bmp";
	*/

	unsigned char* textureu = BmpToTexture(fileu, &widthu, &heightu);
	unsigned char* texture0 = BmpToTexture(file0, &width0, &height0);
	unsigned char* textureM = BmpToTexture(fileM, &widthM, &heightM);
	unsigned char *texture = BmpToTexture(file, &width, &height);
	unsigned char *texture1 = BmpToTexture(file1, &width1, &height1);
	unsigned char *texture2 = BmpToTexture(file2, &width2, &height2);
	unsigned char *texture3 = BmpToTexture(file3, &width3, &height3);
	unsigned char *texture4 = BmpToTexture(file4, &width4, &height4);
	unsigned char *texture5 = BmpToTexture(file5, &width5, &height5);
	unsigned char *texture6 = BmpToTexture(file6, &width6, &height6);
	unsigned char* texture7 = BmpToTexture(file7, &widthmoon, &heightmoon);
	unsigned char* textures_r = BmpToTexture(files_r, &widths_r, &heights_r);

	if( texture == NULL )
        	fprintf( stderr, "Cannot open texture '%s'\n", file);
	else
        	fprintf( stderr, "Opened '%s': width = %d ; height = %d\n", file, width, height );

	glGenTextures(1, &StarTex);
	glGenTextures(1, &SunTex);
	glGenTextures(1, &MercuryTex);
	glGenTextures( 1, &MarsTex );
	glGenTextures(1, &EarthTex);
	glGenTextures(1, &VenusTex);
	glGenTextures(1, &JupiterTex);
	glGenTextures(1, &SaturnTex);
	glGenTextures(1, &UranusTex);
	glGenTextures(1, &NeptuneTex);
	glGenTextures(1, &MoonTex);
	glGenTextures(1, &S_RingTex);

	glBindTexture(GL_TEXTURE_2D, StarTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, widthu, heightu, 0, GL_RGB, GL_UNSIGNED_BYTE, textureu);

	glBindTexture(GL_TEXTURE_2D, SunTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, width0, height0, 0, GL_RGB, GL_UNSIGNED_BYTE, texture0);

	glBindTexture(GL_TEXTURE_2D, MercuryTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, widthM, heightM, 0, GL_RGB, GL_UNSIGNED_BYTE, textureM);

	glBindTexture( GL_TEXTURE_2D, MarsTex );
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1 );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, 3, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture );

	glBindTexture( GL_TEXTURE_2D, EarthTex);
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, 3, width1, height1, 0, GL_RGB, GL_UNSIGNED_BYTE, texture1 );

	glBindTexture( GL_TEXTURE_2D, VenusTex);
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, 3, width2, height2, 0, GL_RGB, GL_UNSIGNED_BYTE, texture2 );

	glBindTexture( GL_TEXTURE_2D, JupiterTex);
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, 3, width3, height3, 0, GL_RGB, GL_UNSIGNED_BYTE, texture3 );

	glBindTexture( GL_TEXTURE_2D, SaturnTex);
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, 3, width4, height4, 0, GL_RGB, GL_UNSIGNED_BYTE, texture4 );

	glBindTexture( GL_TEXTURE_2D, UranusTex);
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, 3, width5, height5, 0, GL_RGB, GL_UNSIGNED_BYTE, texture5 );

	glBindTexture( GL_TEXTURE_2D, NeptuneTex);
	glPixelStorei( GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, 3, width6, height6, 0, GL_RGB, GL_UNSIGNED_BYTE, texture6 );

	glBindTexture(GL_TEXTURE_2D, S_RingTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, widths_r, heights_r, 0, GL_RGB, GL_UNSIGNED_BYTE, textures_r);

	glBindTexture(GL_TEXTURE_2D, MoonTex);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, widthmoon, heightmoon, 0, GL_RGB, GL_UNSIGNED_BYTE, texture7);

	//VenusTex, EarthTex, MarsTex, JupiterTex, SaturnTex, UranusTex, NeptuneTex
}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )

void
InitLists( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting InitLists.\n");

	float dx = BOXSIZE / 2.f;
	float dy = BOXSIZE / 2.f;
	float dz = BOXSIZE / 2.f;
	glutSetWindow( MainWindow );

	// create the object:

	SphereDL = glGenLists( 1 );
	glNewList( SphereDL, GL_COMPILE );
		OsuSphere(1., 200, 200);
	glEndList( );


	OrbitDL = glGenLists(1);
	glNewList( OrbitDL, GL_COMPILE );
		Circle(0., 0., 1., 50);
	glEndList( );

	MorbitDL = glGenLists(1);
	glNewList( MorbitDL, GL_COMPILE );

		Circle(0., 0., MERCURY_ORBITAL_RADIUS_MILES, 100);

	glEndList( );

	VorbitDL = glGenLists(1);
	glNewList( VorbitDL, GL_COMPILE );

		Circle(0., 0., VENUS_ORBITAL_RADIUS_MILES, 100);

	glEndList( );

	EorbitDL = glGenLists(1);
	glNewList( EorbitDL, GL_COMPILE );

		Circle(0., 0., EARTH_ORBITAL_RADIUS_MILES, 100);

	glEndList( );

	MarsorbitDL = glGenLists(1);
	glNewList( MarsorbitDL, GL_COMPILE );

		Circle(0., 0., MARS_ORBITAL_RADIUS_MILES, 100);

	glEndList( );

	JorbitDL = glGenLists(1);
	glNewList( JorbitDL, GL_COMPILE );

		Circle(0., 0., JUPITER_ORBITAL_RADIUS_MILES, 100);

	glEndList( );

	SorbitDL = glGenLists(1);
	glNewList( SorbitDL, GL_COMPILE );

		Circle(0., 0., SATURN_ORBITAL_RADIUS_MILES, 100);

	glEndList( );

	UorbitDL = glGenLists(1);
	glNewList( UorbitDL, GL_COMPILE );

		Circle(0., 0., URANUS_ORBITAL_RADIUS_MILES, 100);

	glEndList( );

	NorbitDL = glGenLists(1);
	glNewList( NorbitDL, GL_COMPILE );

		Circle(0., 0., NEPTUNE_ORBITAL_RADIUS_MILES, 100);

	glEndList( );

	// VenusTex, EarthTex, MarsTex, JupiterTex, SaturnTex, UranusTex, NeptuneTex;

	StarDL = glGenLists( 1 );
	glNewList(StarDL, GL_COMPILE );
		glBindTexture( GL_TEXTURE_2D, StarTex );
		glPushMatrix( );
			glScalef(50, 50, 50);
			glCallList( SphereDL );		
		glPopMatrix( );
	glEndList( );

	SunDL = glGenLists( 1 );
	glNewList(SunDL, GL_COMPILE );
		glBindTexture( GL_TEXTURE_2D, SunTex );
		glPushMatrix( );
			glScalef( 2, 2, 2 );
			//glScalef( SUN_RADIUS_MILES * 10, SUN_RADIUS_MILES * 10, SUN_RADIUS_MILES * 10);
			glCallList( SphereDL );		
		glPopMatrix( );
	glEndList( );

	//0.383
	MercuryDL = glGenLists( 1 );
	glNewList(MercuryDL, GL_COMPILE );
		glBindTexture( GL_TEXTURE_2D, MercuryTex );
		glPushMatrix( );
			glScalef(0.0383, 0.0383, 0.0383);
			glCallList( SphereDL );		
		glPopMatrix( );
	glEndList( );

	VenusDL = glGenLists( 1 );
	glNewList(VenusDL, GL_COMPILE );
		glBindTexture( GL_TEXTURE_2D, VenusTex );
		glPushMatrix( );
			glScalef( 0.095f, 0.095f, 0.095f );
			glCallList( SphereDL );		
		glPopMatrix( );
	glEndList( );

	EarthDL = glGenLists( 1 );
	glNewList( EarthDL, GL_COMPILE );
		glBindTexture( GL_TEXTURE_2D, EarthTex );	
		glPushMatrix( );
			glScalef( EARTH_RADIUS_MILES, EARTH_RADIUS_MILES, EARTH_RADIUS_MILES);
			glCallList( SphereDL );		
		glPopMatrix( );
	glEndList( );

	MoonDL = glGenLists( 1 );
	glNewList( MoonDL, GL_COMPILE );
		glBindTexture( GL_TEXTURE_2D, MoonTex );	
		glPushMatrix( );
			glScalef( MOON_RADIUS_MILES, MOON_RADIUS_MILES, MOON_RADIUS_MILES);
			glCallList( SphereDL );		
		glPopMatrix( );
	glEndList( );

	MarsDL = glGenLists( 1 );
	glNewList( MarsDL, GL_COMPILE );
		glBindTexture( GL_TEXTURE_2D, MarsTex );	// MarsTex must have already been created when this is called
		glPushMatrix( );
			glScalef( 0.053f, 0.053f, 0.053f );	// scale of mars sphere, from the table
			glCallList( SphereDL );		// a dl can call another dl that has been previously created
		glPopMatrix( );
	glEndList( );

	JupiterDL = glGenLists( 1 );
	glNewList(JupiterDL, GL_COMPILE );
		glBindTexture( GL_TEXTURE_2D, JupiterTex );
		glPushMatrix( );
			glScalef( 1.121f, 1.121f, 1.121f );
			glCallList( SphereDL );		
		glPopMatrix( );
	glEndList( );

	SaturnDL = glGenLists( 1 );
	glNewList(SaturnDL, GL_COMPILE );
		glBindTexture( GL_TEXTURE_2D, SaturnTex );
		glPushMatrix( );
			glScalef( 0.945f, 0.945f, 0.945f );
			glCallList( SphereDL );		
		glPopMatrix( );
	glEndList( );

	GLUquadricObj* p = gluNewQuadric();

	S_RingDL = glGenLists(1);
	glNewList(S_RingDL, GL_COMPILE);
		gluQuadricTexture(p, GL_TRUE);
		glBindTexture(GL_TEXTURE_2D, S_RingTex);
		glPushMatrix( );
			glNormal3f(1, 0, 0);
			glNormal3f(-1, 0, 0);
			glNormal3f(0, 1, 0);
			glNormal3f(0, -1, 0);
			glNormal3f(0, 0, 1);
			glNormal3f(0, 0, -1);
			glRotatef(90, 1, 0, 0);
			gluDisk( p , 46298 * PRS, 84991 * PRS, 100, 100);
		glPopMatrix();
	glEndList( );


	UranusDL = glGenLists( 1 );
	glNewList(UranusDL, GL_COMPILE );
		glBindTexture( GL_TEXTURE_2D, UranusTex );
		glPushMatrix( );
			glScalef( 0.401f, 0.401f, 0.401f );
			glCallList( SphereDL );		
		glPopMatrix( );
	glEndList( );

	NeptuneDL = glGenLists( 1 );
	glNewList(NeptuneDL, GL_COMPILE );
		glBindTexture( GL_TEXTURE_2D, NeptuneTex );
		glPushMatrix( );
			glScalef( 0.388f, 0.388f, 0.388f );
			glCallList( SphereDL );		
		glPopMatrix( );
	glEndList( );

	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 10.5 );
		glLineWidth( 1. );
	glEndList( );
}


// the keyboard callback:

void
Keyboard( unsigned char c, int x, int y )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Keyboard: '%c' (0x%0x)\n", c, c );

	switch( c )
	{
		case '1':
			NowView = OUTSIDEVIEW1;
			break;

		case '2':
			NowView = OUTSIDEVIEW2;
			break;

		case '3':
			NowView = OUTSIDEVIEW3;
			break;

		case 'p':
		case 'P':
			NowProjection = PERSP;
			break;

		case '+':
			TimeScale += 10;
			break;

		case '-':
			TimeScale -= 10;
			if (TimeScale < 1)
				TimeScale = 1;
			break;

		case 'r':
		case 'R':
			NowView = MERVIEW;
			break;

		case 'v':
		case 'V':
			NowView = VENUSVIEW;
			break;

		case 'e':
		case 'E':
			NowView = EARTHVIEW;
			break;

		case 'o':
		case 'O':
			NowView = MOONVIEW;
			break;

		case 'm':
		case 'M':
			NowView = MARSVIEW;
			break;

		case 'j':
		case 'J':
			NowView = JVIEW;
			break;

		case 's':
		case 'S':
			NowView = SVIEW;
			break;

		case 'u':
		case 'U':
			NowView = UVIEW;
			break;

		case 'n':
		case 'N':
			NowView = NVIEW;
			break;

		case 't':
		case 'T':
			NowMode += 1;
			if (NowMode > 2)
				NowMode -= 3;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		default:
			fprintf( stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c );
	}

	// force a call to Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// called when the mouse button transitions down or up:

void
MouseButton( int button, int state, int x, int y )
{
	int b = 0;			// LEFT, MIDDLE, or RIGHT

	if( DebugOn != 0 )
		fprintf( stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y );

	
	// get the proper button bit mask:

	switch( button )
	{
		case GLUT_LEFT_BUTTON:
			b = LEFT;		break;

		case GLUT_MIDDLE_BUTTON:
			b = MIDDLE;		break;

		case GLUT_RIGHT_BUTTON:
			b = RIGHT;		break;

		case SCROLL_WHEEL_UP:
			Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		case SCROLL_WHEEL_DOWN:
			Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
			// keep object from turning inside-out or disappearing:
			if (Scale < MINSCALE)
				Scale = MINSCALE;
			break;

		default:
			b = 0;
			fprintf( stderr, "Unknown mouse button: %d\n", button );
	}

	// button down sets the bit, up clears the bit:

	if( state == GLUT_DOWN )
	{
		Xmouse = x;
		Ymouse = y;
		ActiveButton |= b;		// set the proper bit
	}
	else
	{
		ActiveButton &= ~b;		// clear the proper bit
	}

	glutSetWindow(MainWindow);
	glutPostRedisplay();

}


// called when the mouse moves while a button is down:

void
MouseMotion( int x, int y )
{
	int dx = x - Xmouse;		// change in mouse coords
	int dy = y - Ymouse;

	if( ( ActiveButton & LEFT ) != 0 )
	{
		Xrot += ( ANGFACT*dy );
		Yrot += ( ANGFACT*dx );
	}

	if( ( ActiveButton & MIDDLE ) != 0 )
	{
		Scale += SCLFACT * (float) ( dx - dy );

		// keep object from turning inside-out or disappearing:

		if( Scale < MINSCALE )
			Scale = MINSCALE;
	}

	Xmouse = x;			// new current position
	Ymouse = y;

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void
Reset( )
{
	ActiveButton = 0;
	AxesOn = 0;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	ShadowsOn = 0;
	NowColor = YELLOW;
	NowProjection = PERSP;
	NowPlanet = EARTH;
	Xrot = Yrot = 0.;
}


// called when user resizes the window:

void
Resize( int width, int height )
{
	// don't really need to do anything since window size is
	// checked each time in Display( ):

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}


// handle a change to the window's visibility:

void
Visibility ( int state )
{
	if( DebugOn != 0 )
		fprintf( stderr, "Visibility: %d\n", state );

	if( state == GLUT_VISIBLE )
	{
		glutSetWindow( MainWindow );
		glutPostRedisplay( );
	}
	else
	{
		// could optimize by keeping track of the fact
		// that the window is not visible and avoid
		// animating or redrawing it ...
	}
}



///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////


// the stroke characters 'X' 'Y' 'Z' :

static float xx[ ] = { 0.f, 1.f, 0.f, 1.f };

static float xy[ ] = { -.5f, .5f, .5f, -.5f };

static int xorder[ ] = { 1, 2, -3, 4 };

static float yx[ ] = { 0.f, 0.f, -.5f, .5f };

static float yy[ ] = { 0.f, .6f, 1.f, 1.f };

static int yorder[ ] = { 1, 2, 3, -2, 4 };

static float zx[ ] = { 1.f, 0.f, 1.f, 0.f, .25f, .75f };

static float zy[ ] = { .5f, .5f, -.5f, -.5f, 0.f, 0.f };

static int zorder[ ] = { 1, 2, 3, 4, -5, 6 };

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void
Axes( float length )
{
	glBegin( GL_LINE_STRIP );
		glVertex3f( length, 0., 0. );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., length, 0. );
	glEnd( );
	glBegin( GL_LINE_STRIP );
		glVertex3f( 0., 0., 0. );
		glVertex3f( 0., 0., length );
	glEnd( );

	float fact = LENFRAC * length;
	float base = BASEFRAC * length;

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 4; i++ )
		{
			int j = xorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( base + fact*xx[j], fact*xy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 5; i++ )
		{
			int j = yorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( fact*yx[j], base + fact*yy[j], 0.0 );
		}
	glEnd( );

	glBegin( GL_LINE_STRIP );
		for( int i = 0; i < 6; i++ )
		{
			int j = zorder[i];
			if( j < 0 )
			{
				
				glEnd( );
				glBegin( GL_LINE_STRIP );
				j = -j;
			}
			j--;
			glVertex3f( 0.0, fact*zy[j], base + fact*zx[j] );
		}
	glEnd( );

}


// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void
HsvRgb( float hsv[3], float rgb[3] )
{
	// guarantee valid input:

	float h = hsv[0] / 60.f;
	while( h >= 6. )	h -= 6.;
	while( h <  0. ) 	h += 6.;

	float s = hsv[1];
	if( s < 0. )
		s = 0.;
	if( s > 1. )
		s = 1.;

	float v = hsv[2];
	if( v < 0. )
		v = 0.;
	if( v > 1. )
		v = 1.;

	// if sat==0, then is a gray:

	if( s == 0.0 )
	{
		rgb[0] = rgb[1] = rgb[2] = v;
		return;
	}

	// get an rgb from the hue itself:
	
	float i = (float)floor( h );
	float f = h - i;
	float p = v * ( 1.f - s );
	float q = v * ( 1.f - s*f );
	float t = v * ( 1.f - ( s * (1.f-f) ) );

	float r=0., g=0., b=0.;			// red, green, blue
	switch( (int) i )
	{
		case 0:
			r = v;	g = t;	b = p;
			break;
	
		case 1:
			r = q;	g = v;	b = p;
			break;
	
		case 2:
			r = p;	g = v;	b = t;
			break;
	
		case 3:
			r = p;	g = q;	b = v;
			break;
	
		case 4:
			r = t;	g = p;	b = v;
			break;
	
		case 5:
			r = v;	g = p;	b = q;
			break;
	}


	rgb[0] = r;
	rgb[1] = g;
	rgb[2] = b;
}

void
Cross(float v1[3], float v2[3], float vout[3])
{
	float tmp[3];
	tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
	tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
	tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
	vout[0] = tmp[0];
	vout[1] = tmp[1];
	vout[2] = tmp[2];
}

float
Dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}


float
Unit(float vin[3], float vout[3])
{
	float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		vout[0] = vin[0] / dist;
		vout[1] = vin[1] / dist;
		vout[2] = vin[2] / dist;
	}
	else
	{
		vout[0] = vin[0];
		vout[1] = vin[1];
		vout[2] = vin[2];
	}
	return dist;
}


float
Unit( float v[3] )
{
	float dist = v[0] * v[0] + v[1] * v[1] + v[2] * v[2];
	if (dist > 0.0)
	{
		dist = sqrtf(dist);
		v[0] /= dist;
		v[1] /= dist;
		v[2] /= dist;
	}
	return dist;
}
