#version 330 compatibility
#extension GL_EXT_gpu_shader4: enable
#extension GL_EXT_geometry_shader4: enable

layout( triangles )  in;
layout( triangle_strip, max_vertices=204 )  out;

out vec3	gN;		 // normal vector
out vec3	gL;		 // vector from point to light
out vec3	gE;		 // vector from point to eye
out float	gZ;

vec3  V0, V1, V2;
vec3  V01, V02;
vec3  CG;
uniform float uDiam;
uniform int uLevel;
uniform float uQuantize;
vec3 LIGHTPOS = vec3( 25., 10., 10. );

float
Quantize( float f )
{
    f *= uQuantize;
    int fi = int( f );
    f = float( fi ) / uQuantize;
    return f;
}

void ProduceVertex(float s, float t)
{
    
	vec3 v = V0 + s * V01 + t * V02;
    v -= CG;
    v = normalize(v) * (uDiam / 2.0);
    v += CG;

    vec4 ECposition = gl_ModelViewMatrix * vec4(v, 1.0);

    vec3 n = normalize(v - CG);

    n = normalize(gl_NormalMatrix * n);

    gN = n;
    gL = normalize(LIGHTPOS - v);
    gE = normalize(v);
	gZ = -ECposition.z;

    gl_Position = gl_ProjectionMatrix * ECposition;
    EmitVertex();
}

void main( )
{
    V0  =   gl_PositionIn[0].xyz;
    V1  =   gl_PositionIn[1].xyz;
    V2  =   gl_PositionIn[2].xyz;
    V01 = V1 - V0;
    V02 = V2 - V0;

    CG = ( V0 + V1 + V2 ) / 3.;
    CG.x = Quantize( CG.x );
	CG.y = Quantize( CG.y );
	CG.z = Quantize( CG.z );

    int numLayers = 1 << uLevel;
	
	
	float dt = 1. / float( numLayers );
	float t_top = 1.;

	for (int it = 0; it < numLayers; it++)
	{
		float t_bot = t_top - dt;
		float smax_top = 1.0 - t_top;
		float smax_bot = 1.0 - t_bot;
		
		int nums = it + 1;
		float ds_top = smax_top / float(nums - 1);
		float ds_bot = smax_bot / float(nums);
		
		float s_top = 0.0;
		float s_bot = 0.0;

		for (int is = 0; is < nums; is++)
		{
			ProduceVertex(s_bot, t_bot);
			ProduceVertex(s_top, t_top);
			s_top += ds_top;
			s_bot += ds_bot;
		}

		ProduceVertex(s_bot, t_bot);
		EndPrimitive();

		t_top = t_bot;
		t_bot -= dt;
	}
	
	
    //EndPrimitive();
}