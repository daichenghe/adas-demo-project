#version 100
precision mediump float;
attribute vec3 g_vPosition;
attribute vec2 g_vertexUv;

uniform  mat4 g_matModelView;
uniform  mat4 g_matProj;

varying	   vec2 vUV;

void main ()
{
  vec4 tmpvar_1=vec4(g_vPosition.xyz,1.0);
  vec4 tmpvar_2 = (g_matModelView * tmpvar_1);
  gl_Position = (g_matProj * tmpvar_2);
  vUV = g_vertexUv;
  
}

