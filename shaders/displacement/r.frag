
#version 330 compatibility
// you can set these uniform variables  dynamically or hardwire them:
uniform float uKa, uKd, uKs; // coefficients of each type of lighting
uniform float uShininess; // specular exponent
uniform sampler3D Noise3;

uniform float uA, uB, uC, uD;
uniform float uNoiseFreq, uNoiseAmp;
uniform float uLightX, uLightY, uLightZ; 
uniform vec4 uColor, uSpecularColor;


// in variables from the vertex shader:
in vec2 vST; // texture cords
in vec3 vN; // normal vector
in vec3 vL; // vector from point to light
in vec3 vE; // vector from point to eye
in vec3 vMC;

vec3
RotateNormal( float angx, float angy, vec3 n )
{
        float cx = cos( angx );
        float sx = sin( angx );
        float cy = cos( angy );
        float sy = sin( angy );

        // rotate about x:
        float yp =  n.y*cx - n.z*sx;    // y'
        n.z      =  n.y*sx + n.z*cx;    // z'
        n.y      =  yp;
        // n.x      =  n.x;

        // rotate about y:
        float xp =  n.x*cy + n.z*sy;    // x'
        n.z      = -n.x*sy + n.z*cy;    // z'
        n.x      =  xp;
        // n.y      =  n.y;

        return normalize(n );
}

void
main( )
{
	vec3 Normal = normalize(vN);
	vec3 Light = normalize(vL);
	vec3 Eye = normalize(vE);
	vec3 myColor = vec3( 1, 1, 1 );		// whatever default color you'd like
	vec3 mySpecularColor = vec3( 1, 0, 1 );	// whatever default color you'd like
	
	vec4 nvx = texture( Noise3, uNoiseFreq*vMC );
	float angx = nvx.r + nvx.g + nvx.b + nvx.a  -  2.;
	angx *= uNoiseAmp;

    vec4 nvy = texture( Noise3, uNoiseFreq*vec3(vMC.xy,vMC.z+0.5) );
	float angy = nvy.r + nvy.g + nvy.b + nvy.a  -  2.;
	angy *= uNoiseAmp;
	
	vec3 tnorm = normalize(RotateNormal(angx,angy,vN));
	
	vec3 ambient = uKa * uColor.rgb;
    float d = max(dot(tnorm,vL), 0.);
    vec3 diffuse = uKd * d * uColor.rgb;

    vec3 specular = uKs * 0.0 * uSpecularColor.rgb;
    if(dot(tnorm,vL) > 0.){
        vec3 ref = normalize(2. * tnorm * dot(tnorm,vL) - vL);
        float t = pow( max( dot(vE,ref),0. ), uShininess );
		specular = uKs * t * uSpecularColor.rgb;
    }
    gl_FragColor = vec4(ambient.rgb + diffuse.rgb + specular.rgb, 1.);
}