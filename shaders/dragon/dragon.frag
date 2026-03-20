#version 330 compatibility

uniform sampler2D TexUnitA;
uniform sampler2D TexUnitB;

uniform float uBlend;
uniform float Timer;
uniform float uPower;

#define PI 3.1415926535897932384626433832795

in vec2 vST;


void main ( ) {
	vec2 st = vST - vec2(0.5,0.5);
    float x, y;
	float u, v;
	float angle, r;

    x = vST.s - 0.5;
    y = vST.t - 0.5;

    angle = atan( y, x );
    r = sqrt( x * x + y * y );

    u = angle / PI + r;
	 
    v = 100 * pow( r, uPower ) + Timer;
	
	
	vec4 colorA = texture2D(TexUnitA, vec2(u, v));
	vec4 colorB = texture2D(TexUnitB, vec2(u, v));
	vec4 mixColor = mix(colorA, colorB, uBlend);
	
	
	gl_FragColor = mixColor;
}
