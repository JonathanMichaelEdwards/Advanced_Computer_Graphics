#version 330

in float diffTerm_NL; 
in float diffTerm_NV; 
out vec4 outputColor;


void main()
{
   vec4 material = vec4(1.0, 0.64, 0.45, 1.0);
   vec4 grey = vec4(0.2);
   vec4 black = vec4(0);
   
   // Part 1:  (n.l)
   outputColor = (diffTerm_NL < 0.2) ? (grey*material) : material;

   // Part 2:  (n.v)
   if (diffTerm_NV < 0.2) outputColor *= black;
}
