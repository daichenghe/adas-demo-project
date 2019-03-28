#version 100
precision mediump float;                          
varying vec2  v_texCoord;                          
uniform sampler2D  s_texture;     
uniform vec2 win;
     
void main()                            
{ 
	float delta_x = 1.0/win.x;
	float delta_y = 1.0/win.y;
	vec4 color = vec4(0.0);
	float x = 0.0;
	float y = 0.0;
	x = max(0.0, v_texCoord.x + float(-1) * delta_x);
	y = max(0.0, v_texCoord.y );
	color += texture2D(s_texture, vec2(x, y));
	x = max(0.0, v_texCoord.x + float(1) * delta_x);
	y = max(0.0, v_texCoord.y );
	color += texture2D(s_texture, vec2(x, y));
	x = max(0.0, v_texCoord.x);
	y = max(0.0, v_texCoord.y + float(-1) * delta_y);
	color += texture2D(s_texture, vec2(x, y));		
	x = max(0.0, v_texCoord.x); 
	y = max(0.0, v_texCoord.y + float(1) * delta_y);
	color += texture2D(s_texture, vec2(x, y));		
	color += texture2D(s_texture, v_texCoord);		
	color = color/5.0;
	gl_FragColor = color; 								
}
