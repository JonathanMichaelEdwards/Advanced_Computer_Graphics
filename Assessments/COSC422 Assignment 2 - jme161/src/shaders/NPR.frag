#version 330


/* Program Uniforms */
uniform sampler2D pencil_stroke[5];
uniform int render_mode;
uniform int pencil_blending_mode;

in float diffTerm;
in vec4 lighting_color;
in vec2 tex_coords;
out vec4 outColor;


void main() 
{
    //outColor = lighting_color;
    outColor = texture(pencil_stroke[0], tex_coords) * lighting_color;

//     // Two-tone shading
//    if (render_mode == 0)  
//       if (diffTerm < 0)
//         outColor = vec4(0.2, 0.2, 0.2, 1);
//       if (0 < diffTerm && diffTerm < 0.7)
//         outColor = vec4(0.5, 0.5, 0, 1);
//       if (diffTerm > 0.7) 
//         outColor = vec4(1, 1, 0, 1);

//     // Pencil shading
//     if (render_mode == 1)
//     {        
//         if (pencil_blending_mode == 1)  // Blending weights ON
//         {
//             // ... weights
//             outColor = vec4(1, 0, 0, 1);
//         }
//         if (pencil_blending_mode == 0)  // Blending OFF
//         {
//             outColor = vec4(0, 0, 1, 1);  
//         }
//     }
}



///
   // float t;   //Texture blending factor
   // vec4 col1, col2;


   // if(flag == 1) 
   //    outColor = vec4(0.0);
   // else
   // { 
   //    if (diffTerm < -0.1)    // with blending
   //       outColor = texture(pencilStroke[2], texCoord);
   //    else if (diffTerm < 0.1)
   //    {
   //       t = (diffTerm+0.1)/0.2;
   //       col1 = texture(pencilStroke[2], texCoord);
   //       col2 = texture(pencilStroke[1], texCoord);
   //       outColor = (1-t) * col1 + t *col2;
   //    }
   //    else if (diffTerm < 0.65)
   //       outColor = texture(pencilStroke[1], texCoord);
   //    else if (diffTerm < 0.75)
   //    {
   //       t = (diffTerm-0.65)/0.1;
   //       col1 = texture(pencilStroke[1], texCoord);  // blending
   //       col2 = texture(pencilStroke[0], texCoord);
   //       outColor = (1-t) * col1 + t *col2;
   //    }
   //    else 
   //       outColor = texture(pencilStroke[0], texCoord);
   // }

