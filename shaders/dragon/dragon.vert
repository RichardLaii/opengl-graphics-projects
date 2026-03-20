#version 330 compatibility
#define M_PI 3.14159

uniform sampler3D Noise3; //3D noise texture
uniform float Timer;
uniform float MorphFactor;

out vec2 vST;

void main ( ) {
    
	float Time;
	if (Timer > 0.5)
		Time = 1 - Timer;
	else 
		Time = Timer;
	
    vST = gl_MultiTexCoord0.st;
    vec3 vMCposition  = gl_Vertex.xyz;
    vec3 vN = normalize(gl_NormalMatrix * gl_Vertex.xyz);

    vec4 nv = texture3D(Noise3, vMCposition);
    float n = (nv.r + nv.g + nv.b + nv.a) - 10.;

    vec4 glVertex1 = vec4(gl_Vertex.xyz + (vN * Time * cos(Time + n * 300)* 0.5), 1.);
	
	vec4 vertex = gl_Vertex;
	vertex.xyz *= 4. / length(vertex.xyz);

    
	vec4 mixVertex = mix(glVertex1, vertex, MorphFactor);
	gl_Position = gl_ModelViewProjectionMatrix * mixVertex;
}
