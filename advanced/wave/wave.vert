#version 330 compatibility
uniform float uTimeScale;

uniform int uwave;

uniform float uAm0;
uniform float uKm0;
uniform float uGamma0;
uniform float uTemp;

uniform float Timer;

uniform float uLightX, uLightY, uLightZ;

vec3 eyeLightPosition = vec3( uLightX, uLightY, uLightZ );

out vec3 vMC;
out vec3 vEs;
out vec3 vLs;
out vec3 vNs;
out vec4 vPos;

const float PI = 3.14159265;
const float G = 1.;

const float e = 2.71828;

float cotan(float x) 
{ 
	return(1 / tan(x)); 
}

float sign(float x)
{
	if (x > 0)
		return 1;
	if (x < 0)
		return -1;
	return 0;
}

void
main( )
{
		vPos = gl_Vertex;
		float newx = gl_Vertex.x;
		float newy = 0.;
		float newz = gl_Vertex.z;
		float dxda = 1.;
		float dyda = 0.;
		float dzda = 0.;
		float dxdb = 0.;
		float dydb = 0.;
		float dzdb = 1.;
		
		
		if (uwave == 0) // Gerstner wave
		{
			{
				float phiM0 = 0.; 
				float wm0 = sqrt( G*uKm0 );
				float thetam;
				if (uTemp <= 0)
					thetam = gl_Vertex.x*uKm0*cos(uGamma0)+ gl_Vertex.z*uKm0*sin(uGamma0) - phiM0;
				else
					thetam = gl_Vertex.x*uKm0*cos(uGamma0)+ gl_Vertex.z*uKm0*sin(uGamma0) - wm0*Timer*uTimeScale - phiM0;
				
				newx -= uAm0*cos(uGamma0)*sin(thetam);
				newy += uAm0 * cos(thetam);
				newz -= uAm0*sin(uGamma0)*sin(thetam);
				float dthetamda = uKm0*cos(uGamma0);
				float dthetamdb = uKm0*sin(uGamma0);
				dxda -= uAm0*cos(uGamma0)*cos(thetam)*dthetamda;
				dyda -= uAm0*sin(thetam)*dthetamda;
				dzda -= uAm0*sin(uGamma0)*cos(thetam)*dthetamda;
				dxdb -= uAm0*cos(uGamma0)*cos(thetam)*dthetamdb;
				dydb -= uAm0*sin(thetam)*dthetamdb;
				dzdb -= uAm0*sin(uGamma0)*cos(thetam)*dthetamdb;
			}
			
			
		}
		else if (uwave == 1) // sinusoidal wave
		{
			{
				float phiM0 = 0.; 
				float wm0 = sqrt( G*uKm0 );
				float thetam;
				if (uTemp <= 0)
					thetam = gl_Vertex.x * uKm0 + phiM0 - uGamma0;
				else
					thetam = gl_Vertex.x * uKm0 - wm0*Timer*uTimeScale + phiM0 - uGamma0;
					
				newx = gl_Vertex.x;
				newy += uAm0 * sin(thetam);
				newz = gl_Vertex.z;
				float dthetamda = uKm0;
				float dthetamdb = 0;
				dxda -= 1;
				dyda += uAm0 * cos(thetam) * dthetamda;
				dzda -= 1;

				dxdb -= 1;
				dydb += uAm0 * cos(thetam) * dthetamdb;
				dzdb -= 1;
			}
			
		}
		else if (uwave == 2) // Square Wave
		{
			{
				float phiM0 = 0.; 
				float wm0 = sqrt( G*uKm0 );
				float thetam;
				if (uTemp <= 0)
					thetam = gl_Vertex.x * uKm0 + phiM0 - (uGamma0 / 180) * PI;
				else
					thetam = gl_Vertex.x * uKm0 - wm0*Timer*uTimeScale + phiM0 - (uGamma0 / 180) * PI;
				newx = gl_Vertex.x;
				newy += sign(sin(thetam));
				newz = gl_Vertex.z;
				float dthetamda = uKm0;
				float dthetamdb = 0;
				dxda -= 1;
				dyda += sign(sin(thetam)) * uKm0;
				dzda -= 1;

				dxdb -= 1;
				dydb += 0;
				dzdb -= 1;
			}
			
		}
		else if (uwave == 3) // Sawtooth Wave 
		{
			{
				float phiM0 = 0.; 
				float wm0 = sqrt( G*uKm0 );
				float thetam;
				if (uTemp <= 0)
					thetam = gl_Vertex.x * uKm0 * (Timer*0.0001 - phiM0);
				else
					thetam = gl_Vertex.x * uKm0 * (Timer*uTimeScale / 2 - phiM0);
				newx = gl_Vertex.x;
				newy += (2 * uAm0 / PI) * atan(tan(thetam)) * cotan(uGamma0);
				newz = gl_Vertex.z;
				float dthetamda = uKm0 * (Timer*uTimeScale / 2 - phiM0);
				float dthetamdb = 0;
				dxda += dthetamda;
				dyda += (2 * uAm0 / PI) * atan(tan(thetam)) * cotan(uGamma0) * dthetamda;
				dzda += dthetamda;

				dxdb -= 1;
				dydb += 0;
				dzdb -= 1;
			}
		}
		else if (uwave == 4) // Water Wave (Stokes Wave)
		{
			{
				float phiM0 = 0.; 
				float wm0 = sqrt( G*uKm0 );
				float thetam;
				if (uTemp <= 0)
					thetam = gl_Vertex.x * uKm0 * cos(uGamma0) + phiM0;
				else
					thetam = gl_Vertex.x * uKm0 * cos(uGamma0) - wm0*Timer*uTimeScale + phiM0;
				newx = gl_Vertex.x;
				
				if (uTemp <= 0)
					newy += uAm0*cos(thetam) + (uKm0/2) * uAm0 * pow(2/uKm0, 2);
				else
					newy += uAm0*cos(thetam) + (uKm0/2) * uAm0 * pow(2/uKm0, 2) * cos(2*wm0*Timer*uTimeScale);
				newz = gl_Vertex.z;
				float dthetamda = uKm0*cos(uGamma0);
				float dthetamdb = 0;
				dxda -= 1;
				dyda += uAm0*cos(thetam)*dthetamda;
				dzda -= 1;

				dxdb -= 1;
				dydb += 0;
				dzdb -= 1;
			}
		}
		
		
		
	
		vec3 newVertex = vec3( newx, newy, newz );
		vMC = newVertex;
		
		vec3 ta = vec3( dxda, dyda, dzda );
		vec3 tb = vec3( dxdb, dydb, dzdb );
		vNs = normalize( gl_NormalMatrix*cross( tb, ta ) );
		// surface normal vector
		vec4 ECposition = gl_ModelViewMatrix * vec4( newVertex, 1. );
		vLs = normalize( eyeLightPosition - ECposition.xyz ); // vector from the point
		// to the light position
		vEs = normalize( vec3( 0., 0., 0. ) - ECposition.xyz ); // vector from the point
		// to the eye position
		gl_Position = gl_ModelViewProjectionMatrix * vec4( newVertex, 1.);
}

