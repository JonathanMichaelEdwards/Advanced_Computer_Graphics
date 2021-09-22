/* File contains only fragments. */

#version 410 


/* Uniform textures */
uniform sampler2D smokeTex[4];

flat in int tex_index;  // get this from the vert. shader
in float tex_dist;

out vec4 oColor;
void main() 
{
   vec4 col;
   float alpha;

   col = texture(smokeTex[tex_index], gl_PointCoord); 

   if (col.r > 0.9 && col.g > 0.9 && col.b > 0.9) 
      discard;

   if (tex_dist < 1)
      alpha = 0.25;
   else if (tex_dist < 3)
      alpha = 0.2;
   else if (tex_dist < 5)
      alpha = 0.1;
   else
      alpha = 0.05;

   oColor = vec4(col.rgb, alpha);  // 50% transparency for blending
}
	
