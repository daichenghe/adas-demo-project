#extension GL_OES_EGL_image_external : require
precision mediump float;

varying  vec4 rgb;
 
void main ()
{
    gl_FragColor = rgb;
}

