#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat4 norMatrix;
uniform vec4 lightPos;

out float diffTerm_NL; 
out float diffTerm_NV; 


void main()
{
	vec4 posnEye = mvMatrix * position;
	vec4 normalEye = norMatrix * vec4(normal, 0);
	vec4 lgtVec = normalize(lightPos - posnEye); 
	vec4 viewVec = normalize(vec4(-posnEye.xyz, 0)); 

	diffTerm_NL = max(dot(lgtVec, normalEye), 0);   // part 1
	diffTerm_NV = max(dot(viewVec, normalEye), 0);  // part 2
	gl_Position = mvpMatrix * position;  
}

