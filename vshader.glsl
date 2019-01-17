#version 150 

in   vec3 vPosition;
in   vec3 vNormal;
out  vec4 color;

// light and material properties
uniform vec4 AmbientProduct, DiffuseProduct, SpecularProduct;
uniform mat4 ModelView;
uniform mat4 Projection;
uniform vec4 LightPosition;
uniform float Shininess;


void main()
{
	// Transform vertex position into eye coordinates
	vec3 pos = (ModelView * vec4(vPosition, 1.0)).xyz;

	vec3 L = normalize(LightPosition.xyz - pos);
	vec3 E = normalize(-pos);
	vec3 H = normalize(L + E);  // halfway vector

	// Transform vertex normal into eye coordinates
	vec3 N = normalize(ModelView*vec4(vNormal,0.0)).xyz;

	vec4 ambient = AmbientProduct;

	float cos_theta = max(dot(L,N), 0.0);
	vec4 diffuse = cos_theta * DiffuseProduct;
	float cos_phi = pow(max(dot(N,H),0.0), Shininess);
	vec4 specular = cos_phi * SpecularProduct;
	if(dot(L,N)<0.0) specular = vec4(0.0, 0.0, 0.0, 1.0);
	gl_Position = Projection * ModelView * vec4(vPosition, 1.0);

	color = ambient + diffuse + specular;
	color.a = 1.0;
}
