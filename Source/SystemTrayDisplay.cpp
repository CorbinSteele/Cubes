/*
  ==============================================================================

    SystemTrayDisplay.cpp
    Created: 20 Mar 2021 1:02:37pm
    Author:  corbin

  ==============================================================================
*/

#include "SystemTrayDisplay.h"
#include "Cube.h"

const void SystemTrayDisplay::drawImage(juce::Graphics& g, const CubeData& cube)
{    
    juce::AffineTransform transform = juce::AffineTransform::translation(cube.state < 2 ? -200 : -400, -200);
    const juce::Image& img = Cube::GetImage(cube, transform);
    transform = transform.translated(cube.state < 2 ? 200 : 400, 200);
    
    g.drawImageTransformed(img, transform.translated(400*cube.state*(cube.state < 2), 400*(cube.state == 2)));
}

void SystemTrayDisplay::update()
{
    iconGraphics.fillAll();
    for (const CubeData& cube : data.cubes) {
        if (cube.state >= 0 && cube.state <= 2)
            drawImage(iconGraphics, cube);
    }
    #if JUCE_LINUX
    repaint();
    #else
    setIconImage (icon, {});
    #endif
}

//==============================================================================
SystemTrayDisplay::SystemTrayDisplay(CubeCalendarData& calendarData, juce::Component& window) :
    data(calendarData), icon(juce::Image::RGB, 800, 800, true), iconGraphics(icon), mainWindow(window)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    //juce::File file("~/Data/Media/Pragmatic/Picture/moon/MoonICO.jpg");
    //setIconImage(juce::ImageFileFormat::loadFrom(file), {});
    
    //iconGraphics.setColour(juce::Colours::crimson);
    setIconImage (icon, {});
    
    /*
    juce::Image cube1 = juce::ImageFileFormat::loadFrom(Resources::zero_png, (size_t) Resources::zero_pngSize);
    juce::Image cube2 = juce::ImageFileFormat::loadFrom(Resources::one_png, (size_t) Resources::zero_pngSize);
    juce::Image cube3 = juce::ImageFileFormat::loadFrom(Resources::January_png, (size_t) Resources::zero_pngSize);
        
        
    g.drawImageAt(cube1, 0, 0);
    g.drawImageAt(cube2, 400, 0);
    g.drawImage(cube3, 0, 400, 800, 400, 50, 0, 200, 100);
    */

        //setIconTooltip ("Juce Demo App!");
        //showInfoBubble("Title", "Content");
}

SystemTrayDisplay::~SystemTrayDisplay()
{
    menu.addItem(1, "Show");
    menu.addItem (2, "Quit");
}

void SystemTrayDisplay::mouseDown (const juce::MouseEvent& e)
{
    if (e.mods.isLeftButtonDown()) {
        mainWindow.setVisible(true);
    } else {
        // It's always better to open menus asynchronously when possible.
        menu.showMenuAsync (juce::PopupMenu::Options(),
                    juce::ModalCallbackFunction::forComponent (menuInvocationCallback, this));
    }
}
// This is invoked when the menu is clicked or dismissed
void SystemTrayDisplay::menuInvocationCallback (int chosenItemID, SystemTrayDisplay*)
{
    if (chosenItemID == 2)
        juce::JUCEApplication::getInstance()->systemRequestedQuit();
}

/*
void SystemTrayDisplay::paint (juce::Graphics& g)
{
    juce::SystemTrayIconComponent::paint(g);
}
*/