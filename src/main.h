#pragma once

void createZBuffer(std::vector<float> &zbuffer);

void perspectiveDivide(glm::vec3 &v0, glm::vec3 &v1, glm::vec3 &v2);

void convertToRasterSpace(glm::vec3 &v0, Camera &camera);
