#version 450
layout(triangles) in; 
layout(line_strip,max_vertices = 9) out; 
 
void main() 
{ 
    vec4 p1 = gl_in[0].gl_Position;
	vec4 p2 = gl_in[1].gl_Position;
	vec4 p3 = gl_in[2].gl_Position;
	
	vec4 center = (p1+p2+p3)*0.33333;
	
	//1
	gl_Position = p1;
	EmitVertex();
	
	gl_Position = p2;
	EmitVertex();
	
	gl_Position = center;
	EmitVertex();	

    //2
	gl_Position = p2;
	EmitVertex();
	
	gl_Position = p3;
	EmitVertex();

	gl_Position = center;
	EmitVertex();	
	
	//3
	gl_Position = center;
	EmitVertex();
	
	gl_Position = p3;
	EmitVertex();

	gl_Position = p1;
	EmitVertex();
	
	EndPrimitive();
}
