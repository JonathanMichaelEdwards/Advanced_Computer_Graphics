#version 330

uniform sampler2D spriteTex;
uniform float aspect;
out vec4 color;


void main() 
{
    // color = texture(spriteTex, gl_PointCoord);  // old


    /* Mapping a non-square texture, lec 3 - pg 18*/
    // aspect = Aspect ratio (width / height)
    float d1 = 0.5 * (1 - aspect);  // point A
    float d2 = 0.5 * (1 + aspect);  // point B
    
    if (gl_PointCoord.x < d1 || gl_PointCoord.x > d2)
        color = vec4(0);
    else 
    {
        float tcoordx = (gl_PointCoord.x - d1) / (d2 - d1);
        float tcoordy = gl_PointCoord.y; 
        color = texture(spriteTex, vec2(tcoordx, tcoordy));  // output reformated texture
    }
}
	
