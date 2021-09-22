#version 410 

// Only fragments

/* Program Uniforms */
//uniform int texIndex; // tex index

/* Uniform textures */
uniform sampler2D smokeTex[4];
// uniform sampler2D smokeTex1;
// uniform sampler2D smokeTex2;
// uniform sampler2D smokeTex3;
// uniform sampler2D smokeTex4;

flat in int tex_index;  // send this to the frag. shader 


out vec4 oColor;
void main() 
{
   vec4 col;

   // if (texIndex <= 100)
   //    col = texture(smokeTex4, gl_PointCoord); 
   // else 

   col = texture(smokeTex[tex_index], gl_PointCoord); 

   if (col.r > 0.9 && col.g > 0.9 && col.b > 0.9) 
      discard;




   oColor = vec4(col.rgb, 0.5);  // 50% transparency for blending
}
	
