#version 400

uniform int flag;

in float diffTerm;  // l.n
out vec4 outColor;


void main()
{
   vec4 _out_color;
   int _flag = 0;

   if (diffTerm < 0);
   {
      _out_color = vec4(0.2, 0.2, 0.2, 1);
   }
   if (0 < diffTerm && diffTerm < 0.7)
   {
      _out_color = vec4(0.5, 0.5, 0, 1);
   }
   if (diffTerm > 0.7)
   {
      _out_color = vec4(1, 1, 0, 1);
   }

   _flag = flag;
   
   if (flag > 1)
      _out_color = vec4(0, 0, 0, 1);

   outColor = _out_color;
}

