#include "geometryUtils.h"

glm::vec3 GeometryUtils::calculateNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2) {
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    return glm::normalize(glm::cross(edge1, edge2));
}

glm::vec3 GeometryUtils::calculateTangent(
    const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2,
    const glm::vec2& uv0, const glm::vec2& uv1, const glm::vec2& uv2
) {
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;
    glm::vec2 deltaUV1 = uv1 - uv0;
    glm::vec2 deltaUV2 = uv2 - uv0;

    float f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);
    glm::vec3 tangent = f * (deltaUV2.y * edge1 - deltaUV1.y * edge2);
    return glm::normalize(tangent);
}