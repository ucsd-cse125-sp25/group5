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
        float glow = smoothstep(0.5, 0.0, pow(dist,2)); // radial gradient, fades outward
        glow *= 0.95 + 0.05 * sin(time * 2.0);   // subtle shimmer animation

        vec3 bgColor = mix(
            vec3(0.0, 0.1, 0.2),    // outer glow color (dark teal)
            vec3(0.3, 0.7, 0.9),    // center glow color (cyan-blue)
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
        vec3 baseColor = vec3(
            0.2 + 0.2 * TexCoord.y,       // Red — low, with a slight vertical gradient
            0.6 + 0.4 * TexCoord.y,       // Green — high, more variation
            0.6 + 0.4 * TexCoord.x        // Blue — high, varies with horizontal coord
        );
        float flicker = sin(TexCoord.x * 30.0 + TexCoord.y * 20.0 + time * 4.0) * 0.1;

        vec3 flickerColor = vec3(
            0.2,
            0.7 + flicker,
            0.8 + flicker
        );

        //fragColor = vec4(flickerColor, 1.0) * 0.7 + texColor * 1.5;

        fragColor = vec4(baseColor, 1.0) * 0.3 + vec4(flickerColor, 1.0) * 0.6 + texColor * 1.5;
    }
}