#version 100
precision mediump float;
uniform mat4 g_matMVP;
uniform  mat4 g_matModelView;
uniform  int g_is3d;
uniform  mat4 g_matProj;
attribute   vec2 g_vPosition;
attribute  vec4 g_vcolor;

varying   vec4 rgb;
void main ()
{
  vec4 tmpvar_1 = vec4(g_vPosition.xy,0,1.0);
  if(g_is3d>0)
  {
     vec4 tmpvar_3 = (g_matModelView * tmpvar_1);
     gl_Position = (g_matProj * tmpvar_3);
  }
  else
  {
     gl_Position  = (g_matMVP * tmpvar_1);
  }
  rgb = g_vcolor;
}

