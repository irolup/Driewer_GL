#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
uniform sampler2D screenTexture;
uniform vec2 inverseScreenSize;

void main() {
    vec3 color = texture(screenTexture, TexCoords).rgb;

    // FXAA 2.0 Algorithm
    // Calculate texture coordinates offsets
    vec2 texOffset = inverseScreenSize; // 1/screenSize
    vec3 luma = vec3(0.299, 0.587, 0.114);  // Luma weights

    // Sample surrounding pixels
    float lumaTL = dot(texture(screenTexture, TexCoords + vec2(-texOffset.x, texOffset.y)).rgb, luma);
    float lumaTR = dot(texture(screenTexture, TexCoords + vec2(texOffset.x, texOffset.y)).rgb, luma);
    float lumaBL = dot(texture(screenTexture, TexCoords + vec2(-texOffset.x, -texOffset.y)).rgb, luma);
    float lumaBR = dot(texture(screenTexture, TexCoords + vec2(texOffset.x, -texOffset.y)).rgb, luma);
    float lumaM  = dot(color, luma);

    // Edge detection
    float maxLuma = max(max(lumaTL, lumaTR), max(lumaBL, lumaBR));
    float minLuma = min(min(lumaTL, lumaTR), min(lumaBL, lumaBR));

    // Calculate edge intensity
    float edgeDetect = maxLuma - minLuma;

    // FXAA blend weights
    vec2 dir = vec2(0.0);
    if (edgeDetect > 0.1) {
        // If we detect an edge, calculate the blend direction
        dir = normalize(vec2(lumaTR - lumaTL, lumaBL - lumaTL));
    }

    // Calculate the final color
    vec3 finalColor = color;
    if (edgeDetect > 0.1) {
        finalColor = mix(color, 
                         (texture(screenTexture, TexCoords + dir * texOffset).rgb + 
                          texture(screenTexture, TexCoords - dir * texOffset).rgb) * 0.5, 
                         0.5); // Blend the color based on detected edge
    }

    FragColor = vec4(finalColor, 1.0);
}