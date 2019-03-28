#version 100
precision mediump float;
attribute vec3 g_vPosition;
attribute vec3 g_vNormal;
attribute vec4 g_vDiffuse;
uniform  mat4 g_matModelView;
uniform  mat4 g_matProj;
varying    vec3 v_Normal;
varying    vec4 v_Diffuse;
varying      vec3 v_ObjP;
void main ()
{
  vec4 tmpvar_1;
  tmpvar_1.w = 1.0;
  tmpvar_1.xyz = g_vPosition;
   vec4 tmpvar_2;
  tmpvar_2 = (g_matModelView * tmpvar_1);
  gl_Position = (g_matProj * tmpvar_2);
  v_Normal = g_vNormal;
  v_Diffuse = g_vDiffuse;
  v_ObjP = tmpvar_2.xyz;
}

