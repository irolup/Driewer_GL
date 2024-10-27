#version 330 core

in vec2 TexCoord;
out vec4 FragColor;

uniform sampler2DMS screenTexture; // MSAA texture
uniform vec2 inverseScreenSize;

void main() {
    vec3 color = vec3(0.0);
    int samples = 4; // Number of samples in the MSAA texture

    for (int i = 0; i < samples; ++i) {
        color += texelFetch(screenTexture, ivec2(gl_FragCoord.xy), i).rgb;
    }

    FragColor = vec4(color / float(samples), 1.0);
}