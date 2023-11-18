/*
  ==============================================================================

    DraggableCubeDisplay.h
    Created: 23 Mar 2021 4:51:00pm
    Author:  corbin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

#include "OpenGLWrapper.h"
#include "Cube.h"

//==============================================================================
/*
*/
class DraggableCubeDisplay  : public OpenGLWrapper
{
public:
    DraggableCubeDisplay(CubeCalendarData&);
    ~DraggableCubeDisplay() override;
    
    void mouseDown (const juce::MouseEvent& e) override;
    void mouseDrag (const juce::MouseEvent& e) override;
    void mouseUp (const juce::MouseEvent& e) override;
    void mouseDoubleClick (const juce::MouseEvent& e) override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (DraggableCubeDisplay)
    
    CubeCalendarData& calendarData;
    juce::Draggable3DOrientation draggableQuat;
    Cube* activeCube;
    
    void commitActiveCube(bool hardCommit = true);
    bool settingActive {false};
    
    struct CubesComparator
    {
        static const int compareElements (Cube* first, Cube* second) {
            const int f = first->getStatefulPosition();
            const int s = second->getStatefulPosition();
            return f < 0 ? (s > -1) : (f < s ? -1 : 1);
        }
    };
};
