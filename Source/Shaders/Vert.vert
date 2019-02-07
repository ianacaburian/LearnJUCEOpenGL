//#version 330 core
// On macOS, only shader version 150 works, 330 (and thus "layout") isn’t supported
// layout(location = 0) in vec4 position;
#version 150

in vec4 position;

void main()
{
    gl_Position = position;
}
