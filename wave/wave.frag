#version 330 compatibility

in vec3 vMC;
in vec3 vNs;
in vec3 vLs;
in vec3 vEs;
in vec4  vPos;

uniform float uKa, uKd, uKs;
uniform vec4 uColor;
uniform float uShininess;

uniform sampler3D Noise3;
uniform samplerCube uReflectUnit;
uniform samplerCube uRefractUnit;
uniform float uTemp;

uniform float uMix;
uniform float uEta;

uniform float uNoiseAmp;
uniform float uNoiseFreq;

const vec3 WATERCOLOR = vec3( .6, .85, 1. );
const vec3 ICECOLOR = vec3( 0.75, 0.85, 1.);
const vec3 WHITE = vec3( 0.75, 0.85, 1.);

vec3
RotateNormal( float angx, float angy, vec3 n )
{
	float cx = cos( angx );
	float sx = sin( angx );
	float cy = cos( angy );
	float sy = sin( angy );
	
	// rotate about x:
	float yp = n.y*cx - n.z*sx; // y'
	n.z = n.y*sx + n.z*cx; // z'
	n.y = yp;
	// n.x = n.x;
	
	// rotate about y:
	float xp = n.x*cy + n.z*sy; // x'
	n.z = -n.x*sy + n.z*cy; // z'
	n.x = xp;
	// n.y = n.y;
	
	return normalize( n );
}

void
main( )
{
	vec3 reflectvector;
	vec3 refractvector;
	vec3 reflectcolor;
	vec3 refractcolor;
	vec3 FragColor;
	float Alpha = 1;
	
	vec4 nvx = texture3D( Noise3, uNoiseFreq*vMC );
	vec4 nvy = texture3D( Noise3, uNoiseFreq*vec3(vMC.xy,vMC.z+0.5) );
	
	float angx = nvx.r + nvx.g + nvx.b + nvx.a; // 1. -> 3.
	angx = angx - 2.;
	
	angx *= uNoiseAmp;
	
	float angy = nvy.r + nvy.g + nvy.b + nvy.a; // 1. -> 3.
	angy = angy - 2.;

	angy *= uNoiseAmp;

	vec3 normal = normalize( vNs );
	vec3 light = normalize( vLs );
	vec3 eye = normalize( vEs );

	normal = RotateNormal( angx, angy, normal );
	
	/*
	vec4 nvx = texture3D( Noise3, 0.28*vPos.xyz );
    vec4 nvy = texture3D( Noise3, 0.28*vec3(vPos.xy,vPos.z+0.5) );
    float angx = nvx.r + nvx.g + nvx.b + nvx.a;
    angx =  uNoiseAmp * (angx - 2.);
    float angy = nvy.r + nvy.g + nvy.b + nvy.a;
    angy =  uNoiseAmp * (angy - 2.);
    normal = RotateNormal(angx, angy, normal);
	*/
	

	reflectvector = reflect( eye, normal );
	reflectcolor = textureCube( uReflectUnit, reflectvector ).rgb;
	refractvector = refract( eye, normal, uEta );
	refractcolor = textureCube( uRefractUnit, refractvector ).rgb;



	if( (uTemp>0)&&(uTemp<1) )
	{
		refractcolor = mix( refractcolor, WATERCOLOR, 0.30 );
		FragColor = mix( refractcolor, reflectcolor, uMix );
		if(uTemp >=0.5)
		{
			Alpha = 1 - (uTemp - 0.5);
		}
	}
	else if( uTemp <= 0 )
	{
		refractcolor = mix( refractcolor, ICECOLOR, 1 );
		FragColor = mix( refractcolor, reflectcolor, uMix+0.3 );
	}
	
	

	gl_FragColor = vec4( FragColor, Alpha );
	
}
