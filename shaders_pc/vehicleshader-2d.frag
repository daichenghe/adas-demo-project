#extension GL_OES_EGL_image_external : require
precision mediump float;
uniform sampler2D m_texture;
varying vec2 g_vVSTexCoord;
 
void main ()
{
  gl_FragColor = texture2D (m_texture, g_vVSTexCoord);
}

