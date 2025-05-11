#ifndef KEYFRAME_H
#define KEYFRAME_H
#include <glm/gtc/quaternion.hpp>

class Keyframe {
public:
    float time;
    float value;
    bool import;
    glm::quat quatValue;
    float tangentIn;
    float tangentOut;
    char ruleIn;
    char ruleOut;
    float a;
    float b;
    float c;
    float d;

    Keyframe(float time, float value, float tangentIn = 0, float tangentOut = 0, char ruleIn = 0, char ruleOut = 0)
        : time(time), value(value), tangentIn(tangentIn), tangentOut(tangentOut), ruleIn(ruleIn), ruleOut(ruleOut), a(0), b(0), c(0), d(0) {
    }

    void computeCoefficients(float nextValue, float nextTime, float nextTanIn);
};

#endif // KEYFRAME_H