#extension GL_OES_EGL_image_external : require
precision mediump float;

uniform float alpha_model;
varying   vec2 vUV;
uniform sampler2D textureMap;

void main ()
{
  vec4 rgb_texture;
  rgb_texture = texture2D(textureMap, vec2(vUV.x,1.0-vUV.y));	
  rgb_texture.w = alpha_model;
  gl_FragColor = rgb_texture;
  
}

