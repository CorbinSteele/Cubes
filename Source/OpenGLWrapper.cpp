/*
  ==============================================================================

    OpenGLWrapper.cpp
    Created: 27 Mar 2021 8:37:54am
    Author:  corbin

  ==============================================================================
*/

#include "OpenGLWrapper.h"

void OpenGLWrapper::initCubes(CubeCalendarData& calendarData) {
    addShape(Cube::GetShape(0));
    addShape(Cube::GetShape(2));
    for (int i = 0; i < 5; i++) {
        addCubemap(Cube::TypeToFaces(i));
        cubes.add(new Cube(i, (i < 2) ? 0 : 1, calendarData.cubes[i]));
    }
}

//==============================================================================
OpenGLWrapper::OpenGLWrapper()
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    setOpaque(true);
    context.setComponentPaintingEnabled(false);
    context.setRenderer(this);
    context.attachTo(*this);
    context.setContinuousRepainting(true);
}

OpenGLWrapper::~OpenGLWrapper()
{
    context.detach();
    cubes.clear();
    cubemaps.clear();
    cubeDimensions.clear();
}

void OpenGLWrapper::newOpenGLContextCreated()
{
    createShaders();
    for (size_t i = 0; i < cubemaps.size(); i++) {
        juce::OpenGLTexture* tex = cubemapTextures.add(std::make_unique<juce::OpenGLTexture>());
        tex->loadImage(*cubemaps[i]);
        // OpenGL method to specify how the image is horizontally tiled
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        // OpenGL method to specify how the image is vertically tiled
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    }
    
    for (size_t i = 0; i < cubeDimensions.size(); i++) {
        shapes.add(new Shape(context, cubeDimensions[i]));
    }
}
void OpenGLWrapper::renderOpenGL()
{
    const float desktopScale = (float) context.getRenderingScale();
    juce::OpenGLHelpers::clear(findColour(0));
    
    // OpenGL methods to avoid displaying pixels behind front pixels
    glEnable (GL_CULL_FACE);
    glEnable (GL_DEPTH_TEST);   // Enable the test
    glDepthFunc (GL_LESS);      // Do not display back pixels
    // Using a texture to paint main OpenGL object (teapot)
    context.extensions.glActiveTexture (GL_TEXTURE0); // Using texture #0
    glEnable (GL_TEXTURE_2D);   // It's a 2-D image texture
    
    glViewport (0, 0, juce::roundToInt(desktopScale * getWidth()), juce::roundToInt(desktopScale * getHeight()));
    shaderProgram->use();
    
    // Modify the uniform (global) variable projectionMatrix that will be used by the GPU when executing the shaders
    if (uniforms->projectionMatrix != nullptr)
        // Update the projection matrix with the values given, 1 matrix, do not transpose
        uniforms->projectionMatrix->setMatrix4(getProjectionMatrix().mat, 1, false);
        
    for (Cube* cube : cubes) {
        // Modify the uniform (global) variable viewMatrix that will be used by the GPU when executing the shaders
        if (uniforms->viewMatrix != nullptr)
            // Update the view matrix with the values given, 1 matrix, do not transpose
            uniforms->viewMatrix->setMatrix4(cube->getViewMatrix().mat, 1, false);
            
        cubemapTextures[cube->cubemapID]->bind();
        shapes[cube->shapeID]->draw(context, *attributes);
        cubemapTextures[cube->cubemapID]->unbind();
    }
    
    // Reset the element buffers so child Components draw correctly
    //context.extensions.glBindBuffer (GL_ARRAY_BUFFER, 0);
    //context.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, 0);
}
void OpenGLWrapper::openGLContextClosing()
{
    shaderProgram = nullptr;
    attributes = nullptr;
    uniforms = nullptr;
    for (auto& tex : cubemapTextures) {
        tex->release();
    }
    cubemapTextures.clear();
    shapes.clear();
}

void OpenGLWrapper::createShaders()
{
    vertexShader =
            "attribute vec4 position;\n"
            "attribute vec2 textureCoordIn;\n"
            "\n"
            "uniform mat4 projectionMatrix;\n"
            "uniform mat4 viewMatrix;\n"
            "\n"
            "varying vec2 textureCoordOut;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    textureCoordOut = textureCoordIn;\n"
            "    gl_Position = projectionMatrix * viewMatrix * position;\n"
            "}\n";

    fragmentShader =
#if JUCE_OPENGL_ES
            "precision lowp float;\n"
            "varying lowp vec2 textureCoordOut;\n"
#else
            "varying vec2 textureCoordOut;\n"
#endif
            "\n"
            "uniform sampler2D texture;\n"
            "\n"
            "void main()\n"
            "{\n"
            "    gl_FragColor = texture2D (texture, textureCoordOut);\n"
            "}\n";

    // An OpenGLShaderProgram is a combination of shaders which are compiled and linked together
    shaderProgram.reset(new juce::OpenGLShaderProgram(context));

    // Create the program that combines Vertex and Fragment shaders
    if (shaderProgram->addVertexShader(juce::OpenGLHelpers::translateVertexShaderToV3(vertexShader))
          && shaderProgram->addFragmentShader(juce::OpenGLHelpers::translateFragmentShaderToV3(fragmentShader))
          && shaderProgram->link())
    {
        // Do the following to be in a known state in case problems occur
        //shape = nullptr;
        attributes = nullptr;
        uniforms = nullptr;

        // Load the object that will be used to defined vertices and fragments
        //shape      = new Shape (context);                             // The object itself
        /* here or in render?
        // Tell the GPU to use the specified vertex and index buffers for next frame
        context.extensions.glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
        context.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
        */
        
        attributes.reset(new Attributes(context, *shaderProgram));    // The object attributes
        uniforms.reset(new Uniforms(context, *shaderProgram));      // The global data to be shared between
    }
}

//==============================================================================
// This matrix will be constant as it will be used by the GPU
// to modify the coordinates of each vertex based on the "frustum" defining the
// view from the virtual camera that "sees" the 3-D view. The frustum is just
// a truncated horizontal pyramid whose base represents the far side of the view
// (background) and the top is the near side of the view. Only the objects inside
// this truncated pyramid will be displayed by the GPU.
// (Use https://en.wikipedia.org/wiki/Viewing_frustum for a graphic explanation).
// The multiplication by this matrix will be the final step used by the GPU
// (in the Vertex shader) to create the object positions in the final 3-D view.

juce::Matrix3D<float> OpenGLWrapper::getProjectionMatrix() const
{
    float w = 1.0f;
    float h = w * getLocalBounds().toFloat().getAspectRatio (false);
    // All objects coordinates in the pre-final view will be modified by multiplying
    // their position vectors by this matrix (see getViewMatrix() below).
    // The first 4 parameters define the view size, the 5th one is a factor that
    // defines from how far the near side will be seen.
    // The 6th one is a factor that defines from how far the far side will be seen.
    // Obiviously this 6th param should be greater than the 5th one.
    // All the values used in this method are somewhat empirical and depend on
    // what you want to show and how close (big) the displayed objects will be.
    return juce::Matrix3D<float>::fromFrustum (-w, w, -h, h, 1.0f, 20.0f);
}
//==============================================================================

void OpenGLWrapper::addCubemap(const std::array<const juce::Image, 6> images)
{
    juce::Image* cubemapImage = cubemaps.add(std::make_unique<juce::Image>(juce::Image::RGB, FACE_SIZE*4, FACE_SIZE*2, true));
    juce::Graphics g (*cubemapImage);
    for (int i = 0; i < 6; i++)
    {
        g.drawImageAt(images[i].rescaled(FACE_SIZE,FACE_SIZE), FACE_SIZE*(i%3), FACE_SIZE*(i>2));
    }
    
    
    if (juce::OpenGLHelpers::isContextActive()) {
        juce::OpenGLTexture* tex = cubemapTextures.add(std::make_unique<juce::OpenGLTexture>());
        tex->loadImage(*cubemapImage);
    }
}

void OpenGLWrapper::addShape(const juce::Vector3D<float> dimensions)
{
    cubeDimensions.add(dimensions);
    if (juce::OpenGLHelpers::isContextActive())
        shapes.add(new Shape(context, dimensions));
}
