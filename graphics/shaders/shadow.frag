#version 430 core

#define BIAS 0.01

void main(void)
{
    gl_FragDepth = gl_FragCoord.z;
    gl_FragDepth += gl_FrontFacing ? BIAS : 0.0;
}