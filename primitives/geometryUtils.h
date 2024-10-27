#ifndef GEOMETRY_UTILS_H
#define GEOMETRY_UTILS_H

#include <vector>
#include <glm/glm.hpp>

class GeometryUtils {
public:
    // Function to calculate normals for each face given a list of vertices
    static glm::vec3 calculateNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2);

    // Function to calculate tangents for each face
    static glm::vec3 calculateTangent(
        const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
        const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec2& uv2
    );
};

#endif // GEOMETRY_UTILS_H