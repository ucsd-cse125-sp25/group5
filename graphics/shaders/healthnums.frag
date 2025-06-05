#version 430 core
in vec2 TexCoord;
in vec3 FragColor;
in vec2 posOut;

out vec4 fragColor;

uniform sampler2D texture1;
uniform float time;

void main() {
    vec4 texColor = texture(texture1, TexCoord);
    vec4 shadow = texture(texture1, TexCoord + vec2(-0.07, 0.06));
    //transparent background
    if (texColor.a == 0.0 && shadow.a == 0.0) {
        // Smooth alpha blending instead of hard cutoff
        float texEdge = smoothstep(0.0, 0.1, texColor.a);
        float shadowEdge = smoothstep(0.0, 0.1, shadow.a);
        float alphaMix = max(texEdge, shadowEdge); // For use if blending with foreground

        // Radial glow around center
        vec2 center = vec2(0.5, 0.5);
        float dist = distance(TexCoord, center);
        float glow = smoothstep(0.5, 0.0, dist * 0.8); // radial gradient, fades outward
        glow *= 0.95 + 0.05 * sin(time * 2.0);   // subtle shimmer animation

        vec3 bgColor = mix(
            vec3(0.0, 0.5, 1.0),    // outer glow color (dark teal)
            vec3(1.0, 0.5, 0.5),    // center glow color (cyan-blue)
            glow
        );

        fragColor = vec4(bgColor, 0.4 * glow);  // smooth alpha, brighter center
        return;
    }

    //is a shadow
    else if(texColor.a == 0.0){
        fragColor = vec4(0.0,0.0,0.0, 0.9);
    }
    else{
        fragColor = vec4(1.0-TexCoord.x * TexCoord.y, 0.5, TexCoord.y/2.0 + 0.5, 10.0)*0.7 + texColor * 1.5;
    }
}