#version 460

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 worldPosition;
    vec3 normal;
    vec2 texCoord;
} gs_in[];

out GS_OUT {
    vec3 worldPosition;
    vec3 normal;
    vec2 texCoord;
} gs_out;

uniform vec3 voxelGridCenter;  // g_xWorld_VoxelRadianceDataCenter
uniform float voxelGridSize;  // g_xWorld_VoxelRadianceDataSize
uniform int voxelResolution;  // g_xWorld_VoxelRadianceDataRes

void main() {
    //// Compute face normal and find dominant axis
    //vec3 faceNormal = normalize(gs_in[0].normal + gs_in[1].normal + gs_in[2].normal);
    //faceNormal = abs(faceNormal); // Use absolute values
    //int dominantAxis = faceNormal.x > faceNormal.y ? (faceNormal.x > faceNormal.z ? 0 : 2) : (faceNormal.y > faceNormal.z ? 1 : 2);
//
    //for (int i = 0; i < 3; ++i) {
    //    vec4 pos = vec4((gs_in[i].worldPosition - voxelGridCenter) / voxelGridSize, 1.0);
//
    //    // Project onto dominant axis
    //    if (dominantAxis == 0) {
    //        pos.xyz = pos.zyx; // Swap x and z
    //    } else if (dominantAxis == 1) {
    //        pos.xyz = pos.xzy; // Swap y and z
    //    }
//
    //    // Scale to clip space
    //    pos.xy /= float(voxelResolution);
    //    pos.z = 1.0;
//
    //    // Conservative rasterization
    //    vec2 side0 = normalize(gs_in[(i + 1) % 3].worldPosition.xy - gs_in[i].worldPosition.xy);
    //    vec2 side1 = normalize(gs_in[(i + 2) % 3].worldPosition.xy - gs_in[(i + 1) % 3].worldPosition.xy);
    //    vec2 side2 = normalize(gs_in[i].worldPosition.xy - gs_in[(i + 2) % 3].worldPosition.xy);
    //    float texelSize = 1.0 / float(voxelResolution);
//
    //    pos.xy += normalize(-side0 + side2) * texelSize;
//
    //    // Pass data to fragment shader
    //    gs_out.worldPosition = gs_in[i].worldPosition;
    //    gs_out.normal = gs_in[i].normal;
    //    gs_out.texCoord = gs_in[i].texCoord;
//
    //    gl_Position = pos;
    //    EmitVertex();
    //}
    //EndPrimitive();


    for (int i = 0; i < 3; ++i) {
        gs_out.worldPosition = gs_in[i].worldPosition;
        gs_out.normal = gs_in[i].normal;
        gs_out.texCoord = gs_in[i].texCoord;

        gl_Position = vec4(gs_in[i].worldPosition, 1.0); // Pass through
        EmitVertex();
    }
    EndPrimitive();
}