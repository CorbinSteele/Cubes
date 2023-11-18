/*
  ==============================================================================

    CubeData.h
    Created: 21 Mar 2021 8:37:16pm
    Author:  corbin

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

struct CubeData final
{
    /*
    static constexpr float angle_ = juce::MathConstants<float>::sqrt2 / 2;
    //Quaternion[] rotations = { Quaternion.Euler(-90,0,0), Quaternion.Euler(0,90,0), Quaternion.Euler(0,0,0), Quaternion.Euler(90,0,0), Quaternion.Euler(0,-90,0), Quaternion.Euler(0,180,0)};
    //CubemapFace[] faces = { CubemapFace.PositiveY, CubemapFace.PositiveX, CubemapFace.PositiveZ, CubemapFace.NegativeY, CubemapFace.NegativeX, CubemapFace.NegativeZ };
    enum struct Rotation: int { Up, Down, Left, Right, Clock, Counterclock };
    const std::map<Rotation, juce::Quaternion<float>> rotationToQuat {
        {Rotation::Up, juce::Quaternion<float>(angle_,0,0,angle_)},
        {Rotation::Down, juce::Quaternion<float>(-angle_,0,0,angle_)},
        {Rotation::Left, juce::Quaternion<float>(0,angle_,0,angle_)},
        {Rotation::Right, juce::Quaternion<float>(0,-angle_,0,angle_)},
        {Rotation::Clock, juce::Quaternion<float>(0,0,angle_,angle_)},
        {Rotation::Counterclock, juce::Quaternion<float>(0,0,-angle_,angle_)}
    };    
    */
    
    enum struct Type: int { Low, High, Prototertile, Deuterotertile, Tritertile, Null };
    
    CubeData(Type type);
    
    Type type;
    int state;
    juce::Quaternion<float> quat;
};