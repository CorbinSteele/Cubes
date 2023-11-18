/*
  ==============================================================================

    CubeCalendarData.h
    Created: 21 Mar 2021 12:41:29pm
    Author:  corbin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CubeData.h"

class CubeCalendarData final
{
public:
    CubeCalendarData();
    CubeCalendarData(const juce::MemoryBlock& data);
    ~CubeCalendarData();
    
    const void commit(bool hardCommit = false);
    const juce::MemoryBlock getData();
    void setData(const juce::MemoryBlock& data);
    void registerCommitCallback(std::function<void(const juce::MemoryBlock&, bool)> callback);
    
    //const std::array<int, 3> getDate();
    
    std::array<CubeData, 5> cubes {CubeData::Type::Low, CubeData::Type::High, CubeData::Type::Prototertile, CubeData::Type::Deuterotertile, CubeData::Type::Tritertile};
    
private:
    juce::MemoryOutputStream outStream;
    template <typename Archiver>
    void serialize(Archiver& archiver) {
        for (CubeData& cube : cubes) {
            archiver(cube.type);
            archiver(cube.state);
            archiver(cube.quat);
        }
    }
    std::function<void(const juce::MemoryBlock&, bool)> onCommit; //ADD CALLBACK TO CONNECTION_HANDLER
};