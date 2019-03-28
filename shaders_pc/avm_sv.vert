#version 100
precision mediump float;

uniform mat4 g_matMVP;

attribute   vec2 g_vPosition;
attribute   vec2 g_vTexCoord;
varying   vec2 g_vVSTexCoord;

void main ()
{
  vec4 tmpvar_1 = vec4(g_vPosition.xy,0.0, 1.0);
  gl_Position = (g_matMVP * tmpvar_1);
  g_vVSTexCoord = g_vTexCoord;
}

