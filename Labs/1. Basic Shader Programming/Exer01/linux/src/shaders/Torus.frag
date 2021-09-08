#version 330

in vec4 theColour;


// built-in variable "gl_FragCoord"
// gl_FragCoord.z - gives the depth of the fragment in the range [0, 1].
void main()
{
   // gl_FragColor = vec4(1.0, 0.5, 0.25, 1.0); // setting the colour
   // gl_FragColor = vec4(gl_FragCoord.z, gl_FragCoord.z, gl_FragCoord.z, 1.0); // setting the colour - with frag coord 
   gl_FragColor = theColour;
}
