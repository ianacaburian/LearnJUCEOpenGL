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
    openGL_context.attachTo(*this);
    openGL_context.setContinuousRepainting(true);
    setSize (800, 600);
}

MainComponent::~MainComponent()
{
    openGL_context.detach();
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
    
    // check shader compilation success
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
                                   sizeof(char) * source.VertexSource.length());
    const auto fsID = create_shader(GL_FRAGMENT_SHADER, source.FragmentSource.getCharPointer(),
                                   sizeof(char) * source.FragmentSource.length());
    const auto spID = GL::glCreateProgram();
    GL::glAttachShader(spID, vxID);
    GL::glAttachShader(spID, fsID);
    GL::glLinkProgram(spID);
    
    // check program linking success
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
void MainComponent::newOpenGLContextCreated()
{
    shader_prog_ID = create_program(shader_program_source);
    uniform_loc = GL::glGetUniformLocation(shader_prog_ID, "distance");
    
    GL::glGenBuffers(1, &vertex_buff_ID);
    GL::glBindBuffer(GL_ARRAY_BUFFER, vertex_buff_ID);
    GL::glBufferData(GL_ARRAY_BUFFER, positions_count * sizeof(float), positions, GL_STATIC_DRAW);
    
    GL::glGenBuffers(1, &index_buff_ID);
    GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buff_ID);
    GL::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}

void MainComponent::renderOpenGL()
{
    GL::glUseProgram(shader_prog_ID);
    const auto distance = std::abs(std::sin(Time::currentTimeMillis() / 1000.) * 1000.);
    glUniform1f(uniform_loc, distance);

    GL::glBindBuffer(GL_ARRAY_BUFFER, vertex_buff_ID);
    GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buff_ID);    
    
    GL::glVertexAttribPointer(pos_attrib_id, num_floats_per_pos_attrib, GL_FLOAT, GL_FALSE,
                              num_floats_per_pos_attrib * sizeof(float), (const void*)0); // pointer can be determined by using c++ MACRO
    GL::glEnableVertexAttribArray(pos_attrib_id);
    
    glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, nullptr);
}

void MainComponent::openGLContextClosing()
{
    GL::glDeleteProgram(shader_prog_ID);
    GL::glDeleteBuffers(1, &vertex_buff_ID);
    GL::glDeleteBuffers(1, &index_buff_ID);
}
