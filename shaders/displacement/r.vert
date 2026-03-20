// make this 120 for the mac:
#version 330 compatibility

uniform float uLightX;
uniform float uLightY;
uniform float uLightZ;

uniform float uA, uB, uC, uD;

out vec2 vST; // texture coords
out vec3 vN; // normal vector
out vec3 vL; // vector from point to light
out vec3 vE; // vector from point to eye
out vec3 vMC;
out vec3 vECposition;

vec4 LIGHTPOS = vec4(uLightX,uLightY,uLightZ, 1.);

#define PI 3.1415926535897932384626433832795

void
main( )
{
	vec4 p = gl_Vertex;
	vST = gl_MultiTexCoord0.st;
	vMC = gl_Vertex.xyz;
	vECposition = ( gl_ModelViewMatrix * p ).xyz;
	
	
	float r = sqrt( pow(p.x, 2) + pow(p.y, 2));
	float dzdr = uA * ( -sin(2.*PI*uB*r+uC) * 2.*PI*uB * exp(-uD*r) + cos(2.*PI*uB*r+uC) * -uD * exp(-uD*r) );
	float drdx = p.x / r;
	float drdy = p.y / r;
	float dzdx = dzdr * drdx;
	float dzdy = dzdr * drdy;

	vec3 Tx = vec3(1., 0., dzdx );
	vec3 Ty = vec3(0., 1., dzdy );

	p.z = uA * cos(2*PI*uB*r + uC) * exp(-uD*r);
	
	vec4 mixLight =  gl_ModelViewMatrix* LIGHTPOS;
	vec3 tnorm = normalize(gl_NormalMatrix * gl_Normal);
	vN = normalize(gl_NormalMatrix * normalize(cross(Tx, Ty)));
	vL = normalize(mixLight.xyz - vECposition);
	vE = normalize(vec3(0.0, 0.0, 0.0) - vECposition);
	gl_Position = gl_ModelViewProjectionMatrix * p;
	
}