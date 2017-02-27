// phongEC.frag

// These are set by the .vert code, and they're interpolated.
varying vec3 ec_vnormal, ec_vposition;

void main() {
	// ?, normal?, light pos?, vertex pos?, halfway?
	vec3 P, N, L, V, H;

	//get material params
	vec4 diffuse_color = gl_FrontMaterial.diffuse; 
	vec4 specular_color = gl_FrontMaterial.specular; 
	float shininess = gl_FrontMaterial.shininess;

	float pi = 3.14159265;
	int light_count = 1;

	vec4 color = vec4(0.0,0.0,0.0,0.0);

	P = ec_vposition;
	
	//do per light calculations and add to color
	int i;
	for (i = 0; i < light_count; i++) {

		N = normalize(ec_vnormal);
		L = normalize(gl_LightSource[i].position.xyz - P);
		V = normalize(-P);
		H = normalize(L+V);

		//calculate colors
		diffuse_color *= max(dot(N,L),0.0);
		clamp(diffuse_color, 0.0, 1.0);

		specular_color *= ((shininess+2.0)/(8.0*pi))*pow(max(dot(H,N),0.0),shininess);
		clamp(specular_color, 0.0, 1.0);

		color += (diffuse_color + specular_color);
	}
			
	gl_FragColor = (color);
}