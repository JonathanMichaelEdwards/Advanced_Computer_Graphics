#version 330

uniform sampler2D smokeTex[4];
flat in int indx;
out vec4 oColor;
void main() 
{
/*
   vec3 temp;
   float alpha, scale;
   if(valid < 0) discard;    
   vec4 col = texture(smokeTex[indx], gl_PointCoord); 
   if(col.r > 0.9 && col.g > 0.9 && col.b > 0.9) discard;
   if(tfrac <= 0.3)
   {
       alpha = 0.5;
       temp = col.rgb;
   }
   if(tfrac > 0.3 && tfrac < 0.6)
   { 
        scale = (0.6-tfrac)/0.3;
        temp = (col.rgb) * scale;
        alpha = 0.5;
   }
   else if(tfrac >= 0.6)
   {
       alpha = 5*(1-tfrac)/4.0;
       temp = vec3(0); 
   }            

   gl_FragColor = vec4 (temp, alpha);
*/
   oColor = vec4(0, 0, 1, 1);

}
	
