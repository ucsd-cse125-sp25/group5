#ifndef DOF_INCLUDED
#define DOF_INCLUDED
class DOF {
public:
    DOF();
    void SetValue(float val);
    double GetValue();
    void SetMinMax(float new_min, float new_max);
    float min;
    float max;
    float value;
};
#endif