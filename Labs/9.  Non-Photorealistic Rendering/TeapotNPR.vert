#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;

uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat4 norMatrix;
uniform vec4 lightPos;

out float diffTerm;
out vec4 phongColor;

void main()
{
	vec4 posnEye = mvMatrix * position;
	vec4 vNormal = norMatrix * vec4(normal, 0);
	vec4 lgtVec = normalize(lightPos - posnEye); 
	vec4 viewVec = - normalize(vec4(posnEye.xyz, 0));
	vec4 halfVec = normalize(lgtVec + viewVec);
	vec4 grey = vec4(0.2, 0.2, 0.2, 1.0);
	vec4 white = vec4(1);
	vec4 material = vec4(1.0, 1.0, 0.0, 1.0);
	vec4 ambOut = grey * material;
 	float shininess = 100.0;
	diffTerm = dot(lgtVec, vNormal);
	vec4 diffOut = material * diffTerm;
	float specTerm = dot(halfVec, vNormal);
	vec4 specOut = white *  pow(specTerm, shininess);
	phongColor = vec4(ambOut.xyz + diffOut.xyz + specOut.xyz, 1);
	gl_Position = mvpMatrix * position;
}

