#version 100
precision mediump float;

uniform vec4 g_isEnableBlend;
uniform vec3      alpha_win;
uniform vec3 g_hsv;
varying   vec2 vUV;
varying   vec2 posion;
 
	

uniform sampler2D textureMap;
uniform sampler2D textureMap1;


vec4 nv21torgba()
{
    vec2 uv_texcoord;
    vec4 v_rgba;
    vec2 v_texcoord;

    uv_texcoord = vUV;
    uv_texcoord.y = uv_texcoord.y+ g_isEnableBlend.y - g_isEnableBlend.y;
    v_rgba = texture2D(textureMap, uv_texcoord);

    return  v_rgba;
}

void main ()
{
   vec4 tmpvar_1;
   vec4 rgba;
   vec4 rgba_1;
   vec2 posion_avm;

   vec2 uv_texcoord;
   vec2 v_texcoord;

   rgba = nv21torgba();
   rgba.xyz *= g_hsv;
   uv_texcoord = vUV ;
   posion_avm.x = (posion.x/alpha_win.z)/(alpha_win.x/2.0);
   posion_avm.y = (posion.y/alpha_win.z)/(alpha_win.y/2.0);
   tmpvar_1 = texture2D(textureMap1, vec2((posion_avm.x+1.0)/2.0  , abs((posion_avm.y-1.0)/2.0)*0.25 + g_isEnableBlend.y*0.25));

   if((uv_texcoord.x < -1.0)||(uv_texcoord.y < -1.0))
   {
      rgba_1 = vec4(rgba.xyz,0.0);	
   }
   else
   {
      rgba_1 = vec4(rgba.xyz,tmpvar_1.w);
   }
  
   gl_FragColor = rgba_1;

}

