#version 100
precision mediump float;

varying  vec2 g_vVSTexCoord;

uniform vec4 g_isEnableBlend;
uniform sampler2D m_texture;


vec4 nv21torgba()
{
    vec2 uv_texcoord;
    vec4 v_rgba_tmp;
    
    uv_texcoord = g_vVSTexCoord;
    uv_texcoord.y = uv_texcoord.y+ g_isEnableBlend.y - g_isEnableBlend.y;
    v_rgba_tmp = texture2D(m_texture, uv_texcoord);
			
    return v_rgba_tmp;
}
void main ()
{
	vec4 rgba_1;
	vec4 rgba;
     
	rgba = nv21torgba();
	rgba_1   = vec4(rgba.xyz,1.0);

	gl_FragColor = rgba_1;
}

