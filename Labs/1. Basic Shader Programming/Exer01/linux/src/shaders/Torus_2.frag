#version 330

// Input varibles from Vertex shader
in vec4 theColour;
in float diffTerm_NL; 
in float diffTerm_NV; 


void main()
{
   vec4 grey = vec4(0.2);
   vec4 black = vec4(0);


   // Part 1:  (n.l)
   if (diffTerm_NL < 0.1) gl_FragColor = grey*theColour;
   else gl_FragColor = theColour;

   // Part 2:  (n.v)
   if (abs(diffTerm_NV) < 0.2) gl_FragColor *= black;
}
