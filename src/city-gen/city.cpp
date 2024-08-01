#include "realtime.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>

void Realtime::GenerateCity() {
    m_building_arr.clear(); // clean the old scene first:

    // make the ground plane
    glm::mat4 groundCTM = glm::mat4(1.0f);
    groundCTM = glm::scale(groundCTM, glm::vec3(40.0f, 0.001f, 40.0f));
    m_CTM_collection.push_back(groundCTM);
    // add some streets in the city:
    Building street1(glm::vec3(0.0f, 0.0f, 2.0f),glm::vec3(40.0f, 0.02f, 0.5f));
    Building street2(glm::vec3(0.0f, 0.0f, -5.0f),glm::vec3(40.0f, 0.02f, 0.7f));
    Building street3(glm::vec3(-5.0f, 0.0f, 0.0f),glm::vec3(0.7f, 0.02f, 40.0f));
    Building street4(glm::vec3(3.0f, 0.0f, 0.0f),glm::vec3(0.7f, 0.02f, 40.0f));
    Building street5(glm::vec3(7.0f, 0.0f, 0.0f),glm::vec3(0.5f, 0.02f, 40.0f));
    Building street6(glm::vec3(0.0f, 0.0f, 8.0f),glm::vec3(40.0f, 0.02f, 0.5f));
    Building street7(glm::vec3(0.0f, 0.0f, -12.0f),glm::vec3(40.0f, 0.02f, 0.7f));
    Building street8(glm::vec3(0.0f, 0.0f, -17.0f),glm::vec3(40.0f, 0.02f, 0.7f));
    Building street9(glm::vec3(-12.0f, 0.0f, 0.0f),glm::vec3(0.7f, 0.02f, 40.0f));
    Building street0(glm::vec3(15.0f, 0.0f, 0.0f),glm::vec3(0.5f, 0.02f, 40.0f));
    m_building_arr.push_back(street1);
    m_building_arr.push_back(street2);
    m_building_arr.push_back(street3);
    m_building_arr.push_back(street4);
    m_building_arr.push_back(street5);
    m_building_arr.push_back(street6);
    m_building_arr.push_back(street7);
    m_building_arr.push_back(street8);
    m_building_arr.push_back(street9);
    m_building_arr.push_back(street0);

    numOfStreet = 10;

    // add some random cubes:
    int overlapCounter = 0;
    // if the random building generator tries X times and still overalps with some buildings,
    // then the city might be too crowded, we should stop adding new Buildings
    int overlapThreshold = 100;
    for(int i = 0; i < 80;){
        float random_scale_x = getRandomFloat(0.8f, 1.5f);
        float random_scale_y = getRandomFloat(1.0f, 3.5f);
        float random_scale_z = getRandomFloat(0.8f, 1.5f);
        float random_x = getRandomFloat(-18.0f,18.0f);
        float random_z = getRandomFloat(-18.0f,18.0f);
        float y = 0.5f*random_scale_y;
        glm::vec3 scale(random_scale_x,random_scale_y,random_scale_z);
        glm::vec3 pos(random_x, y, random_z);
        Building newBuilding(pos, scale);
        bool overlapFound = false;
        for(Building b : m_building_arr){
            if(newBuilding.checkOverlap(b)){
                overlapFound = true;
                overlapCounter++; // increment the overlap counter
                break;
            }
        }

        if(overlapFound && overlapCounter >= overlapThreshold){
            std::cout<<"The city is too crowded! Stop adding new buildings!"<<std::endl;
            break; // Break out of the loop if the overlap threshold is reached
        }

        if(!overlapFound){
            m_building_arr.push_back(newBuilding);
            i++;
            overlapCounter = 0; // Reset overlap counter after successful addition
        }
    }
}
