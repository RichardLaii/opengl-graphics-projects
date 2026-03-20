#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define _USE_MATH_DEFINES
#include <math.h>

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

bool Frozen;

// which projection:

enum Projections
{
	ORTHO,
	PERSP
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

enum View
{
	OUTSIDE,
	INSIDE,
	CAR
};

enum Light
{
	POINTLIGHT,
	SPOTLIGHT
};

enum Colors
{
	RED,
	YELLOW,
	GREEN,
	CYAN,
	BLUE,
	MAGENTA,
	WHITE_L,
	CHANGE
};

char * ColorNames[ ] =
{
	(char *)"Red",
	(char*)"Yellow",
	(char*)"Green",
	(char*)"Cyan",
	(char*)"Blue",
	(char*)"Magenta",
	(char*)"White"
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
	{ 1., 1., 1. },		// white
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
const int MSEC = 10000;


// what options should we compile-in?
// in general, you don't need to worry about these
// i compile these in to show class examples of things going wrong
//#define DEMO_Z_FIGHTING
//#define DEMO_DEPTH_BUFFER


// non-constant global variables:

int		ActiveButton;			// current button that is down
GLuint	AxesList;				// list to hold the axes
int		AxesOn;					// != 0 means to draw the axes
GLuint	SphereDL;				// object display list
GLuint  GridDL;
GLuint  GridDL2;
int		BusDL;
int		AirplaneDL;
int		DebugOn;				// != 0 means to print debugging info
int		DepthCueOn;				// != 0 means to use intensity depth cueing
int		DepthBufferOn;			// != 0 means to use the z-buffer
int		DepthFightingOn;		// != 0 means to force the creation of z-fighting
int		MainWindow;				// window id for main graphics window
int		NowColor;				// index into Colors[ ]
int		NowProjection;		// ORTHO or PERSP
int     LightColor;
int		NowLightSource;
int		NowLook;				// GLulookat
float	Scale;					// scaling factor
int		ShadowsOn;				// != 0 means to turn shadows on
float	Time;					// used for animation, this has a value between 0. and 1.
int		Xmouse, Ymouse;			// mouse values
float	Xrot, Yrot;				// rotation angles in degrees


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

void			Axes( float );
void			HsvRgb( float[3], float [3] );
void			Cross(float[3], float[3], float[3]);
float			Dot(float [3], float [3]);
float			Unit(float [3], float [3]);
float			Unit(float [3]);

	
// utility to create an array from 3 separate values:

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
//#include "bmptotexture.cpp"
#include "loadobjfile.cpp"
#include "keytime.cpp"
//#include "glslprogram.cpp"


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

	int ms = glutGet(GLUT_ELAPSED_TIME);
	ms %= MS_PER_CYCLE;							// makes the value of ms between 0 and MS_PER_CYCLE-1
	Time = (float)ms / (float)MS_PER_CYCLE;		// makes the value of Time between 0. and slightly less than 1.

	// for example, if you wanted to spin an object in Display( ), you might call: glRotatef( 360.f*Time,   0., 1., 0. );

	// force a call to Display( ) next time it is convenient:

	glutSetWindow( MainWindow );
	glutPostRedisplay( );
}

// Keytime value
Keytimes Xpos1, Ypos1, Zpos1;
Keytimes Xpos2, Ypos2, Zpos2;
Keytimes Xpos2_L, Ypos2_L, Zpos2_L;
Keytimes Xsun, Ysun, Zsun;
Keytimes ThetaX, ThetaY, ThetaZ;
Keytimes ThetaX2, ThetaY2, ThetaZ2;
Keytimes Red1, Green1, Blue1;
Keytimes Red2, Green2, Blue2;
Keytimes RedL, GreenL, BlueL;
Keytimes RedL2, GreenL2, BlueL2;
Keytimes Xeye1, Yeye1, Zeye1;
Keytimes Xeye2, Yeye2, Zeye2;
// draw the complete scene:

void
Display( )
{
	if (DebugOn != 0)
		fprintf(stderr, "Starting Display.\n");

	// turn # msec into the cycle ( 0 - MSEC-1 ):
	int msec = glutGet(GLUT_ELAPSED_TIME) % MSEC;

	// turn that into a time in seconds:
	float nowTime = (float)msec / 1000.;

	

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

	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	// set the eye position, look-at position, and up-vector:
	float X_eye1 = Xeye1.GetValue(nowTime);
	float Y_eye1 = Yeye1.GetValue(nowTime);
	float Z_eye1 = Zeye1.GetValue(nowTime);

	float X_eye2 = Xeye2.GetValue(nowTime);
	float Y_eye2 = Yeye2.GetValue(nowTime);
	float Z_eye2 = Zeye2.GetValue(nowTime);

	if( NowLook == OUTSIDE)
		gluLookAt( 4.f, 2.5f, 2.f,     0.f, 0.5f, 0.f,     0.f, 1.f, 0.f );
	else if ( NowLook == INSIDE){
		gluLookAt(X_eye1 + 1, Y_eye1 + 1, Z_eye1 + 1, X_eye1, Y_eye1, Z_eye1, 0.f, 1.f, 0.f);
	}
	else if ( NowLook == CAR)
		gluLookAt(X_eye2 + 1, Y_eye2 + 1, Z_eye2 + 1, X_eye2, Y_eye2, Z_eye2, 0.f, 1.f, 0.f);

	// rotate the scene:

	glRotatef( (GLfloat)Yrot, 0.f, 1.f, 0.f );
	glRotatef( (GLfloat)Xrot, 1.f, 0.f, 0.f );

	// light sorce
	float xlight = Xsun.GetValue(nowTime);
	float ylight = Ysun.GetValue(nowTime);
	float zlight = Zsun.GetValue(nowTime);

	if (LightColor == WHITE_L && NowLightSource == POINTLIGHT)
	{
		SetPointLight(GL_LIGHT0, 0., 1.5, zlight, 1., 1., 1.);
	}
	else if (LightColor == WHITE_L && NowLightSource == SPOTLIGHT)
		SetSpotLight(GL_LIGHT0, xlight, ylight, zlight, 0, -0.2, 0, 1., 1., 1.);

	else if (LightColor == CHANGE && NowLightSource == POINTLIGHT)
		SetPointLight(GL_LIGHT0, xlight, ylight, zlight, RedL.GetValue(nowTime), GreenL.GetValue(nowTime), BlueL.GetValue(nowTime));
	else if (LightColor == CHANGE && NowLightSource == SPOTLIGHT)
		SetSpotLight(GL_LIGHT0, xlight, ylight, zlight, 0, -0.2, 0, RedL.GetValue(nowTime), GreenL.GetValue(nowTime), BlueL.GetValue(nowTime));

	else if (LightColor == RED && NowLightSource == POINTLIGHT)
		SetPointLight(GL_LIGHT0, xlight, ylight, zlight, 1., 0., 0.);
	else if (LightColor == RED && NowLightSource == SPOTLIGHT)
		SetSpotLight(GL_LIGHT0, xlight, ylight, zlight, 0, -0.2, 0, 1., 0., 0.);

	else if (LightColor == GREEN && NowLightSource == POINTLIGHT)
		SetPointLight(GL_LIGHT0, xlight, ylight, zlight, 0., 1., 0.);
	else if (LightColor == GREEN && NowLightSource == SPOTLIGHT)
		SetSpotLight(GL_LIGHT0, xlight, ylight, zlight, 0, -0.2, 0, 0., 1., 0.);

	else if (LightColor == BLUE && NowLightSource == POINTLIGHT)
		SetPointLight(GL_LIGHT0, xlight, ylight, zlight, 0., 0., 1.);
	else if (LightColor == BLUE && NowLightSource == SPOTLIGHT)
		SetSpotLight(GL_LIGHT0, xlight, ylight, zlight, 0, -0.2, 0, 0., 0., 1.);

	else if (LightColor == YELLOW && NowLightSource == POINTLIGHT)
		SetPointLight(GL_LIGHT0, xlight, ylight, zlight, 1., 1., 0.);
	else if (LightColor == YELLOW && NowLightSource == SPOTLIGHT)
		SetSpotLight(GL_LIGHT0, xlight, ylight, zlight, 0, -0.2, 0, 1., 1., 0.);

	else if (LightColor == MAGENTA && NowLightSource == POINTLIGHT)
		SetPointLight(GL_LIGHT0, xlight, ylight, zlight, 1., 0., 1.);
	else if (LightColor == MAGENTA && NowLightSource == SPOTLIGHT)
		SetSpotLight(GL_LIGHT0, xlight, ylight, zlight, 0, -0.2, 0, 1., 0., 1.);
	//CYAN
	else if (LightColor == CYAN && NowLightSource == POINTLIGHT)
		SetPointLight(GL_LIGHT0, xlight, ylight, zlight, 0., 1., 1.);
	else if (LightColor == CYAN && NowLightSource == SPOTLIGHT)
		SetSpotLight(GL_LIGHT0, xlight, ylight, zlight, 0, -0.2, 0, 0., 1., 1.);

	glPushMatrix();
	glTranslatef(Xpos2.GetValue(nowTime), Ypos2.GetValue(nowTime), Zpos2.GetValue(nowTime));
	glRotatef(ThetaY2.GetValue(nowTime), 0., 1., 0.);
	SetSpotLight(GL_LIGHT1,0.3, 0, 0.1, Xpos2_L.GetValue(nowTime), -0.3, abs(Zpos2_L.GetValue(nowTime))
					, RedL2.GetValue(nowTime), 0, BlueL2.GetValue(nowTime));
	glPopMatrix();

	glShadeModel(GL_SMOOTH);
	// uniformly scale the scene:

	if( Scale < MINSCALE )
		Scale = MINSCALE;
	glScalef( (GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale );

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

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);

	
	// since we are using glScalef( ), be sure the normals get unitized:

	glEnable( GL_NORMALIZE );

	glPushMatrix();
		glTranslatef(Xpos1.GetValue(nowTime), Ypos1.GetValue(nowTime), Zpos1.GetValue(nowTime));
		glRotatef(0., 1., 0., 0.);
		glRotatef(ThetaY.GetValue(nowTime), 0., 1., 0.);
		glRotatef(0., 0., 0., 1.);
	//<< draw object #1 >>
		//glTranslatef(0., 0., 0.);
		glRotatef(-180, 0., 1., 0.);
		glRotatef(-90, 1, 0., 0.);
		glScalef(0.0005, 0.0005, 0.0005);
		
		SetMaterial( Red1.GetValue(nowTime), Green1.GetValue(nowTime), Blue1.GetValue(nowTime), 100.f);
		glCallList(AirplaneDL);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(Xpos2.GetValue(nowTime), Ypos2.GetValue(nowTime), Zpos2.GetValue(nowTime));
	glRotatef(ThetaY2.GetValue(nowTime), 0., 1., 0.);
	glTranslatef(0.3, 0, 0.3);
	glRotatef(-180, 0., 1., 0.);
	glScalef(0.06, 0.06, 0.06);
	SetMaterial(Red2.GetValue(nowTime), Green2.GetValue(nowTime), Blue2.GetValue(nowTime), 100.f);
	glCallList(BusDL);
	glPopMatrix();

	glCallList(GridDL);

	glPushMatrix();
		glCallList(GridDL2);
	glPopMatrix();
	// draw the box object by calling up its display list:
	glDisable(GL_LIGHTING);

/*
	glPushMatrix();
	glTranslatef(xlight, ylight, zlight);
	glColor3f(0.9, 0.8, 0.1);
	OsuSphere(0.05, 32, 32);
	glPopMatrix();
	*/

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
	glColor3f( 0.f, 1.f, 1.f );
	//DoRasterString( 0.f, 1.f, 0.f, (char *)"Text That Moves" );


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

	// Time Value
	Xpos1.Init();
	Xpos1.AddTimeValue(0.0, 0.000);
	Xpos1.AddTimeValue(0.5, 0.218);
	Xpos1.AddTimeValue(1.0, 0.333);
	Xpos1.AddTimeValue(2.0, 0.518);
	Xpos1.AddTimeValue(3.5, 1.518);
	Xpos1.AddTimeValue(4.0, 1.818);
	Xpos1.AddTimeValue(5.0, 2.300);
	Xpos1.AddTimeValue(7.0, 0.700);
	Xpos1.AddTimeValue(9.0, 0.300);
	Xpos1.AddTimeValue(10.0, 0.000);

	Ypos1.Init();
	Ypos1.AddTimeValue(0.0, 0.000);
	Ypos1.AddTimeValue(0.5, 0.318);
	Ypos1.AddTimeValue(2.0, 0.800);
	Ypos1.AddTimeValue(2.5, 1.318);
	Ypos1.AddTimeValue(3.0, 1.518);
	Ypos1.AddTimeValue(5.0, 2.200);
	Ypos1.AddTimeValue(6.0, 1.710);
	Ypos1.AddTimeValue(7.0, 1.310);
	Ypos1.AddTimeValue(8.0, 0.610);
	Ypos1.AddTimeValue(10.0, 0.000);

	Zpos1.Init();
	Zpos1.AddTimeValue(0.0, 0.000);
	Zpos1.AddTimeValue(1.0, 0.718);
	Zpos1.AddTimeValue(2.0, 1.500);
	Zpos1.AddTimeValue(3.0, 2.018);
	Zpos1.AddTimeValue(4.0, 1.318);
	Zpos1.AddTimeValue(5.0, 0.000);
	Zpos1.AddTimeValue(6.0, -1.000);
	Zpos1.AddTimeValue(7.0, -2.000);
	Zpos1.AddTimeValue(8.0, -1.500);
	Zpos1.AddTimeValue(9.0, -0.500);
	Zpos1.AddTimeValue(10.0, 0.000);

	ThetaY.Init();
	ThetaY.AddTimeValue(0.0, 0.000);
	ThetaY.AddTimeValue(0.5, 0);
	ThetaY.AddTimeValue(1.0, 0);
	ThetaY.AddTimeValue(2.0, 30);
	ThetaY.AddTimeValue(2.5, 70);
	ThetaY.AddTimeValue(3.0, 90);
	ThetaY.AddTimeValue(3.5, 110);
	ThetaY.AddTimeValue(4.0, 130);
	ThetaY.AddTimeValue(5.0, 160);
	ThetaY.AddTimeValue(6.0, 190);
	ThetaY.AddTimeValue(7.0, 230);
	ThetaY.AddTimeValue(8.0, 270);
	ThetaY.AddTimeValue(9.0, 330);
	ThetaY.AddTimeValue(10.0, 360);

	Xsun.Init();
	Xsun.AddTimeValue(0.0, 1);
	Xsun.AddTimeValue(1.0, 1.3);
	Xsun.AddTimeValue(2.0, 1.5);
	Xsun.AddTimeValue(3.0, 2.3);
	Xsun.AddTimeValue(4.0, 1.4);
	Xsun.AddTimeValue(5.0, 0.6);
	Xsun.AddTimeValue(6.0, 0.);
	Xsun.AddTimeValue(7.0, 0.4);
	Xsun.AddTimeValue(8.0, 0.7);
	Xsun.AddTimeValue(9.0, 0.9);
	Xsun.AddTimeValue(10.0, 1);

	Ysun.Init();
	Ysun.AddTimeValue(0.0, 2);
	Ysun.AddTimeValue(1.0, 1.8);
	Ysun.AddTimeValue(2.0, 2.3);
	Ysun.AddTimeValue(3.0, 1.7);
	Ysun.AddTimeValue(4.0, 2.4);
	Ysun.AddTimeValue(5.0, 1.9);
	Ysun.AddTimeValue(6.0, 1.8);
	Ysun.AddTimeValue(7.0, 1.7);
	Ysun.AddTimeValue(8.0, 2.2);
	Ysun.AddTimeValue(9.0, 2.6);
	Ysun.AddTimeValue(10.0, 2);

	Zsun.Init();
	Zsun.AddTimeValue(0.0, 0);
	Zsun.AddTimeValue(1.0, 1.3);
	Zsun.AddTimeValue(2.0, 1.7);
	Zsun.AddTimeValue(3.0, 2.);
	Zsun.AddTimeValue(4.0, 1.2);
	Zsun.AddTimeValue(5.0, 0.2);
	Zsun.AddTimeValue(6.0, -0.9);
	Zsun.AddTimeValue(7.0, -1.7);
	Zsun.AddTimeValue(8.0, -2.2);
	Zsun.AddTimeValue(9.0, -1.3);
	Zsun.AddTimeValue(10.0, 0);

	Xpos2.Init();
	Xpos2.AddTimeValue(0.0, 0.300);
	Xpos2.AddTimeValue(0.5, 0.3);
	Xpos2.AddTimeValue(1.0, 0.3);
	Xpos2.AddTimeValue(2.0, 0.518);
	Xpos2.AddTimeValue(3.5, 1.418);
	Xpos2.AddTimeValue(4.0, 1.618);
	Xpos2.AddTimeValue(4.5, 1.718);
	Xpos2.AddTimeValue(5.0, 1.500);
	Xpos2.AddTimeValue(5.5, 1.200);
	Xpos2.AddTimeValue(6.0, 0.600);
	Xpos2.AddTimeValue(7.0, 0.300);
	Xpos2.AddTimeValue(9.0, 0.300);
	Xpos2.AddTimeValue(10.0, 0.300);

	Zpos2.Init();
	Zpos2.AddTimeValue(0.0, 0.300);
	Zpos2.AddTimeValue(0.5, 0.6);
	Zpos2.AddTimeValue(1.0, 0.99);
	Zpos2.AddTimeValue(2.0, 1.23);
	Zpos2.AddTimeValue(3.5, 1.518);
	Zpos2.AddTimeValue(4.0, 0.718);
	Zpos2.AddTimeValue(4.5, 0.318);
	Zpos2.AddTimeValue(5.0, -0.500);
	Zpos2.AddTimeValue(5.5, -0.900);
	Zpos2.AddTimeValue(6.0, -1.500);
	Zpos2.AddTimeValue(7.0, -0.700);
	Zpos2.AddTimeValue(9.0, -0.300);
	Zpos2.AddTimeValue(10.0, 0.300);

	Ypos2.Init();
	Ypos2.AddTimeValue(0.0, 0.000);
	Ypos2.AddTimeValue(1.0, -0.100);
	Ypos2.AddTimeValue(2.0, 0.000);
	Ypos2.AddTimeValue(3.0, -0.100);
	Ypos2.AddTimeValue(4.0, 0.000);
	Ypos2.AddTimeValue(5.0, -0.100);
	Ypos2.AddTimeValue(6.0, 0.000);
	Ypos2.AddTimeValue(7.0, -0.100);
	Ypos2.AddTimeValue(8.0, 0.000);
	Ypos2.AddTimeValue(9.0, -0.100);
	Ypos2.AddTimeValue(10.0, 0.000);

	ThetaY2.Init();
	ThetaY2.AddTimeValue(0.0, 0);
	ThetaY2.AddTimeValue(1.0, 10);
	ThetaY2.AddTimeValue(2.0, 60);
	ThetaY2.AddTimeValue(3.0, 90);
	ThetaY2.AddTimeValue(3.5, 180);
	ThetaY2.AddTimeValue(4.0, 200);
	ThetaY2.AddTimeValue(4.5, 230);
	ThetaY2.AddTimeValue(5.0, 250);
	ThetaY2.AddTimeValue(6.0, 270);
	ThetaY2.AddTimeValue(7.0, 360);
	ThetaY2.AddTimeValue(8.0, 360);
	ThetaY2.AddTimeValue(10.0, 360);

	Red1.Init();
	Red1.AddTimeValue(0.0, 1.000);
	Red1.AddTimeValue(1.0, 0.500);
	Red1.AddTimeValue(2.0, 0.000);
	Red1.AddTimeValue(3.0, 0.500);
	Red1.AddTimeValue(4.0, 1.000);
	Red1.AddTimeValue(5.0, 0.500);
	Red1.AddTimeValue(6.0, 0.600);
	Red1.AddTimeValue(7.0, 0.500);
	Red1.AddTimeValue(8.0, 1.000);
	Red1.AddTimeValue(9.0, 0.500);
	Red1.AddTimeValue(10.0, 0.000);

	Green1.Init();
	Green1.AddTimeValue(0.0, 0.000);
	Green1.AddTimeValue(1.0, 1.000);
	Green1.AddTimeValue(2.0, 0.000);
	Green1.AddTimeValue(3.0, 1.000);
	Green1.AddTimeValue(4.0, 0.000);
	Green1.AddTimeValue(5.0, 1.000);
	Green1.AddTimeValue(6.0, 0.000);
	Green1.AddTimeValue(7.0, 1.000);
	Green1.AddTimeValue(8.0, 0.400);
	Green1.AddTimeValue(9.0, 1.000);
	Green1.AddTimeValue(10.0, 0.000);

	Blue1.Init();
	Blue1.AddTimeValue(0.0, 0.000);
	Blue1.AddTimeValue(1.0, 0.300);
	Blue1.AddTimeValue(2.0, 0.600);
	Blue1.AddTimeValue(3.0, 1.000);
	Blue1.AddTimeValue(4.0, 0.700);
	Blue1.AddTimeValue(5.0, 0.400);
	Blue1.AddTimeValue(6.0, 0.100);
	Blue1.AddTimeValue(7.0, 0.300);
	Blue1.AddTimeValue(8.0, 0.600);
	Blue1.AddTimeValue(9.0, 0.900);
	Blue1.AddTimeValue(10.0, 1.000);

	Red2.Init();
	Red2.AddTimeValue(0.0, 0.000);
	Red2.AddTimeValue(1.0, 0.300);
	Red2.AddTimeValue(2.0, 0.600);
	Red2.AddTimeValue(3.0, 0.900);
	Red2.AddTimeValue(4.0, 1.700);
	Red2.AddTimeValue(5.0, 0.500);
	Red2.AddTimeValue(6.0, 0.300);
	Red2.AddTimeValue(7.0, 0.100);
	Red2.AddTimeValue(8.0, 1.000);
	Red2.AddTimeValue(9.0, 0.800);
	Red2.AddTimeValue(10.0, 0.000);

	Green2.Init();
	Green2.AddTimeValue(0.0, 0.400);
	Green2.AddTimeValue(1.0, 0.600);
	Green2.AddTimeValue(2.0, 0.800);
	Green2.AddTimeValue(3.0, 1.000);
	Green2.AddTimeValue(4.0, 0.700);
	Green2.AddTimeValue(5.0, 0.400);
	Green2.AddTimeValue(6.0, 0.100);
	Green2.AddTimeValue(7.0, 0.500);
	Green2.AddTimeValue(8.0, 1.000);
	Green2.AddTimeValue(9.0, 0.600);
	Green2.AddTimeValue(10.0, 1.000);

	Blue2.Init();
	Blue2.AddTimeValue(0.0, 1.000);
	Blue2.AddTimeValue(1.0, 0.300);
	Blue2.AddTimeValue(2.0, 0.600);
	Blue2.AddTimeValue(3.0, 1.000);
	Blue2.AddTimeValue(4.0, 0.700);
	Blue2.AddTimeValue(5.0, 0.400);
	Blue2.AddTimeValue(6.0, 1.000);
	Blue2.AddTimeValue(7.0, 0.300);
	Blue2.AddTimeValue(8.0, 0.600);
	Blue2.AddTimeValue(9.0, 0.900);
	Blue2.AddTimeValue(10.0, 1.000);

	RedL.Init();
	RedL.AddTimeValue(0.0, 0.800);
	RedL.AddTimeValue(1.0, 0.600);
	RedL.AddTimeValue(2.0, 0.900);
	RedL.AddTimeValue(3.0, 0.000);
	RedL.AddTimeValue(4.0, 0.800);
	RedL.AddTimeValue(5.0, 0.900);
	RedL.AddTimeValue(6.0, 1.000);
	RedL.AddTimeValue(7.0, 0.700);
	RedL.AddTimeValue(8.0, 0.400);
	RedL.AddTimeValue(9.0, 0.600);
	RedL.AddTimeValue(10.0, 0.900);

	GreenL.Init();
	GreenL.AddTimeValue(0.0, 0.000);
	GreenL.AddTimeValue(1.0, 1.000);
	GreenL.AddTimeValue(2.0, 1.000);
	GreenL.AddTimeValue(3.0, 1.000);
	GreenL.AddTimeValue(4.0, 0.000);
	GreenL.AddTimeValue(5.0, 0.500);
	GreenL.AddTimeValue(6.0, 1.000);
	GreenL.AddTimeValue(7.0, 0.100);
	GreenL.AddTimeValue(8.0, 0.900);
	GreenL.AddTimeValue(9.0, 0.000);
	GreenL.AddTimeValue(10.0, 1.000);

	BlueL.Init();
	BlueL.AddTimeValue(0.0, 1.000);
	BlueL.AddTimeValue(1.0, 0.000);
	BlueL.AddTimeValue(2.0, 0.200);
	BlueL.AddTimeValue(3.0, 0.900);
	BlueL.AddTimeValue(4.0, 0.100);
	BlueL.AddTimeValue(5.0, 0.200);
	BlueL.AddTimeValue(6.0, 0.100);
	BlueL.AddTimeValue(7.0, 1.000);
	BlueL.AddTimeValue(8.0, 0.400);
	BlueL.AddTimeValue(9.0, 0.100);
	BlueL.AddTimeValue(10.0, 0.700);

	RedL2.Init();
	RedL2.AddTimeValue(0.0, 1.000);
	RedL2.AddTimeValue(1.0, 1.000);
	RedL2.AddTimeValue(2.0, 0.000);
	RedL2.AddTimeValue(3.0, 0.000);
	RedL2.AddTimeValue(4.0, 1.000);
	RedL2.AddTimeValue(5.0, 1.000);
	RedL2.AddTimeValue(6.0, 0.000);
	RedL2.AddTimeValue(7.0, 0.000);
	RedL2.AddTimeValue(8.0, 1.00);
	RedL2.AddTimeValue(9.0, 1.00);
	RedL2.AddTimeValue(10.0, 0.000);

	

	BlueL2.Init();
	BlueL2.AddTimeValue(0.0, 0.000);
	BlueL2.AddTimeValue(1.0, 0.000);
	BlueL2.AddTimeValue(2.0, 1.00);
	BlueL2.AddTimeValue(3.0, 1.00);
	BlueL2.AddTimeValue(4.0, 0.00);
	BlueL2.AddTimeValue(5.0, 0.00);
	BlueL2.AddTimeValue(6.0, 1.00);
	BlueL2.AddTimeValue(7.0, 1.000);
	BlueL2.AddTimeValue(8.0, 0.000);
	BlueL2.AddTimeValue(9.0, 0.000);
	BlueL2.AddTimeValue(10.0, 1.00);

	Xeye1.Init();
	Xeye1.AddTimeValue(0.0, 0.000);
	Xeye1.AddTimeValue(0.5, 0.218);
	Xeye1.AddTimeValue(1.0, 0.333);
	Xeye1.AddTimeValue(2.0, 0.518);
	Xeye1.AddTimeValue(3.5, 1.518);
	Xeye1.AddTimeValue(4.0, 1.818);
	Xeye1.AddTimeValue(5.0, 2.300);
	Xeye1.AddTimeValue(7.0, 0.700);
	Xeye1.AddTimeValue(9.0, 0.300);
	Xeye1.AddTimeValue(10.0, 0.000);

	Yeye1.Init();
	Yeye1.AddTimeValue(0.0, 0.000);
	Yeye1.AddTimeValue(0.5, 0.318);
	Yeye1.AddTimeValue(2.0, 0.800);
	Yeye1.AddTimeValue(2.5, 1.318);
	Yeye1.AddTimeValue(3.0, 1.518);
	Yeye1.AddTimeValue(5.0, 2.200);
	Yeye1.AddTimeValue(6.0, 1.710);
	Yeye1.AddTimeValue(7.0, 1.310);
	Yeye1.AddTimeValue(8.0, 0.610);
	Yeye1.AddTimeValue(10.0, 0.000);

	Zeye1.Init();
	Zeye1.AddTimeValue(0.0, 0.000);
	Zeye1.AddTimeValue(1.0, 0.718);
	Zeye1.AddTimeValue(2.0, 1.500);
	Zeye1.AddTimeValue(3.0, 2.018);
	Zeye1.AddTimeValue(4.0, 1.318);
	Zeye1.AddTimeValue(5.0, 0.000);
	Zeye1.AddTimeValue(6.0, -1.000);
	Zeye1.AddTimeValue(7.0, -2.000);
	Zeye1.AddTimeValue(8.0, -1.500);
	Zeye1.AddTimeValue(9.0, -0.500);
	Zeye1.AddTimeValue(10.0, 0.000);

	Xeye2.Init();
	Xeye2.AddTimeValue(0.0, 0.300);
	Xeye2.AddTimeValue(0.5, 0.3);
	Xeye2.AddTimeValue(1.0, 0.3);
	Xeye2.AddTimeValue(2.0, 0.518);
	Xeye2.AddTimeValue(3.5, 1.418);
	Xeye2.AddTimeValue(4.0, 1.618);
	Xeye2.AddTimeValue(4.5, 1.718);
	Xeye2.AddTimeValue(5.0, 1.500);
	Xeye2.AddTimeValue(5.5, 1.200);
	Xeye2.AddTimeValue(6.0, 0.600);
	Xeye2.AddTimeValue(7.0, 0.300);
	Xeye2.AddTimeValue(9.0, 0.300);
	Xeye2.AddTimeValue(10.0, 0.300);

	Zeye2.Init();
	Zeye2.AddTimeValue(0.0, 0.300);
	Zeye2.AddTimeValue(0.5, 0.6);
	Zeye2.AddTimeValue(1.0, 0.99);
	Zeye2.AddTimeValue(2.0, 1.23);
	Zeye2.AddTimeValue(3.5, 1.518);
	Zeye2.AddTimeValue(4.0, 0.718);
	Zeye2.AddTimeValue(4.5, 0.318);
	Zeye2.AddTimeValue(5.0, -0.500);
	Zeye2.AddTimeValue(5.5, -0.900);
	Zeye2.AddTimeValue(6.0, -1.500);
	Zeye2.AddTimeValue(7.0, -0.700);
	Zeye2.AddTimeValue(9.0, -0.300);
	Zeye2.AddTimeValue(10.0, 0.300);

	Yeye2.Init();
	Yeye2.AddTimeValue(0.0, 0.000);
	Yeye2.AddTimeValue(1.0, -0.100);
	Yeye2.AddTimeValue(2.0, 0.000);
	Yeye2.AddTimeValue(3.0, -0.100);
	Yeye2.AddTimeValue(4.0, 0.000);
	Yeye2.AddTimeValue(5.0, -0.100);
	Yeye2.AddTimeValue(6.0, 0.000);
	Yeye2.AddTimeValue(7.0, -0.100);
	Yeye2.AddTimeValue(8.0, 0.000);
	Yeye2.AddTimeValue(9.0, -0.100);
	Yeye2.AddTimeValue(10.0, 0.000);

	Xpos2_L.Init();
	Xpos2_L.AddTimeValue(0.0, 0.300);
	Xpos2_L.AddTimeValue(0.5, 0.3);
	Xpos2_L.AddTimeValue(1.0, 0.3);
	Xpos2_L.AddTimeValue(2.0, 0.518);
	Xpos2_L.AddTimeValue(3.5, 1.418);
	Xpos2_L.AddTimeValue(4.0, 1.618);
	Xpos2_L.AddTimeValue(4.5, 1.718);
	Xpos2_L.AddTimeValue(5.0, 1.500);
	Xpos2_L.AddTimeValue(5.5, 1.200);
	Xpos2_L.AddTimeValue(6.0, 0.600);
	Xpos2_L.AddTimeValue(7.0, 0.300);
	Xpos2_L.AddTimeValue(9.0, 0.300);
	Xpos2_L.AddTimeValue(10.0, 0.300);

	Zpos2_L.Init();
	Zpos2_L.AddTimeValue(0.0, 0.300);
	Zpos2_L.AddTimeValue(0.5, 0.6);
	Zpos2_L.AddTimeValue(1.0, 0.99);
	Zpos2_L.AddTimeValue(2.0, 1.23);
	Zpos2_L.AddTimeValue(3.5, 1.518);
	Zpos2_L.AddTimeValue(4.0, 0.718);
	Zpos2_L.AddTimeValue(4.5, 0.318);
	Zpos2_L.AddTimeValue(5.0, -0.500);
	Zpos2_L.AddTimeValue(5.5, -0.900);
	Zpos2_L.AddTimeValue(6.0, -1.500);
	Zpos2_L.AddTimeValue(7.0, -0.700);
	Zpos2_L.AddTimeValue(9.0, -0.300);
	Zpos2_L.AddTimeValue(10.0, 0.300);

	Ypos2_L.Init();
	Ypos2_L.AddTimeValue(0.0, 0.000);
	Ypos2_L.AddTimeValue(1.0, -0.100);
	Ypos2_L.AddTimeValue(2.0, 0.000);
	Ypos2_L.AddTimeValue(3.0, -0.100);
	Ypos2_L.AddTimeValue(4.0, 0.000);
	Ypos2_L.AddTimeValue(5.0, -0.100);
	Ypos2_L.AddTimeValue(6.0, 0.000);
	Ypos2_L.AddTimeValue(7.0, -0.100);
	Ypos2_L.AddTimeValue(8.0, 0.000);
	Ypos2_L.AddTimeValue(9.0, -0.100);
	Ypos2_L.AddTimeValue(10.0, 0.000);
}


// initialize the display lists that will not change:
// (a display list is a way to store opengl commands inF
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

	#define XSIDE	3			// length of the x side of the grid
	#define X0      (-XSIDE/2.)		// where one side starts
	#define NX	300			// how many points in x
	#define DX	( XSIDE/(float)NX )	// change in x between the points

	#define YGRID	0.f

	#define ZSIDE	4			// length of the z side of the grid
	#define Z0      (-ZSIDE/2.)		// where one side starts
	#define NZ	300			// how many points in z
	#define DZ	( ZSIDE/(float)NZ )	// change in z between the points

	GridDL = glGenLists( 1 ); //grid
	glNewList(GridDL, GL_COMPILE );
	glPushMatrix();
		SetMaterial(0.6f, 0.6f, 0.6f, 30.f);
		glColor3f(0.6, 0.6, 0.6);
		glTranslatef(0.5, -0.2, 0.);
		glNormal3f(0., 1., 0.);
		for (int i = 0; i < NZ; i++)
		{
			glBegin(GL_QUAD_STRIP);
			for (int j = 0; j < NX; j++)
			{
				glVertex3f(X0 + DX * (float)j, YGRID, Z0 + DZ * (float)(i + 0));
				glVertex3f(X0 + DX * (float)j, YGRID, Z0 + DZ * (float)(i + 1));
			}
			glEnd();
		}
	glPopMatrix();
	glEndList( );

#define YSIDE1	3			// length of the x side of the grid
#define Y01      (-YSIDE1/2.)		// where one side starts
#define NY1	300			// how many points in x
#define DY1	( YSIDE1/(float)NY1 )	// change in x between the points

#define XGRID1	0.f

#define ZSIDE1	4			// length of the z side of the grid
#define Z01      (-ZSIDE1/2.)		// where one side starts
#define NZ1	300			// how many points in z
#define DZ1	( ZSIDE1/(float)NZ1 )	// change in z between the points

	GridDL2 = glGenLists(1); //grid
	glNewList(GridDL2, GL_COMPILE);

	SetMaterial(0.6f, 0.6f, 0.6f, 30.f);
	glColor3f(0.6, 0.6, 0.6);
	glTranslatef(-1., 1.3, 0.);
	glNormal3f(1., 0., 0.);
	for (int i = 0; i < NZ1; i++)
	{
		glBegin(GL_QUAD_STRIP);
		for (int j = 0; j < NY1; j++)
		{
			glVertex3f(XGRID1, Y01 + DY1 * (float)j, Z01 + DZ1 * (float)(i + 0));
			glVertex3f(XGRID1, Y01 + DY1 * (float)j, Z01 + DZ1 * (float)(i + 1));
		}
		glEnd();
	}
	glEndList();

	

	//SphereDL = glGenLists(1);
	//glNewList(SphereDL, GL_COMPILE);
	//	SetMaterial(1.f, 0.f, 0.f, 30.f);
	//	OsuSphere(0.3, 32, 32);
	//glEndList();

	AirplaneDL = glGenLists(1);
	glNewList(AirplaneDL, GL_COMPILE);	
		LoadObjFile((char*)"11805_airplane_v2_L2.obj");
	glEndList();

	BusDL = glGenLists(1);
	glNewList(BusDL, GL_COMPILE);
		LoadObjFile((char*)"G7 1200.obj");
	glEndList();

	

	// create the axes:

	AxesList = glGenLists( 1 );
	glNewList( AxesList, GL_COMPILE );
		glLineWidth( AXES_WIDTH );
			Axes( 1.5 );
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
		//case 'o':
		//case 'O':
			//NowProjection = ORTHO;
			//break;

		//case 'p':
		//case 'P':
			//NowProjection = PERSP;
			//break;

		case 's':
		case 'S':
			NowLightSource = SPOTLIGHT;
			break;

		case 'p':
		case 'P':
			//NowProjection = PERSP;
			NowLightSource = POINTLIGHT;
			break;

		case 'h':
		case 'H':
			LightColor = CHANGE;				
			break;

		case 'w':
		case 'W':
			LightColor = WHITE_L;				//white light
			break;

		case 'r':
		case 'R':
			LightColor = RED;
			break;

		case 'g':
		case 'G':
			LightColor = GREEN;
			break;

		case 'b':
		case 'B':
			LightColor = BLUE;
			break;

		case 'y':
		case 'Y':
			LightColor = YELLOW;
			break;

		case 'm':
		case 'M':
			LightColor = MAGENTA;
			break;

		case 'c':
		case 'C':
			LightColor = CYAN;
			break;

		case 'o':
		case 'O':
			NowLook = OUTSIDE;
			break;

		case 'i':
		case 'I':
			NowLook = INSIDE;
			break;

		case 'k':
		case 'K':
			NowLook = CAR;
			break;

		case 'q':
		case 'Q':
		case ESCAPE:
			DoMainMenu( QUIT );	// will not return here
			break;				// happy compiler

		case 'f':
		case 'F':
			Frozen = !Frozen;
			if (Frozen)
				glutIdleFunc(NULL);
			else
				glutIdleFunc(Animate);
			break;

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
	AxesOn = 1;
	DebugOn = 0;
	DepthBufferOn = 1;
	DepthFightingOn = 0;
	DepthCueOn = 0;
	Scale  = 1.0;
	ShadowsOn = 0;
	NowColor = YELLOW;
	NowProjection = PERSP;
	Xrot = Yrot = 0.;
	NowLook = OUTSIDE;
	NowLightSource = POINTLIGHT;
	LightColor = WHITE_L;
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
