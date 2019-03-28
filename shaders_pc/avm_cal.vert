#version 100
precision mediump float;
uniform mat4 g_matMVP;
uniform vec2 g_isColorcorrect;
uniform vec3 g_color;
attribute  vec2 g_vPosition;
attribute  vec2 g_vTexCoord;
varying   vec2 g_vVSTexCoord;
varying   vec2 g_vVisColorcorrect;
varying   vec3 rgb;
void main ()
{
  vec4 tmpvar_1;
  tmpvar_1.zw = vec2(0.0, 1.0);
  tmpvar_1.xy = g_vPosition;
  gl_Position = g_matMVP*tmpvar_1;
  g_vVSTexCoord = g_vTexCoord;
  g_vVisColorcorrect=g_isColorcorrect;
  rgb = g_color;
}

