#version 330 core

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

in vec3 fragNormal;
in vec3 FragPos;

// uniforms used for lighting
uniform vec3 viewPos;
uniform vec3 DiffuseColor;	// passed in from c++ side NOTE: you can also set the value here and then remove 
							// color from the c++ side

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{

	if(length(FragPos - viewPos) < 1.0){
		discard;
	}

	// Compute irradiance (sum of ambient & direct lighting)
	vec3 irradiance = DiffuseColor;

	// Gamma correction
	fragColor = vec4(sqrt(irradiance), 1);
}