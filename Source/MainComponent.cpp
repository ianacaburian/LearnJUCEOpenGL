/*
  ==============================================================================

    This file was auto-generated!

  ==============================================================================
*/

#include "MainComponent.h"
//==============================================================================
MainComponent::MainComponent()
{
    shader_program_source = parseShaders();
    openGLContext.setOpenGLVersionRequired(juce::OpenGLContext::openGL3_2);
    openGLContext.setRenderer(this);
    openGLContext.attachTo(*this);
    openGLContext.setContinuousRepainting(true);
    setSize (800, 600);
}

MainComponent::~MainComponent()
{
    openGLContext.detach();
}
MainComponent::ShaderProgramSource MainComponent::parseShaders()
{
    auto shader_folder = File::getCurrentWorkingDirectory();
    while (!shader_folder.isRoot()) {
        if (shader_folder.isDirectory()
            && shader_folder.getFileName() == JUCEApplication::getInstance()->getApplicationName()) {
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
GLuint MainComponent::createShader(const GLenum type, const GLchar* source, const GLint source_length)
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
void MainComponent::loadShaders(const ShaderProgramSource& source)
{
    const auto vxID = createShader(GL_VERTEX_SHADER, source.VertexSource.getCharPointer(),
                                   sizeof(char) * source.VertexSource.length());
    const auto fsID = createShader(GL_FRAGMENT_SHADER, source.FragmentSource.getCharPointer(),
                                   sizeof(char) * source.FragmentSource.length());
    shader_prog_ID = GL::glCreateProgram();
    GL::glAttachShader(shader_prog_ID, vxID);
    GL::glAttachShader(shader_prog_ID, fsID);
    GL::glLinkProgram(shader_prog_ID);
    
    // check program linking success
    GLint success;
    GL::glGetProgramiv(shader_prog_ID, GL_LINK_STATUS, &success);
    if (!success) {
        char infoLog[512];
        GL::glGetProgramInfoLog(shader_prog_ID, 512, nullptr, infoLog);
        DBG(infoLog);
        jassertfalse;
    }
    GL::glDeleteShader(vxID);
    GL::glDeleteShader(fsID);
}

void MainComponent::newOpenGLContextCreated()
{
    loadShaders(shader_program_source);
    uniform_loc = GL::glGetUniformLocation(shader_prog_ID, "distance");
    
    GL::glGenBuffers(num_buffers_to_generate, &vertex_buff_ID);
    GL::glBindBuffer(GL_ARRAY_BUFFER, vertex_buff_ID);
    GL::glBufferData(GL_ARRAY_BUFFER, positions_count * sizeof(float), positions, GL_STATIC_DRAW);
    
    GL::glGenBuffers(num_buffers_to_generate, &index_buff_ID);
    GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buff_ID);
    GL::glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices_count * sizeof(unsigned int), indices, GL_STATIC_DRAW);
}

void MainComponent::renderOpenGL()
{
    GL::glUseProgram(shader_prog_ID);
    const auto in_distance = std::abs(std::sin(Time::currentTimeMillis() / 1000.) * 1000.);
    glUniform1f(uniform_loc, in_distance);

    GL::glBindBuffer(GL_ARRAY_BUFFER, vertex_buff_ID);
    GL::glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buff_ID);
    
    const auto pos_attrib_id = 0;
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
