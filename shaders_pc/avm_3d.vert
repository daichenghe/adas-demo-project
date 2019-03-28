#version 100
precision mediump float;
attribute   vec3 vVertex;
attribute   vec2 vVertexUv;

	

uniform mat4 g_matMVP;

varying   vec2 vUV;
varying   vec2 posion;

void main ()
{
  vec4 tmpvar_1_2=vec4(vVertex.xyz,1.0);
  posion = vec2(vVertex.xy);
  gl_Position = (g_matMVP * tmpvar_1_2);
  vUV = vVertexUv;
}


