/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
//==============================================================================
MainComponent::MainComponent()
{
    shader_program_source = parse_shaders();
    openGL_context.setOpenGLVersionRequired(juce::OpenGLContext::openGL3_2);
    openGL_context.setRenderer(this);
    openGL_context.setContinuousRepainting(true);
    
    // Benchmarking-only
    openGL_context.setComponentPaintingEnabled(true);
    setPaintingIsUnclipped(true);
    setOpaque(true);
    init_button();
    startTimerHz(software_fps_requested);
    
    setSize(screen_resolution.x, screen_resolution.y);
}
MainComponent::~MainComponent()
{
    openGL_context.detach();
}
void MainComponent::newOpenGLContextCreated()
{
    // Create the shader program and specify the CPU-computed variables (uniforms) required for animation.
    shader_prog_ID = create_program(shader_program_source);
    GL::glUseProgram(shader_prog_ID);
    
    // The resolution is static, computed at app initialization, and thus can afford to be sent just once here.
    uf_resolution = GL::glGetUniformLocation(shader_prog_ID, "resolution");
    GL::glUniform2f(uf_resolution, screen_resolution.x, screen_resolution.y);

    // The distance variable is dynamic, and thus is only declared here.
    // Sending it to the GPU is deferred as it must be computed and sent at each draw call.
    uf_distance = GL::glGetUniformLocation(shader_prog_ID, "distance");

    // Explicitly generate a vertex array (rather than use the default created one) to specify the vertex buffer layout in each draw call.
    // Otherwise, using the default requires that each draw call must bind the vertex buffer, enable the attrib array, and specify the attrib pointers.
    GL::glGenVertexArrays(1, &vertex_arr_ID);
    GL::glBindVertexArray(vertex_arr_ID);
    
    // Load the (x, y) positions (which are just the corners of the screen), into a static area of GPU memory.
    GL::glGenBuffers(1, &vertex_buff_ID);
    GL::glBindBuffer(GL_ARRAY_BUFFER, vertex_buff_ID);
    GL::glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * positions_count, positions, GL_STATIC_DRAW);
    
    // Specify the attribute layout of the currently bound vertex buffer object (there's only one attribute - position).
    // The currently instantiated vertex array object will use this specification for each draw call.
    GL::glEnableVertexAttribArray(pos_attrib_id);
    GL::glVertexAttribPointer(pos_attrib_id, num_floats_per_pos_attrib, GL_FLOAT, GL_FALSE,
                              sizeof(GLfloat) * num_floats_per_pos_attrib, (const void*)0);     // For more that one attrib, can use a struct with a C++ macro to determine this void* offset.

    // Specify which (x, y) positions are used to draw two triangles in the shape of a rectangle (the screen) using the positions in the currently bound vertex buffer object.
    GL::glGenBuffers(1, &index_buff_ID);
    GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buff_ID);
    GL::glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements_count, elements, GL_STATIC_DRAW);
    
    // No need to unbind the array, buffer, or shader program thanks to JUCE.
}
void MainComponent::renderOpenGL()
{
    // Bind shader program and send the CPU-computed distance variable (it needs for this frame) for animation.
    GL::glUseProgram(shader_prog_ID);
    const auto distance = std::abs(std::sin(Time::currentTimeMillis() / 1000.) * screen_resolution.y * 2);
    GL::glUniform1f(uf_distance, distance);
    
    // Binding the array simultaneously tells the GPU which vertex buffer to use as well as the specification of the buffer's memory layout.
    GL::glBindVertexArray(vertex_arr_ID);
    
    // The element (or index) buffer is still required to be bound at each draw call to specify how to use the positions in the buffer to draw the correct shape.
    GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buff_ID);
    glDrawElements(GL_TRIANGLES, elements_count, GL_UNSIGNED_INT, nullptr);
    
    // Benchmark the software renderer against the OpenGL renderer
    time_frames();
}
void MainComponent::openGLContextClosing()
{   // Destroying GL objects require that the openGL_context is not yet destroyed, i.e. cannot delete in the destructor.
    GL::glDeleteProgram(shader_prog_ID);
    GL::glDeleteBuffers(1, &vertex_buff_ID);
    GL::glDeleteBuffers(1, &index_buff_ID);
}
void MainComponent::paint(Graphics& g)
{
    if (! openGL_context.isAttached()) {
        const auto bounds = getLocalBounds().toFloat();
        g.setColour(Colours::black);
        g.fillRect(bounds);
        g.setColour(Colour::fromFloatRGBA(1.f, 0.f, 1.f, 0.8f));
        const auto d = std::abs(std::sin(Time::currentTimeMillis() / 1000.) * getHeight());
        g.drawEllipse(bounds.withSizeKeepingCentre(d, d), d / 12.f);
        
        // Benchmark the software renderer against the OpenGL renderer
        time_frames();
    }
}
void MainComponent::resized()
{
    openGL_button.setBoundsRelative(0.f, 0.f, 0.1f, 0.1f);
}
void MainComponent::timerCallback()
{
    repaint();
}
MainComponent::ShaderProgramSource MainComponent::parse_shaders()
{
    auto shader_folder = File::getCurrentWorkingDirectory();
    while (!shader_folder.isRoot()) {
        if (shader_folder.getFileName() == JUCEApplication::getInstance()->getApplicationName()) {
            shader_folder = shader_folder.getChildFile("Source/Shaders/");
            break;
        }
        shader_folder = shader_folder.getParentDirectory();
    }
    if (! shader_folder.exists()) {
        jassertfalse;
    }
    const auto vertex_file = shader_folder.getChildFile("Vert.vert");
    const auto fragment_file = shader_folder.getChildFile("Frag.frag");
    return { vertex_file.loadFileAsString(), fragment_file.loadFileAsString() };
}
GLuint MainComponent::create_shader(const GLenum type, const GLchar* source, const GLint source_length)
{
    const auto shID = GL::glCreateShader(type);
    GL::glShaderSource(shID, 1, &source, &source_length);
    GL::glCompileShader(shID);
    
    // Check shader compilation success
    GLint success;
    GL::glGetShaderiv(shID, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        GL::glGetShaderInfoLog(shID, 512, nullptr, infoLog);
        DBG(infoLog);
        jassertfalse;
    }
    return shID;
}
GLuint MainComponent::create_program(const ShaderProgramSource& source)
{
    const auto vxID = create_shader(GL_VERTEX_SHADER, source.VertexSource.getCharPointer(),
                                    sizeof(GLchar) * source.VertexSource.length());
    const auto fsID = create_shader(GL_FRAGMENT_SHADER, source.FragmentSource.getCharPointer(),
                                    sizeof(GLchar) * source.FragmentSource.length());
    const auto spID = GL::glCreateProgram();
    GL::glAttachShader(spID, vxID);
    GL::glAttachShader(spID, fsID);
    GL::glLinkProgram(spID);
    
    // Check program linking success
    GLint success;
    GL::glGetProgramiv(spID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        GL::glGetProgramInfoLog(spID, 512, nullptr, infoLog);
        DBG(infoLog);
        jassertfalse;
    }
    GL::glDeleteShader(vxID);
    GL::glDeleteShader(fsID);
    return spID;
}
void MainComponent::init_button()
{
    addAndMakeVisible(openGL_button);
    auto&& laf = getLookAndFeel();
    laf.setColour(TextButton::buttonColourId, Colours::black);
    laf.setColour(TextButton::buttonOnColourId, Colours::black);
    laf.setColour(TextButton::textColourOnId, Colours::limegreen);
    laf.setColour(TextButton::textColourOffId, Colours::white);

    openGL_button.setClickingTogglesState(true);
    openGL_button.onClick = [this] {
        if (openGL_button.getToggleState()) {
            stopTimer();
            openGL_context.attachTo(*this);
        }
        else {
            openGL_context.detach();
            startTimerHz(software_fps_requested);
        }
    };
}
void MainComponent::time_frames()
{
    const auto current_time = Time::currentTimeMillis();
    frame_count++;
    if (current_time - prev_time >= 1000. ){
        frame_time = 1000. / frame_count;
        DBG(String::formatted("%f ms/frame", frame_time));
        frame_count = 0;
        prev_time = current_time;
    }
}
