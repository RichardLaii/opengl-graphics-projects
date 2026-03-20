#version 330 compatibility

// you can set these uniform variables dynamically or hardwire them:
uniform float uKa, uKd, uKs; // coefficients of each type of lighting
uniform float uShininess; // specular exponent
uniform float uRedDepth, uBlueDepth;
uniform bool uUseChromaDepth;

in vec3 gN; // Input normal vector
in vec3 gL; // Input light direction
in vec3 gE; // Input eye direction
in float gZ;

vec3
Rainbow( float t )
{
        t = clamp( t, 0., 1. );         // 0.00 is red, 0.33 is green, 0.67 is blue

        float r = 1.;
        float g = 0.0;
        float b = 1.  -  6. * ( t - (5./6.) );

        if( t <= (5./6.) )
        {
                r = 6. * ( t - (4./6.) );
                g = 0.;
                b = 1.;
        }

        if( t <= (4./6.) )
        {
                r = 0.;
                g = 1.  -  6. * ( t - (3./6.) );
                b = 1.;
        }

        if( t <= (3./6.) )
        {
                r = 0.;
                g = 1.;
                b = 6. * ( t - (2./6.) );
        }

        if( t <= (2./6.) )
        {
                r = 1.  -  6. * ( t - (1./6.) );
                g = 1.;
                b = 0.;
        }

        if( t <= (1./6.) )
        {
                r = 1.;
                g = 6. * t;
        }

        return vec3( r, g, b );
}

void main()
{
    vec3 myColor = vec3(0, 0.7, 1);        // whatever default color you'd like
    vec3 mySpecularColor = vec3(1, 1, 1); // whatever default color you'd like

    // here is the per-fragment lighting:
    vec3 ambient = uKa * myColor;

    float d = 0.0;
    float s = 0.0;

    if (dot(gN, gL) > 0.0) // only do specular if the light can see the point
    {
        d = dot(gN, gL);
        vec3 ref = normalize(reflect(-gL, gN)); // reflection vector
        s = pow(max(dot(gE, ref), 0.0), uShininess);
    }
	
	if( uUseChromaDepth )
    {
		float t = (2./3.) * ( abs(gZ) - uRedDepth ) / ( uBlueDepth - uRedDepth );
        t = clamp( t, 0., 2./3. );
        myColor = Rainbow( t );
    }

    vec3 diffuse = uKd * d * myColor;
    vec3 specular = uKs * s * mySpecularColor;
    gl_FragColor = vec4(ambient + diffuse + specular, 1.0);
}