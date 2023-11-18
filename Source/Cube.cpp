/*
  ==============================================================================

    Cube.cpp
    Created: 29 Mar 2021 9:07:53am
    Author:  corbin

  ==============================================================================
*/

#include "Cube.h"

const juce::Vector3D<float> Cube::InitPosition(int type, int state)
{
    if (state == 0) {
        //const float cubesFromBottom = (30.0f*SCALE - 3.0f*SCALE*SCALE - 80.0f) / 8.0f;
        if (type < 2) {
            return juce::Vector3D<float> {(type == 0) ? -SCALE : SCALE, 2.0f*SCALE - 8.0f, -8.0f - SCALE};
        } // (?) = (-10 + SCALE*15/4 - (3/8)*SCALE^2) = (-10 - 1.5 + 7.5)
        // ((- ?) = -10 + 52/32*SCALE - SCALE^2/32 = (SCALE^2 - 52*SCALE + 320) / 32
        // (1 + (? + [0.5*]SCALE)/(-8 - SCALE + [0.5*]SCALE)) = (2 - [[3*]]SCALE/8)
        // (?) = 2*SCALE - 8
        // (?) = 0.5*SCALE - 8
        // (?) = (2 - [3*]SCALE/8)(-10 + SCALE) = -10 +[3*]10*SCALE/8 - [3*]SCALE^2/8
        //WRONG (?) = (10*SCALE + SCALE^2 - 80)/8
        //1=13/8[650]; 2=10/8[500]; 4=4/8[200]
        //1=15/8[750]; 2=14/8[700]; 4=12/8[600]
        
        return juce::Vector3D<float> {0.0f, 0.5f*SCALE - 8.0f, -8.0f - SCALE*((float)type-1.5f)};
    }
    
    if (state == 1) {
        return juce::Vector3D<float> {0.0f, (type < 2) ? 1.5f : 1.0f, -4.0f - SCALE};
    }
    
    if (type < 2)
        return juce::Vector3D<float> {-5.0f, 9.0f - 6.0f*type, -13.0f - SCALE};
    else
        return juce::Vector3D<float> {5.0f, 10.0f - 4.0f*(type-2), -13.0f - SCALE*1.5f};
}


const bool Cube::checkBounds(const juce::Rectangle<int>& scale, const juce::Point<int>& mousePosition) {
    
    const juce::Vector3D<float> position = getPosition();
    const juce::Point<float> center = scale.getCentre().toFloat();
    const juce::Vector3D<float> radius = GetShape(cubemapID);
    
    // -2.0f : 2.0f, -4.0f, -10.0f
    // x: 1 - (-2 - 2) / (-10 + 2) = 0.5 * 400 = 200 CHECK
    // y: 1 + (-4 + 2) / (-10 - 2) = 7/6 * 400 = 466.66 CHECK
    // x2: 1 - (-2 + 2) / (-10 + 2) = 1 * 400 = 400 CHECK
    // y2: 1 + (-4 - 2) / (-10 + 2) = 7/4 * 400 = 700 CHECK
    // w: -2 * 2 / (-10 + 2) = 0.5 * 400 = 200 CHECK
    // h: -2 * 2 / (-10 + 2) = 0.5 * 400 = 200 CHECK
    
    /*
    const float x1 = (1.0f - (position.x-radius.x)/(position.z+radius.z)) * center.x;
    const float y1 = (1.0f + (position.y+radius.y)/(position.z+radius.z)) * center.y;
    const float x2 = (1.0f - (position.x+radius.x)/(position.z+radius.z)) * center.x;
    const float y2 = (1.0f + (position.y-radius.y)/(position.z+radius.z)) * center.y;
    */
    
    const juce::Rectangle<float> frontFace (
        center.x * (1.0f - (position.x-radius.x)/(position.z+radius.z)),
        center.y * (1.0f + (position.y+radius.y)/(position.z+radius.z)),
        center.x * -2.0f * radius.x / (position.z+radius.z),
        center.y * -2.0f * radius.y / (position.z+radius.z)
    );
    const juce::Rectangle<float> backFace (
        center.x * (1.0f - (position.x-radius.x)/(position.z-radius.z)),
        center.y * (1.0f + (position.y+radius.y)/(position.z-radius.z)),
        center.x * -2.0f * radius.x / (position.z-radius.z),
        center.y * -2.0f * radius.y / (position.z-radius.z)
    );
    return frontFace.getUnion(backFace).getSmallestIntegerContainer().contains(mousePosition);

}

Cube::Cube(int cubemapID, int shapeID, CubeData& cubeData) : cubemapID(cubemapID), shapeID(shapeID)//,
            //position(InitPosition(  cubeData.state < 0 ? cubemapID, cubeData.state,
            //                        cubeData.state < 0 ? -cubeData.state : 0))
{
    data = &cubeData;
    //rotation_ptr = &(cubeData.quat);
}

Cube::~Cube()
{
    data = nullptr;
    //rotation_ptr = nullptr;
}

const int Cube::getStatefulPosition() {
    return data->state;
}

const juce::Vector3D<float> Cube::getPosition() {
    return InitPosition(data->state < 0 ? cubemapID : data->state, data->state < 0 ? -data->state : 0);
    //return data->state;
}
void Cube::setPosition(int index, int state) {
    //position = InitPosition(index, state);
    data->state = (state == 0) ? index : -state;
}
const juce::Matrix3D<float> Cube::getViewMatrix()
{
    //position = InitPosition(data->state < 0 ? cubemapID : data->state, data->state < 0 ? -data->state : 0);
    return getRotation().getRotationMatrix() * getPosition();
}

const juce::Quaternion<float>& Cube::getRotation() {
    return data->quat;
}
void Cube::setRotation(const juce::Quaternion<float>& rot) {
    data->quat = rot;
}
/*
void Cube::commitRotation(const bool hardCommit) {
    data->quat = *rotation_ptr;
    if (hardCommit)
        setRotation(data->quat);
}
*/
void Cube::snapRotation() {
    juce::Matrix3D<float> matrix = getRotation().getRotationMatrix();
    RoundMatrix(matrix);
    setRotation(MatrixToQuat(matrix));
    
    /*
    auto v = data->quat.vector;
    auto s = data->quat.scalar;
    std::cout << "FACE: " << v.x << ", " << v.y << ", " << v.z << " - " << s << "\n";
    */

    /*
    std::cout << "FACE:\n";
    for (int k = 0; k < 12; k++) {
        if (k % 4 != 3)
            std::cout << matrix.mat[k] << "\t";
        else
        std::cout << "\n";
    }
    */
}