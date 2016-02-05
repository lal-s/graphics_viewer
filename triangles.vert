#version 430 core

in vec3 vertexPosition_modelspace;

layout(location = 1) in vec3 vertexColor;
out vec3 fragmentColor;
uniform mat4 MVP;
 
void main(){
 
    fragmentColor = vertexColor;
    vec4 v = vec4(vertexPosition_modelspace,1); 
    gl_Position = MVP * v;
}
