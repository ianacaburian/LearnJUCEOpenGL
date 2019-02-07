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
class MainComponent   : public Component, public OpenGLRenderer, public Timer
{
public:
    //==============================================================================
    MainComponent();
    ~MainComponent();

    //==============================================================================
    void newOpenGLContextCreated() override;
    void renderOpenGL() override;
    void openGLContextClosing() override;
    void paint(Graphics& g) override;
    void resized() override;
    void timerCallback() override;
    
private:
    struct ShaderProgramSource
    {
        String VertexSource;
        String FragmentSource;
    };
    //==============================================================================

    // OpenGL
    OpenGLContext openGL_context;
    ShaderProgramSource shader_program_source;
    GLuint vertex_buff_ID, index_buff_ID, shader_prog_ID;
    GLint uniform_loc;
    
    static constexpr int positions_count = 8;    
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
    // "Position" vertex attribute parameters
    static constexpr int pos_attrib_id = 0;
    static constexpr int num_floats_per_pos_attrib = 2;

    // JUCE Benchmark
    TextButton openGL_button{ "OpenGL" };
    double prev_time{}, frame_time{};
    int frame_count{};
    
    //==============================================================================

    static ShaderProgramSource parse_shaders();
    static GLuint create_shader(const GLenum type, const GLchar* source, const GLint source_length);
    static GLuint create_program(const ShaderProgramSource& source);
    void init_button();
    void time_frames();
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MainComponent)
};
