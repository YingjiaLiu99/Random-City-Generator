#include "Skyscraper.h"

void Skyscraper::updateParams(int param1) {
    m_vertexData = std::vector<float>();
    m_param1 = 3 * param1;
    setVertexData();
}

void Skyscraper::makeTile(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight) {
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(glm::cross(bottomLeft - topLeft, topRight - topLeft)));

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft)));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(glm::cross(topRight - bottomRight, bottomLeft - bottomRight)));
    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, glm::normalize(glm::cross(topRight - bottomRight, bottomLeft - bottomRight)));
    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft)));
    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, glm::normalize(glm::cross(bottomLeft - topLeft, topRight - topLeft)));

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, -glm::normalize(glm::cross(topRight - bottomRight, bottomLeft - bottomRight)));

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, -glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft)));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, -glm::normalize(glm::cross(bottomLeft - topLeft, topRight - topLeft)));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, -glm::normalize(glm::cross(bottomLeft - topLeft, topRight - topLeft)));

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, -glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft)));

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, -glm::normalize(glm::cross(topRight - bottomRight, bottomLeft - bottomRight)));
}

void Skyscraper::makeReverseTileOnly(glm::vec3 topLeft,
                               glm::vec3 topRight,
                               glm::vec3 bottomLeft,
                               glm::vec3 bottomRight) {

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, -glm::normalize(glm::cross(topRight - bottomRight, bottomLeft - bottomRight)));

    insertVec3(m_vertexData, bottomLeft);
    insertVec3(m_vertexData, -glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft)));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, -glm::normalize(glm::cross(bottomLeft - topLeft, topRight - topLeft)));

    insertVec3(m_vertexData, topLeft);
    insertVec3(m_vertexData, -glm::normalize(glm::cross(bottomLeft - topLeft, topRight - topLeft)));

    insertVec3(m_vertexData, topRight);
    insertVec3(m_vertexData, -glm::normalize(glm::cross(bottomRight - bottomLeft, topLeft - bottomLeft)));

    insertVec3(m_vertexData, bottomRight);
    insertVec3(m_vertexData, -glm::normalize(glm::cross(topRight - bottomRight, bottomLeft - bottomRight)));
}

void Skyscraper::makeFace(glm::vec3 topLeft,
                    glm::vec3 topRight,
                    glm::vec3 bottomLeft,
                    glm::vec3 bottomRight, bool makeWindows) {
    glm::vec3 x = 1.f / m_param1 * (topRight - topLeft);
    glm::vec3 y = 1.f / m_param1 * (bottomLeft - topLeft);
    for (int i = 0; i < m_param1; i++) {
        for (int j = 0; j < m_param1; j++) {
            float c = (float) i;
            float r = (float) j;
            glm::vec3 t_l = topLeft + r * x + c * y;
            glm::vec3 t_r = topLeft + (r + 1.f) * x + c * y;
            glm::vec3 b_l = topLeft + r * x + (c + 1.f) * y;
            glm::vec3 b_r = topLeft + (r + 1.f) * x + (c + 1.f) * y;

            if ((!(i % 3 == 1 && j % 3 == 1)) || (!makeWindows)) {
                makeTile(t_l, t_r, b_l, b_r);
            } else {
                makeReverseTileOnly(t_l, t_r, b_l, b_r);
            }
        }
    }
}

void Skyscraper::setVertexData() {

    makeFace(glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3(0.5f, -0.5f, 0.5f), true);

    makeFace(glm::vec3(0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f, -0.5f),
             glm::vec3(-0.5f,-0.5f,-0.5f), true);

    makeFace(glm::vec3(-0.5f, -0.5f, 0.5f),
             glm::vec3(0.5f,  -0.5f, 0.5f),
             glm::vec3(-0.5f,-0.5f, -0.5f),
             glm::vec3(0.5f,-0.5f,-0.5f), false);

    makeFace(glm::vec3( 0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3(0.5f,  0.5f, -0.5f),
             glm::vec3(-0.5f, 0.5f,-0.5f), false);

    makeFace(glm::vec3(-0.5f, 0.5f, -0.5f),
             glm::vec3(-0.5f,  0.5f, 0.5f),
             glm::vec3(-0.5f, -0.5f,-0.5f),
             glm::vec3(-0.5f, -0.5f,0.5f), true);

    makeFace(glm::vec3(0.5f,  0.5f,  0.5f),
             glm::vec3(0.5f,  0.5f, -0.5f),
             glm::vec3(0.5f, -0.5f,  0.5f),
             glm::vec3(0.5f, -0.5f,-0.5f), true);
}

void Skyscraper::insertVec3(std::vector<float> &data, glm::vec3 v) {
    data.push_back(v.x);
    data.push_back(v.y);
    data.push_back(v.z);
}

