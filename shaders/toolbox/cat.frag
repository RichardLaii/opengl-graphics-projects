#version 330 compatibility

uniform float uPower;
uniform float uRtheta;
uniform float uMosaic;
uniform float uBlend;
uniform sampler2D TexUnitA;
uniform sampler2D TexUnitB;

#define PI 3.1415926535897932384626433832795
in vec2 vST;


const vec4 BLACK = vec4( 0., 0., 0., 1. );

float
atan2( float y, float x )
{
        if( x == 0. )
        {
                if( y >= 0. )
                        return  PI/2.;
                else
                        return -PI/2.;
        }
        return atan(y,x);
}


void
main( )
{
	vec2 st = vST - vec2(0.5,0.5);  // put (0,0) in the middle so that the range is -0.5 to +0.5
	float r = length(st);
	float rPrime = pow(2.0 * r, uPower);
	
	float theta  = atan2( st.t, st.s );
	float thetaPrime = theta - uRtheta * r;
	
	st = rPrime * vec2( cos(thetaPrime) ,sin(thetaPrime) );  		// now in the range -1. to +1.
	st += 1;                        		// change the range to 0. to +2.
	st *= 0.5; 		       			// change the range to 0. to +1.
	
	int numins = int(st.s / uMosaic);
    int numint = int(st.t / uMosaic);
	
	float sc = numins *uMosaic + uMosaic/2;
    float tc = numint *uMosaic + uMosaic/2;
	
	st.s = sc;
    st.t = tc;
	
	if (any(lessThan(st, vec2(0.0))))
	{
		gl_FragColor = BLACK;
	}
	else if (any(greaterThan(st, vec2(1.0))))
	{
		gl_FragColor = BLACK;
	}
	else
	{
		vec3 colorA = texture(TexUnitA, st).rgb;
		vec3 colorB = texture(TexUnitB, st).rgb;

		vec3 Color = mix(colorA, colorB, uBlend);

		gl_FragColor = vec4(Color, 1.0);
	}
}