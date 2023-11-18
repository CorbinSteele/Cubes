#pragma once

#include <JuceHeader.h>
#include "CubeCalendarData.h"
#include "DraggableCubeDisplay.h"

//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
class MainComponent  : public juce::Component
{
public:
    //==============================================================================
    MainComponent(CubeCalendarData& data);
    ~MainComponent() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;

private:
    //==============================================================================
    // Your private member variables go here...
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
    
    CubeCalendarData& cubeCalendarData;
    DraggableCubeDisplay draggableCubeDisplay;
};
