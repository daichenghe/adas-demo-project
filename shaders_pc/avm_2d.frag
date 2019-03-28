#version 100
precision mediump float;

varying  vec2 g_vVSTexCoord;
varying  vec2 posion;

uniform vec4 g_isEnableBlend;
uniform sampler2D m_texture;
uniform sampler2D   textureMap1;
uniform vec3 g_hsv;


vec4 nv21torgba()
{
    vec2 uv_texcoord;
    vec4 v_rgba_tmp;
    vec4 v_test;
    uv_texcoord = g_vVSTexCoord;
    uv_texcoord.y = uv_texcoord.y+ g_isEnableBlend.y - g_isEnableBlend.y;
    v_rgba_tmp = texture2D(m_texture, uv_texcoord);
    return v_rgba_tmp;
}

void main ()
{
  vec4 rgba_1;
  vec4 rgba;
  vec4 tmpvar_1;

  rgba = nv21torgba();
  tmpvar_1 = texture2D(textureMap1, vec2((posion.x+1.0)/2.0, abs((posion.y-1.0)/2.0)*0.25 + g_isEnableBlend.y * 0.25));
  rgba.xyz *= g_hsv;
  rgba_1   = vec4(rgba.xyz,tmpvar_1.w);
  gl_FragColor = rgba_1;

}
