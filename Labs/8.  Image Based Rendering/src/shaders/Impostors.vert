#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 cols;

uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat4 norMatrix;
uniform mat4 mvpMatrixS;

uniform vec4 lightPos;
uniform int pass;
out vec4 vColour;

void main()
{
	vec4 posnEye = mvMatrix * vec4(position, 1.0);
	vec4 normalEye = norMatrix * vec4(normal, 0);
	vec4 lgtVec = normalize(lightPos - posnEye); 

	vec4 material = vec4(0.0, 0.5, 1.0, 1.0);
	vec4 lgtAmb = vec4(0.2 * material.xyz, 1);   //Ambient term
	float diffTerm = max(dot(lgtVec, normalEye), 0);
	vec4 lgtDiff = material * diffTerm;  		  //Diffuse term
	gl_Position = mvpMatrix * vec4(position, 1.0);
	vColour = lgtAmb + lgtDiff;
}

