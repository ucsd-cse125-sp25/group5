#version 430 core

// Inputs to the fragment shader are the outputs of the same name from the vertex shader.
// Note that you do not have access to the vertex shader's default output, gl_Position.

in vec3 fragNormal;

// uniforms used for lighting
uniform vec3 AmbientColor = vec3(0.2);
// uniform vec3 LightDirection = normalize(vec3(1, 5, 2));
// uniform vec3 LightColor = vec3(1);
// uniform vec3 LightDirectionTwo = normalize(vec3(1, 1, 4));
// uniform vec3 LightColorTwo = vec3(1.0,0.1,0.1);
uniform vec3 DiffuseColor;	// passed in from c++ side NOTE: you can also set the value here and then remove 
							// color from the c++ side

layout(std430, binding = 0) buffer LightBuffer {
	struct Light {
		vec4 position;
		vec4 color;
		float intensity;
		float radius;
		int type;
		float padding[5];
	};
	Light lights[];
}

// You can output many things. The first vec4 type output determines the color of the fragment
out vec4 fragColor;

void main()
{
	// Compute irradiance (sum of ambient & direct lighting)
	//vec3 irradiance = AmbientColor + LightColor * max(0, dot(LightDirection, fragNormal)) + LightColorTwo * max(0, dot(LightDirectionTwo, fragNormal));

	vec3 irradiance = AmbientColor;
	for (int i = 0; i < lights.length(); i++) {
		Light light = lights[i];
		vec3 lightDir = normalize(light.position.xyz - fragNormal);

		if (light.type == 0) { //Point Light
			float distance = length(light.position.xyz - fragNormal);
			float attenuation = 1.0 / (1.0 + (distance / light.radius));
			irradiance += light.color.rgb * light.intensity * max(0.0, dot(fragNormal, lightDir)) * attenuation;
		} else if (light.type == 1) {
			irradiance += light.color.rgb * light.intensity * max(0.0, dot(fragNormal, lightDir));
		}
	}

	// Diffuse reflectance
	//vec3 reflectance = irradiance * DiffuseColor;
	// Gamma correction
	//fragColor = vec4(sqrt(reflectance), 1);
	fragColor = vec4(sqrt(irradiance), 1.0);
}