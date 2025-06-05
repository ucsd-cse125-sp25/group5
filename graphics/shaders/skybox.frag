#version 330 core

in vec3 TexCoords;
in vec3 FragPos;
out vec4 FragColor;

uniform samplerCube skybox;

uniform vec3 viewPos;
uniform float time;
uniform float waterLevel;
uniform float fogConstant;
uniform float fogConstantW;
uniform vec3 fogColor;
uniform vec3 fogColorW;
uniform mat4 waterModel;

// Simplex 2D noise
//
vec3 permute(vec3 x) { return mod(((x*34.0)+1.0)*x, 289.0); }

float snoise(vec2 v){
  const vec4 C = vec4(0.211324865405187, 0.366025403784439,
           -0.577350269189626, 0.024390243902439);
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);
  vec2 i1;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;
  i = mod(i, 289.0);
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
  + i.x + vec3(0.0, i1.x, 1.0 ));
  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy),
    dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;
  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
}

float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

float f(float inpt){
    return exp(sin(inpt) - 1) - 0.5;
}

float FBMVertex(vec3 pos){ //return y pos of water at this x and z coord
    float y = 0;
    float dx = 0;
    float dz = 0;

    float amp = 0.2;
    float freq = 0.4;

    float pc = 1.6;

    for(int i = 0; i < 15; i++){
        float angle = 2.0 * 3.1415 * random(vec2(i, 25.0));
        vec2 dir = vec2(cos(angle), sin(angle));
        float v = dot(dir, vec2(pos.x, pos.z));
        float phase = random(vec2(i, 99.99)) * 6.2831;
        float inpt = v * freq + time * freq * pc + phase;
        float height = amp * f(inpt);
        y += height;

        freq *= 1.07;
        amp *= 0.95;
    }

    return y;

}

void main()
{
	vec3 dir = normalize(FragPos);
    vec3 fc = fogColor;

    //float fogWeight = (1.0/exp(fogConstant*length(vec3(dir.x * dir.x * dir.x * 50.0, sqrt(max(0, dir.y)) * 2.5, dir.z * dir.z * dir.z * 50.0))));
    float fogWeight = (1.0/exp(fogConstant*length(vec3(pow(sqrt(dir.x * dir.x + dir.z * dir.z)/sqrt(2), 3.0) * 35.0, sqrt(max(0, dir.y)) * 2.5, pow(sqrt(dir.x * dir.x + dir.z * dir.z)/sqrt(2), 3.0) * 35.0))));

	vec4 result = texture(skybox, TexCoords);
    if(dir.y < 0){
        result.xyz = fogColor;
        fogWeight = 1.0/exp(fogConstant*length(vec3(pow(sqrt(dir.x * dir.x + dir.z * dir.z)/sqrt(2), 10.0) * 10.0, sqrt(max(0, -dir.y)) * 2.5, pow(sqrt(dir.x * dir.x + dir.z * dir.z)/sqrt(2), 10.0) * 10.0)));
        fc.xyz = vec3(0.2, 0.22, 0.34);
    }

	float height = waterLevel + FBMVertex(viewPos);

    if(viewPos.y < height){
        result.xyz = fogColorW + vec3(0, 0.02, 0.05);
    }
    else{
        result.xyz = mix(fc, result.xyz, clamp(fogWeight - 0.02, 0, 1));
    }

	FragColor = result;
}