#version 430 core

out vec3 color;
in vec3 fragmentColor;
void
main()
{
   color = fragmentColor;
}


