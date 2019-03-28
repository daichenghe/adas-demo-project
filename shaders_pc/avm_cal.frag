#extension GL_OES_EGL_image_external : require
precision mediump float;
uniform sampler2D textureMap1;
uniform samplerExternalOES s_texture;
varying   vec2 g_vVSTexCoord;
varying   vec2 g_vVisColorcorrect;
varying   vec3 rgb;

void main ()
{
   vec4 tmpvar_1;
   vec4 tmpvar_2;
  // vec4 tmpvar_2t;
  
  tmpvar_1 = texture2D(s_texture, g_vVSTexCoord);
  tmpvar_2 = tmpvar_1*vec4(rgb.xyz, 0.0)/texture2D(textureMap1, g_vVSTexCoord);

   //tmpvar_2t = vec4(0.8,0.8,0.8, 0.0);///vec4(255.0,255.0,255.0, 0.0);///tmpvar_1;//tmpvar_2t * (vec4(125.6,123.2,124.0, 0.0)*vec4(0.2568 ,0.5014,0.0979,0));
   //tmpvar_2t =tmpvar_2t/tmpvar_2;//tmpvar_2;

  // tmpvar_2t= tmpvar_2t * tmpvar_1;

  if(g_vVisColorcorrect.x > 0.1)
  {
     vec4 tmpvar_4;
    tmpvar_4.w = 1.0;
    tmpvar_4.xyz = tmpvar_2.xyz;
    gl_FragColor = tmpvar_4;
  }
  else
  {
     gl_FragColor = tmpvar_1;
  }
}

