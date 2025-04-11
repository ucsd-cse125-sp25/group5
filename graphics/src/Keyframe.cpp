#include "Keyframe.h"
#include <iostream>
#include "glm/gtx/euler_angles.hpp"


void Keyframe::computeCoefficients(float nextValue,  float nextTime, float nextTanIn) {
    //// Compute the cubic coefficients A, B, C, D based on the tangents and values
    float dt = nextTime - this->time;
    float dv = nextValue - this->value;

    glm::mat4 Hmat = glm::mat4(
        2, -3, 0, 1,
        -2, 3, 0, 0,
        1, -2, 1, 0,
        1, -1, 0, 0
    );
    glm::vec4 vector = glm::vec4(this->value, nextValue, dt * tangentOut, dt * nextTanIn);
    vector = Hmat * vector;
    a = vector[0];
    b = vector[1];
    c = vector[2];
    d = vector[3];
    //a = 2 * dv - (tangentOut + tangentIn) * dt;
    //b = -3 * dv + (2 * tangentOut + tangentIn) * dt;
    //c = tangentOut * dt;
    //d = this->value;
    //float dt = nextTime - time;
    //float dv = nextValue - value;

    //a = 2 * dv - tangentIn - tangentOut;
    //b = -3 * dv + 2 * tangentIn + tangentOut;
    //c = tangentIn;
    //d = value;

    // Debug print statements
    std::cout << "Keyframe: time=" << time << ", value=" << value << ", tangentIn=" << tangentIn << ", tangentOut=" << tangentOut << std::endl;
    std::cout << "Coefficients: a=" << a << ", b=" << b << ", c=" << c << ", d=" << d << std::endl;
}
