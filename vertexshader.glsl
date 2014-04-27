#version 130

uniform mat4 Matrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

in vec3 in_position;
in vec4 in_color;
in vec3 normal;

out vec4 pass_color;
out vec3 out_normal;

void main()
{
	gl_Position=Matrix * viewMatrix * modelMatrix * vec4(in_position, 1.0);
	out_normal = normal;
	pass_color = in_color;
}
