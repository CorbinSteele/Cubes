/*
  ==============================================================================

    OpenGLWrapper.h
    Created: 27 Mar 2021 8:37:54am
    Author:  corbin

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "CubeCalendarData.h"
#include "Cube.h"

//==============================================================================
/*
*/
class OpenGLWrapper  : public juce::Component, public juce::OpenGLRenderer
{
public:    
    OpenGLWrapper();
    ~OpenGLWrapper() override;
    
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;
    
    void initCubes(CubeCalendarData& calendarData);
    
    static const int FACE_SIZE {512};
    
    juce::OwnedArray<Cube> cubes;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OpenGLWrapper)
    
    void addCubemap(const std::array<const juce::Image, 6> images);
    void addShape(const juce::Vector3D<float> dimensions);
    
    const char* vertexShader;   // Source code for the vertx shader
    const char* fragmentShader; // Source code for the fragment shader
    void createShaders();
    
    juce::OpenGLContext context;
    std::unique_ptr<juce::OpenGLShaderProgram> shaderProgram;   // Set of vertex + fragment shaders
    
    juce::OwnedArray<juce::Image> cubemaps;
    juce::OwnedArray<juce::OpenGLTexture> cubemapTextures;
    
    juce::Matrix3D<float> getProjectionMatrix() const;
    
    
    //==============================================================================
    struct Vertex
    {
        float position[3];  // To define vertex x,y,z coordinates
        float texCoord[2];  // A graphic image (file) can be used to define the texture of the drawn object.
                            // This 2-D vector gives the coordinates in the 2-D image file corresponding to
                            // the pixel color to be drawn
    };

    //==============================================================================
    // This class just manages the attributes that the shaders use.
    // "attribute" is a special variable type modifier in the shaders which allows to pass information
    // from the CPU code to the shaders. These attributes will be passed to the Vertex shader
    // to define the coordinates, normal vector, color and texture coordinate of each vertex.
    // Note that an attribute variable can be a scalar, a vector, a matrix, etc.
    struct Attributes
    {
        Attributes(juce::OpenGLContext& openGLContext, juce::OpenGLShaderProgram& shaderProgram)
        {
            // Call openGL functions to get the ID (a number specific to each object or variable)
            // corresponding to the attribute whose name is given as 3rd parameter.
            // This id will be used below to tell the GPU how to use them
            position.reset(createAttribute (openGLContext, shaderProgram, "position"));
            textureCoordIn.reset(createAttribute (openGLContext, shaderProgram, "textureCoordIn"));
        }
        ~Attributes()
        {
            position = nullptr;
            textureCoordIn = nullptr;
        }

        // This method calls openGL functions to tell the GPU that some attributes will be used
        // for each vertex (see comments below) and will be passed as an array of data
        void enable(juce::OpenGLContext& openGLContext)
        {
            if (position != nullptr)
            {
                // Tell the GPU that the first attribute will be the position attribute
                // 2nd parameter gives the number of data (3 coordinates) for this attribute
                // 3rd parameter gives their type (floating-point)
                // 4th parameter indicates they will be left as is (not normalized)
                // 5th parameter indicates the size of the array defined for each stored element (vertex)
                // 6th parameter is the offset in that array for the given attribute in current element
                openGLContext.extensions.glVertexAttribPointer (position->attributeID, 3, GL_FLOAT, GL_FALSE, sizeof (Vertex), 0);
                openGLContext.extensions.glEnableVertexAttribArray (position->attributeID);
            }

            if (textureCoordIn != nullptr)
            {
                // Tell the GPU that the next attribute will be the texture coordinate attribute
                // 2nd parameter gives the number of data (x and y) for this attribute
                // 3rd parameter gives their type (floating-point)
                // 4th parameter indicates they will be left as is (not normalized)
                // 5th parameter indicates the size of the array defined for each stored element (vertex)
                // 6th parameter is the byte offset in that array for the given attribute in current element (0+3+3+4 float)
                openGLContext.extensions.glVertexAttribPointer (textureCoordIn->attributeID, 2, GL_FLOAT, GL_FALSE, sizeof (Vertex), (GLvoid*) (sizeof (float) * 3));
                openGLContext.extensions.glEnableVertexAttribArray (textureCoordIn->attributeID);
            }
        }

        // This method calls openGL functions to tell the GPU to release the resources previously used to store attributes
        void disable(juce::OpenGLContext& openGLContext)
        {
            if (position != nullptr)       openGLContext.extensions.glDisableVertexAttribArray (position->attributeID);
            if (textureCoordIn != nullptr)  openGLContext.extensions.glDisableVertexAttribArray (textureCoordIn->attributeID);
        }

        std::unique_ptr<juce::OpenGLShaderProgram::Attribute> position, textureCoordIn;

    private:
        // This method calls openGL functions to get the ID (a number specific to each object or variable,
        // which is assigned by the GPU itself) corresponding to a certain attribute name, and create the
        // attribute for the OpenGL (CPU) world.
        // Basically this will allow to link a variable in the CPU code to one in the GPU (GLSL) shader.
        // Note that the variable can be a scalar, a vector, a matrix, etc.
        static juce::OpenGLShaderProgram::Attribute* createAttribute(juce::OpenGLContext& openGLContext,
                                                                juce::OpenGLShaderProgram& shaderProgram,
                                                                const char* attributeName)
        {
            // Get the ID
            if (openGLContext.extensions.glGetAttribLocation(shaderProgram.getProgramID(), attributeName) < 0)
                return nullptr; // Return if error
            // Create the atttribute variable
            return new juce::OpenGLShaderProgram::Attribute(shaderProgram, attributeName);
        }
    };

    //==============================================================================
    // This class just manages the uniform values that the demo shaders use.
    // "uniform" is a special variable type modifier in the shaders which allows to pass global
    // variables between the CPU code and the shaders.
    // These uniform global variables can be used by both the Vertex and the Fragment shaders.
    // Contrary to attributes, the uniform variables are not specific to vertices but are global.
    struct Uniforms
    {
        Uniforms(juce::OpenGLContext& openGLContext, juce::OpenGLShaderProgram& shaderProgram)
        {
            // Here we define 2 global variables (matrices)
            projectionMatrix.reset(createUniform (openGLContext, shaderProgram, "projectionMatrix"));
            viewMatrix.reset(createUniform (openGLContext, shaderProgram, "viewMatrix"));
        }
        ~Uniforms()
        {
            projectionMatrix = nullptr;
            viewMatrix = nullptr;
        }

        std::unique_ptr<juce::OpenGLShaderProgram::Uniform> projectionMatrix, viewMatrix;

    private:
        // This method calls openGL functions to get the ID (a number specific to each object or variable,
        // which is assigned by the GPU itself) corresponding to a certain uniform name, and create the
        // global variable for the OpenGL (CPU) world.
        // Basically this will allow to link a variable in the CPU code to one in the GPU (GLSL) shaders.
        // Note that the variable can be a scalar, a vector, a matrix, etc.
        static juce::OpenGLShaderProgram::Uniform* createUniform(juce::OpenGLContext& openGLContext,
                                                            juce::OpenGLShaderProgram& shaderProgram,
                                                            const char* uniformName)
        {
            // Get the ID
            if (openGLContext.extensions.glGetUniformLocation(shaderProgram.getProgramID(), uniformName) < 0)
                return nullptr; // Return if error
            // Create the uniform variable
            return new juce::OpenGLShaderProgram::Uniform(shaderProgram, uniformName);
        }
    };
    
    //==============================================================================
    /** This loads a 3D model from an OBJ file and converts it into some vertex buffers
        that we can draw.
        The OBJ format is a de facto standard that has been defined by a private company
        (Wavefront Technologies) to describe the contents of a vertex buffer.
        You can use software like Blender to create a 3D-object and export it as an OBJ file.
        Jules has done the routines to interpret this format and convert them to store
        the corresponding data into vertex buffers (see WaveFrontObjParser.h)
    */
    struct Shape
    {
        Shape(juce::OpenGLContext& openGLContext, juce::Vector3D<float> d)
        {
            juce::Array<Vertex> vertices;
            #define V(vx,vy,vz,tx,ty) vertices.add(Vertex{{vx##.0f*d.x,vy##.0f*d.y,vz##.0f*d.z},{tx##.0f/4.0f,ty##.0f/2.0f}})
            // Vertex data for face 0
            V(-1,-1,1,0,0);
            V(1,-1,1,1,0);
            V(-1,1,1,0,1);
            V(1,1,1,1,1);
            // Vertex data for face 1
            V(1,-1,1,0,1);
            V(1,-1,-1,1,1);
            V(1,1,1,0,2);
            V(1,1,-1,1,2);
            // Vertex data for face 2
            V(1,-1,-1,3,2);
            V(-1,-1,-1,2,2);
            V(1,1,-1,3,1);
            V(-1,1,-1,2,1);
            // Vertex data for face 3
            V(-1,-1,-1,2,0);
            V(-1,-1,1,3,0);
            V(-1,1,-1,2,1);
            V(-1,1,1,3,1);
            // Vertex data for face 4
            V(-1,-1,-1,1,0);
            V(1,-1,-1,2,0);
            V(-1,-1,1,1,1);
            V(1,-1,1,2,1);
            // Vertex data for face 5
            V(-1,1,1,1,1);
            V(1,1,1,2,1);
            V(-1,1,-1,1,2);
            V(1,1,-1,2,2);
            #undef V
            
            juce::Array<juce::uint32> indices {
                0,  1,  2,  2, 1, 3,        // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
                4,  5,  6,  6,  5,  7,      // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
                8,  9,  10, 10, 9, 11,      // Face 2 - triangle strip ( v8,  v9, v10, v11)
                12, 13, 14, 14, 13, 15,     // Face 3 - triangle strip (v12, v13, v14, v15)
                16, 17, 18, 18, 17, 19,     // Face 4 - triangle strip (v16, v17, v18, v19)
                20, 21, 22, 22, 21, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
            };
            /*
            juce::Array<juce::uint32> indices {
                0,  1,  2,  3,  3,     // Face 0 - triangle strip ( v0,  v1,  v2,  v3)
                4,  4,  5,  6,  7,  7, // Face 1 - triangle strip ( v4,  v5,  v6,  v7)
                8,  8,  9, 10, 11, 11, // Face 2 - triangle strip ( v8,  v9, v10, v11)
                12, 12, 13, 14, 15, 15, // Face 3 - triangle strip (v12, v13, v14, v15)
                16, 16, 17, 18, 19, 19, // Face 4 - triangle strip (v16, v17, v18, v19)
                20, 20, 21, 22, 23      // Face 5 - triangle strip (v20, v21, v22, v23)
            };*/
            shapeData.reset(new ShapeData(openGLContext, vertices, indices));
        }
        Shape(juce::OpenGLContext& openGLContext, juce::Array<Vertex>& vertices, juce::Array<juce::uint32>& indices)
        {
            shapeData.reset(new ShapeData(openGLContext, vertices, indices));
        }
        ~Shape()
        {
            shapeData = nullptr;
        }

        // Send the buffer to the GPU to execute the vertex shader
        void draw (juce::OpenGLContext& openGLContext, Attributes& attributes)
        {
            ShapeData& data = *shapeData;
            // Tell the GPU this is the buffer to be used
            data.bind();

            // Draw triangles (atomic graphic structure for OpenGL)
            // using the vertices stored in the vertexBuffer
            attributes.enable(openGLContext);
            glDrawElements (GL_TRIANGLES, data.numIndices, GL_UNSIGNED_INT, 0);
            attributes.disable(openGLContext);
        }

    private:
        struct ShapeData
        {
            ShapeData(juce::OpenGLContext& context, juce::Array<Vertex>& vertices, juce::Array<juce::uint32>& indices) : openGLContext(context)
            {
                // Indices are used to be more efficient. They allow for instance to define less vertices
                // when they are used more than once. For instance to draw a square using triangles (the
                // most basic shape used by GPUs) you need 2 triangles x 3 vertices, i.e. 6 vertices.
                // But in fact 2 of them are common to the 2 triangles. By indexing the vertices you can
                // significantly reduce the calls to the Vertex Shader for more complex structures.
                numIndices = indices.size();
                
                // These will call OpenGL routines to create a vertex buffer (VBO)
                openGLContext.extensions.glGenBuffers(1, &vertexBuffer);
                openGLContext.extensions.glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
                
                // This will fill in the vertex buffer with the vertex info data that was stored in the OBJ file
                // Note that a default colour is defined here in case no info was given for the color.
                openGLContext.extensions.glBufferData (GL_ARRAY_BUFFER,
                                                       static_cast<GLsizeiptr> (static_cast<size_t> (vertices.size()) * sizeof (Vertex)),
                                                       vertices.getRawDataPointer(), GL_STATIC_DRAW);

                // These will call OpenGL routines to create a buffer and store the indices in it
                openGLContext.extensions.glGenBuffers (1, &indexBuffer);
                openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
                openGLContext.extensions.glBufferData (GL_ELEMENT_ARRAY_BUFFER,
                                                       static_cast<GLsizeiptr> (static_cast<size_t> (numIndices) * sizeof (juce::uint32)),
                                                       indices.getRawDataPointer(), GL_STATIC_DRAW);
            }

            ~ShapeData()
            {
                openGLContext.extensions.glDeleteBuffers (1, &vertexBuffer);
                openGLContext.extensions.glDeleteBuffers (1, &indexBuffer);
            }

            void bind()
            {
                // Tell the GPU to use the specified vertex and index buffers for next frame
                openGLContext.extensions.glBindBuffer (GL_ARRAY_BUFFER, vertexBuffer);
                openGLContext.extensions.glBindBuffer (GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
            }

            GLuint vertexBuffer, indexBuffer;
            int numIndices;
            juce::OpenGLContext& openGLContext;

            JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ShapeData)
        };

        std::unique_ptr<ShapeData> shapeData;
    };
    
    std::unique_ptr<Attributes> attributes;           // Variables sent to the vertex shader
    std::unique_ptr<Uniforms> uniforms;               // Variables sent to both the vertex and fragment shaders
    juce::Array<juce::Vector3D<float>> cubeDimensions;
    juce::OwnedArray<Shape> shapes;
};
