#include "Building.h"
#include <glm/gtx/string_cast.hpp>

// constructor
Building::Building(glm::vec3 pos, glm::vec3 size){
    position = pos;
    size = size;
    glm::mat4 random_transform_matrix = glm::mat4(1.0f);
    glm::mat4 scaleM = glm::scale(random_transform_matrix, size);
    glm::mat4 translateM = glm::translate(random_transform_matrix, pos);
    CTM = translateM * scaleM;
    std::pair<float, float> bx(pos[0]-size[0]*0.5f,pos[0]+size[0]*0.5f);
    boundX = bx;
    std::pair<float, float> bz(pos[2]-size[2]*0.5f,pos[2]+size[2]*0.5f);
    boundZ = bz;

    type = static_cast <float> (rand()) / static_cast <float> (RAND_MAX); // assign a random type
}

bool Building::checkOverlap(Building& B) { // Make sure to pass by const reference
    // Check if there is a separation along the X axis
    if (boundX.second < B.boundX.first || B.boundX.second < boundX.first) return false;

    // Check if there is a separation along the Z axis
    if (boundZ.second < B.boundZ.first || B.boundZ.second < boundZ.first) return false;

    // No separation found, so there is an overlap
    return true;
}
