#version 130

uniform vec4 Ambient;

uniform vec3 LightColor1;
uniform vec3 LightDirection1;
uniform vec3 HalfVector1;

in vec4 pass_color;
in vec3 out_normal;

out vec4 out_color;

void main()
{
	vec3 amb = vec3(Ambient);
	
	float diffuse1 = max(0.0, dot(normalize(out_normal), normalize(LightDirection1)));
	float specular1 = max(0.0, dot(normalize(out_normal), normalize(HalfVector1)));
	
	if (diffuse1 == 0.0)
	{
		specular1 = 0.0;
	}
	else
	{
		specular1 = pow(specular1, 15);
	}
	
	vec3 scatteredLight = Ambient.rgb + LightColor1 * diffuse1;
	vec3 reflectedLight = LightColor1 * specular1 * 5;
	
	vec3 final = min( (pass_color.rgb * amb) * scatteredLight + reflectedLight, vec3(1.0));
	
	out_color = vec4(final, pass_color.a);
}
