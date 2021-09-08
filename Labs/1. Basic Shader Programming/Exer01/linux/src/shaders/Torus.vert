#version 330

layout (location = 0) in vec4 position;
layout (location = 1) in vec3 normal;
uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat4 norMatrix;
uniform vec4 lightPos;

out vec4 theColour;


// Contains code for lighting calculations and 
// the transformation of vertex coordinates to the clip coordinate space - Positions are not avaliable
// gl_Position is the built-in output variable 
void main()
{
	vec4 white = vec4(1.0);
	vec4 grey = vec4(0.2);
	vec4 cyan = vec4(0.0, 1.0, 1.0, 1.0);

	vec4 posnEye = mvMatrix * position;
	vec4 normalEye = norMatrix * vec4(normal, 0);
	vec4 lgtVec = normalize(lightPos - posnEye); 
	vec4 viewVec = normalize(vec4(-posnEye.xyz, 0)); 
	vec4 halfVec = normalize(lgtVec + viewVec); 
	vec4 material = vec4(0.0, 1.0, 1.0, 1.0);  // cyan
	vec4 ambOut = grey * material;
	float shininess = 100.0;
	float diffTerm = max(dot(lgtVec, normalEye), 0);
	vec4 diffOut = material * diffTerm;
	float specTerm = max(dot(halfVec, normalEye), 0);
	vec4 specOut = white *  pow(specTerm, shininess);
	
	// Output clip coordinates
	gl_Position = mvpMatrix * position;
	theColour = ambOut + diffOut + specOut;
}

