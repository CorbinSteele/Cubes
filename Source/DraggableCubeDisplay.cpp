/*
  ==============================================================================

    DraggableCubeDisplay.cpp
    Created: 23 Mar 2021 4:51:00pm
    Author:  corbin

  ==============================================================================
*/

#include "DraggableCubeDisplay.h"

//==============================================================================
/*
*/

DraggableCubeDisplay::DraggableCubeDisplay(CubeCalendarData& calendarData) : calendarData(calendarData)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    //setSize(400,400);
    draggableQuat.setViewport(juce::Rectangle<int>(800, 800));
    activeCube = nullptr;
    
    initCubes(calendarData);
    
    // background colour
    setColour(0, juce::Colours::black);
}
DraggableCubeDisplay::~DraggableCubeDisplay()
{
    activeCube = nullptr;
}

void DraggableCubeDisplay::mouseDown (const juce::MouseEvent& e)
{
    const auto pos = e.getPosition();
    /*
    std::cout << pos.x;
    std::cout << ", ";
    std::cout << pos.y;
    std::cout << "\n";
    */
    if (e.mods.isLeftButtonDown()) {
        CubesComparator c;
        cubes.sort(c);
        for (Cube* cube : cubes) {
            if ((activeCube == nullptr || cube->getStatefulPosition() > -1) && cube->checkBounds(getBounds(),pos)) {
                commitActiveCube(false);
                cube->setPosition(cube->cubemapID, 1);
                activeCube = cube;
                settingActive = true;
                break;
            }
        }
        if (activeCube != nullptr) {
            draggableQuat.getQuaternion() = activeCube->getRotation();
            draggableQuat.mouseDown(pos);
        }
    }
    else if (e.mods.isRightButtonDown()) {
        commitActiveCube(false);
    }
}
void DraggableCubeDisplay::mouseDrag (const juce::MouseEvent& e)
{
    if (activeCube != nullptr) {
        draggableQuat.mouseDrag(e.getPosition());
        activeCube->setRotation(draggableQuat.getQuaternion());
        calendarData.commit();
    }
}
void DraggableCubeDisplay::mouseUp (const juce::MouseEvent& e)
{
    if (settingActive) {
        settingActive = false;
        calendarData.commit(true);
    }
    else if (activeCube != nullptr && !e.mouseWasDraggedSinceMouseDown()) {
        activeCube->snapRotation();
        calendarData.commit();
    }
}
void DraggableCubeDisplay::mouseDoubleClick(const juce::MouseEvent& e)
{
    commitActiveCube();
}


void DraggableCubeDisplay::paint (juce::Graphics& g)
{
    //const juce::Image cubeImage = juce::ImageFileFormat::loadFrom(Resources::zero_png, (size_t) Resources::zero_pngSize);
    
    //const juce::Image cubeImage = CubeUtility::TypeToFaces(cube.type)[1];
    
    //g.drawImageTransformed(cubeImage, juce::AffineTransform::scale(0.5,0.5).translated(100,100));
}

void DraggableCubeDisplay::resized()
{
}

void DraggableCubeDisplay::commitActiveCube(bool hardCommit)
{
    if (activeCube != nullptr) {
        if (hardCommit) {
            for (int i = activeCube->shapeID * 4; i != (2 - activeCube->shapeID); activeCube->shapeID == 0 ? i++ : i--) {
                bool filled = false;
                for (Cube* cube : cubes) {
                    if (cube->getStatefulPosition() == i) {
                        filled = true;
                        break;
                    }
                }
                if (!filled) {
                    activeCube->setPosition(i, 0);
                    CubesComparator c;
                    cubes.sort(c);
                    break;
                }
            }
        }
        else {
            activeCube->setPosition(activeCube->cubemapID, 2);
        }
        calendarData.commit(hardCommit);
        activeCube = nullptr;
    }
}