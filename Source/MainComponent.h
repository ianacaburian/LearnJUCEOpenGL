/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
//==============================================================================
/*
    This component lives inside our window, and this is where you should put all
    your controls and content.
*/
using GL = juce::OpenGLExtensionFunctions;
class MainComponent   : public Component, public OpenGLRenderer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void newOpenGLContextCreated() override;
    
    void renderOpenGL() override;
    
    void openGLContextClosing() override;
    
private:
    struct ShaderProgramSource
    {
        String VertexSource;
        String FragmentSource;
    };
    //==============================================================================

    OpenGLContext openGL_context;
    ShaderProgramSource shader_program_source;
    GLuint vertex_buff_ID, index_buff_ID, shader_prog_ID;
    GLint uniform_loc;
    
    static constexpr int num_buffers_to_generate = 1;
    static constexpr int positions_count = 8;
    static constexpr int num_vertices = 4;
    static constexpr int num_floats_per_pos_attrib = 2;
    GLfloat positions[positions_count] = {
        -1.0f, -1.0f,
         1.0f, -1.0f,
         1.0f,  1.0f,
        -1.0f,  1.0f
    };
    static constexpr int indices_count = 6;
    GLuint indices[indices_count] = {
        0, 1, 2,
        2, 3, 0
    };
    static ShaderProgramSource parse_shaders();
    static GLuint create_shader(const GLenum type, const GLchar* source, const GLint source_length);
    void load_shaders(const ShaderProgramSource& source);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
