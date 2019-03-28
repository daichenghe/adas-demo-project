#extension GL_OES_EGL_image_external : require
precision mediump float;
varying vec3 v_Normal;
varying  vec4 v_Diffuse;
varying vec3 v_ObjP;
 
void main ()
{
  vec3 tmpvar_1;
  tmpvar_1 = normalize(v_Normal);
  mediump vec4 tmpvar_2;
  tmpvar_2.w = 1.0;
  tmpvar_2.xyz = clamp ((clamp (
    (v_Diffuse.xyz * max (dot (tmpvar_1, normalize(
      (vec3(80000.0, 80000.0, 80000.0) - v_ObjP)
    )), 0.0))
  , 0.0, 1.0) + clamp (
    (v_Diffuse.xyz * max (dot (tmpvar_1, normalize(
      (vec3(-80000.0, 80000.0, 80000.0) - v_ObjP)
    )), 0.0))
  , 0.0, 1.0)), 0.0, 1.0);
  gl_FragColor = tmpvar_2;
}