#version 330


/* Program Uniforms */
uniform sampler2D pencil_stroke[5];
uniform int render_mode;
uniform int pencil_blending_mode;
uniform float thickness_silhouette;

in float diffTerm;
in vec2 tex_coords;
in float normals[4];  // 4 faces to compare from
out vec4 outColor;


void main() 
{
    float t;   //Texture blending factor
    vec4 col1, col2;
    vec4 material = vec4(0.75, 0.61, 0.42, 1.0);    // Cammel color


    // -- Two-tone shading ------------------------------------------------------
    if (render_mode == 0)  
    {
        if (diffTerm < 0.7)
            outColor = vec4(0.3) * material;
        if (diffTerm >= 0.7) 
            outColor = vec4(0.8) * material;
    }
    // -------------------------------------------------------------------------



    // -- Pencil shading ------------------------------------------------------
    if (render_mode == 1)
    {        
        if (pencil_blending_mode == 1)  // Blending weights ON ------------------
        {
            if (diffTerm < 0.55)
                outColor = vec4(0.9) * texture(pencil_stroke[4], tex_coords);
            if (diffTerm >= 0.55 && diffTerm < 0.65) 
            {
                t = (diffTerm-0.3) / 0.2;
                col1 = texture(pencil_stroke[4], tex_coords);
                col2 = texture(pencil_stroke[3], tex_coords);
                outColor = vec4(0.75) * ((1-t)*col1 + t * col2);
            } 
            else if (diffTerm >= 0.65 && diffTerm < 0.75) 
            {
                t = (diffTerm-0.3) / 0.2;
                col1 = texture(pencil_stroke[3], tex_coords);
                col2 = texture(pencil_stroke[2], tex_coords);
                outColor = vec4(0.65) * ((1-t)*col1 + t * col2);
            } 
            else if (diffTerm >= 0.75 && diffTerm < 0.85) 
            {
                t = (diffTerm-0.3) / 0.2;
                col1 = texture(pencil_stroke[2], tex_coords);
                col2 = texture(pencil_stroke[1], tex_coords);
                outColor = vec4(0.60) * ((1-t)*col1 + t * col2);
            } 
            else if (diffTerm >= 0.85 && diffTerm < 0.95) 
            {
                t = (diffTerm-0.7) / 0.1;
                col1 = texture(pencil_stroke[2], tex_coords);
                col2 = texture(pencil_stroke[1], tex_coords);
                outColor = vec4(0.50) * ((1-t) * col1 + t * col2);
            } 
            else if (diffTerm >= 0.95) 
            {
                t = (diffTerm-0.95) / 0.7;
                col1 = texture(pencil_stroke[1], tex_coords);
                col2 = texture(pencil_stroke[0], tex_coords);
                outColor = vec4(0.6) * ((1-t) * col1 + t * col2);
            }
        }
        if (pencil_blending_mode == 0)  // Blending OFF ------------------
        {
            if (diffTerm < 0.7)
                outColor = vec4(0.4) * texture(pencil_stroke[3], tex_coords);  // darker
            if (diffTerm >= 0.7 && diffTerm < 0.9)
                outColor = vec4(0.6) * texture(pencil_stroke[2], tex_coords);  // medium
            if (diffTerm >= 0.9) 
                outColor = vec4(0.8) * texture(pencil_stroke[0], tex_coords);  // lighter
        }
    }
    // ---------------------------------------------------------------------------


    // -- Silhouette Edge Identification Levelling ------------------------------
    if (normals[1] < thickness_silhouette && normals[0] > 0)
         outColor = vec4(0.0);
    else if (normals[2] < thickness_silhouette && normals[0] > 0)
        outColor = vec4(0.0);
    else if (normals[3] < thickness_silhouette && normals[0] > 0)
        outColor = vec4(0.0);
    // ---------------------------------------------------------------------------
}


