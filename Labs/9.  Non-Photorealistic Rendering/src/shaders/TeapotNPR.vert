#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;

uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat4 norMatrix;
uniform vec4 lightPos;

out float diffTerm;
out vec2 texCoord;

void main()
{
	vec4 posnEye = mvMatrix * vec4(position, 1);
	vec4 normEye = norMatrix * vec4(normal, 0);
	vec4 lgtVec = normalize(lightPos - posnEye); 
	vec3 unitNorm = normalize(normEye.xyz);	
	diffTerm = dot(lgtVec.xyz, unitNorm);
	gl_Position = mvpMatrix * vec4(position,1);

	int k = int(mod(gl_VertexID, 3));
	if(k == 0) 
		texCoord = vec2(0, 0);
    else if (k == 1) 
		texCoord = vec2(0.5, 0);
    else 
		texCoord = vec2(0.25, 0.5);
}
