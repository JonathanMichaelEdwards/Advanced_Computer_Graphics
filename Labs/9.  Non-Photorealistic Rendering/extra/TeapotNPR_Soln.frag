#version 330
uniform sampler2D pencilStroke[3];
uniform int flag;

in float diffTerm;
in vec2 texCoord;
out vec4 outColor;
	
void main()
{
   float t;   //Texture blending factor
   vec4 col1, col2;

   if(flag == 1) outColor = vec4(0.0);
   else 
   {

/*
       if(diffTerm < 0)    //without blending
            outColor = texture(pencilStroke[2], texCoord);
       else if(diffTerm < 0.7)
            outColor = texture(pencilStroke[1], texCoord);
       else
            outColor = texture(pencilStroke[0], texCoord);

*/

       if(diffTerm < -0.1)    //with blending
            outColor = texture(pencilStroke[2], texCoord);
       else if(diffTerm < 0.1)
       {
	    t = (diffTerm+0.1)/0.2;
	    col1 = texture(pencilStroke[2], texCoord);
	    col2 = texture(pencilStroke[1], texCoord);
            outColor = (1-t) * col1 + t *col2;
       }
       else if(diffTerm < 0.65)
	    outColor = texture(pencilStroke[1], texCoord);
       else if(diffTerm < 0.75)
       {
	    t = (diffTerm-0.65)/0.1;
	    col1 = texture(pencilStroke[1], texCoord);
	    col2 = texture(pencilStroke[0], texCoord);
            outColor = (1-t) * col1 + t *col2;
        }
       else 
	    outColor = texture(pencilStroke[0], texCoord);


   }

}
