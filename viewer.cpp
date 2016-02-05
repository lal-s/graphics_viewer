// ch01.cpp : Defines the entry point for the console application.
//
//////////////////////////////////////////////////////////
// reference from triangles.cpp from the OpenGL Red Book Chapter 1
//////////////////////////////////////////////////////////
#include <iostream>
#include<stdio.h>
using namespace std;
#include <vector>
#include<string.h>
#include "vgl.h"
#include "LoadShaders.h"
#include <math.h>
// includes for glm
#include "glm/glm.hpp"
#include "glm/mat4x4.hpp"
// include glm extensions
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
using namespace std;
const GLuint Triangles = 0, NumVAOs = 1;
const GLuint ArrayBuffer=0, NumBuffers=1;
const GLuint vPosition=0;
float Angle = 45.0f;
float look1=0;
float look2=0;
float look3=0;
float cam1=4;
float cam2=3;
float cam3=3;
float degree1 = cam2/ cam1;
float up1=0;
float up2=0;
float up3=1;
float degree=0.0f;
GLuint colorbuffer;
float t1=0.0f;
float t2=0.0f;
float t3=0.0f;
float s1=1.0f;
float s2=1.0f;
float s3=1.0f;
float aa[3]={0.0,0.0,1.0};
glm::mat4 viewUpVector=glm:: make_mat4 (aa);
glm::mat4 Model;
glm::mat4 Projection;
glm::mat4 View;
glm::mat4 MVP ;
GLuint program;
GLuint MatrixID;
float ka_def= 0.1f;
float kd_def= 0.9f;
void MyKeyboardFunc(unsigned char Key, int x, int y);
void SpecialInput(int key, int x, int y);
void init_display();
void display_bind();
void read_controlfile(char *namefile);
GLuint VAOs[NumVAOs];
GLuint Buffers[NumBuffers];
const GLuint NumVertices = 6;
std::vector< glm::vec3 > read_file();
FILE * file1;
FILE * file2;
char file_name[128];
float near=1.0f;
float far=100.0f;
std::vector< glm::vec3 > vertices;
std::vector< float > temp_color1;
/////////////////////////////////////////////////////
// init funtion
/////////////////////////////////////////////////////
void init(void )
{
    glGenVertexArrays( NumVAOs, VAOs );
    glBindVertexArray( VAOs[Triangles] );
    glGenBuffers(1, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, temp_color1.size()*sizeof(glm::vec3), &temp_color1[0], GL_STATIC_DRAW);
    //check size
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(
    1, // attribute. No particular reason for 1, but must match the layout in the shader.
    3, // size
    GL_FLOAT, // type
    GL_FALSE, // normalized?
    0, // stride
    (void*)0 // array buffer offset
    );
    Projection = glm::frustum( -0.1f, 0.1f, -0.1f, 0.1f, near, far );
    // Camera matrix
    View = glm::lookAt(
    glm::vec3(cam1,cam2,cam3), // Camera is at (4,3,3), in World Space
    glm::vec3(look1,look2,look3), // and looks at the origin
    glm::vec3(up1,up2,up3) // Head is up (set to 0,-1,0 to look upside-down)
    );
    MVP = Projection * View *Model;
    glGenBuffers( NumBuffers, Buffers );
    glBindBuffer( GL_ARRAY_BUFFER, Buffers[ArrayBuffer] );
    glBufferData( GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW );
    ShaderInfo shaders[] = {
        { GL_VERTEX_SHADER, "triangles.vert" },
        { GL_FRAGMENT_SHADER, "triangles.frag" },
        { GL_NONE, NULL }
    };
    program = LoadShaders( shaders );
    glUseProgram( program );
    GLuint MatrixID = glGetUniformLocation(program, "MVP");
    glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
    glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
    glEnableVertexAttribArray( 0 );
}
void init1 (void )
{
    int count_face=0;
    char file_color[128];
    int i=0;
    int len;
    float tempkaklx;
    float tempkakly;
    float tempkaklz;
    std::vector< glm::vec3 > vert_temp= read_file();
    glm :: vec3 vert_t;
    for(int ff=0; ff< vert_temp.size();ff++)
    {
        vert_t= vert_temp[ff];
        vertices.push_back(vert_t);
    }
    float max_x=vertices[0].x;
    float max_y=vertices[0].y;
    float max_z=vertices[0].z;;
    for(int vv=0;vv< vertices.size();vv++)
    {
        max_x=std::max(vertices[vv].x,max_x);
        max_y=std::max(vertices[vv].y,max_y);
        max_z=std::max(vertices[vv].y,max_y);
    }
    cam1=3* max_x;
    cam2=3* max_y;
    cam3=3* max_z;
    std::vector< glm::vec3 > normal;
    std::vector< glm::vec3 > kakd;
    glm :: vec3 normal_vertex;
    glm :: vec3 p1;
    glm :: vec3 p2;
    glm :: vec3 p3;
    glm :: vec3 u;
    glm :: vec3 v;
    std::vector< glm::vec3 > ka;
    glm :: vec3 ka_vertex;
    float nl;
    glm :: vec3 temp_color_vert;
    glm :: vec3 kd_vertex;
    glm :: vec3 light;
    unsigned int vertex_color[3];
    unsigned int uv_color[3];
    unsigned int normal_color[3];
    std::vector< glm::vec3 > kd ;
    std::vector< string > color ;
    int count_mtl=0;
    std::vector< glm::vec3 > shade;
    /////////////////opening mtl file dynamically///////////////////////////////
    int xx;
    int k=0;
    for(xx=0; xx<vert_temp.size();xx=xx+3)
    {
        p1=vert_temp[xx];
        p2=vert_temp[xx+1];
        p3=vert_temp[xx+2];
        u.x=p2.x-p1.x;
        u.y=p2.y-p1.y;
        u.z=p2.z-p1.z;
        v.x=p3.x-p1.x;
        v.y=p3.y-p1.y;
        v.z=p3.z-p1.z;
        normal_vertex.x=u.y*v.z-u.z*v.y;
        normal_vertex.y=u.z*v.x-u.x*v.z;
        normal_vertex.z=u.x*v.y-u.y*v.x;
        normal.push_back(normalize(normal_vertex));
        normal.push_back(normalize(normal_vertex));
        normal.push_back(normalize(normal_vertex));
    }
    light.x=1;
    light.y=1;
    light.z=1;
    FILE * file3 = fopen(file_name, "r");
    if( file3 == NULL )
    {
        printf("Can't open the file\n");
        //return false;
    }
    while( 1 )
    {
        char lineHeader4[128];
        // read the first word of the line
        int res = fscanf(file3, "%s", lineHeader4);
        if (res == EOF)
            break; // EOF = End Of File. Quit the loop.
        if ( strcmp(lineHeader4, "mtllib" ) == 0 )
        {
            count_mtl++;
            fscanf(file3, " %s ",file_color);// it gets the name of the mtl file in file_color
        }
    }
    ////////////////////////////////////////////////////////////////////////////
    if(count_mtl>0)
    {
        FILE * file1 = fopen(file_color, "r");
        if( file1 == NULL ){
            printf("Can't open the file............\n");
        }
        while( 1 )
        {
            char lineHeader[128];
            char color1[50];
            int res = fscanf(file1, "%s", lineHeader);
            if (res == EOF)
                break; // EOF = End Of File. Quit the loop.
            if ( strcmp(lineHeader, "newmtl" ) == 0 )
            {
                fscanf(file1," %s", color1);
                color.push_back(color1);
            }//end of if
            if ( strcmp(lineHeader, "Ka" ) == 0 )
            {
                fscanf(file1, " %f %f %f ",&ka_vertex.x, &ka_vertex.y, &ka_vertex.z);
                ka.push_back(ka_vertex );
            }//end of if
            if ( strcmp(lineHeader, "Kd" ) == 0 )
            {
                fscanf(file1, " %f %f %f ",&kd_vertex.x, &kd_vertex.y, &kd_vertex.z);
                kd.push_back(kd_vertex );
            }//end of if
        }//end of while
    }//////end of if=mtl
    int cou=0;
    FILE * file7 = fopen(file_name, "r");
    if( file7 == NULL ){
        printf("Can't open the file............\n");
    }
    int count_header=0;
    while( 1 ){
        char lineHeader11[128];
        // read the first word of the line
        int res2 = fscanf(file7, "%s", lineHeader11);
        if (res2 == EOF)
            break;
        if ( strcmp(lineHeader11, "usemtl" ) == 0 )
        {count_header++;}
    }
    FILE * file2 = fopen(file_name, "r");
    if( file2 == NULL ){
        printf("Can't open the file............\n");
    }
    if(count_header>0)
    {
        while( 1 ){
            char lineHeader1[128];
            // read the first word of the line
            int res = fscanf(file2, "%s", lineHeader1);
            if (res == EOF)
                break; // EOF = End Of File. Quit the loop.
            char use_color[30];
            if ( strcmp(lineHeader1, "usemtl" ) == 0 ){
                fscanf(file2, " %s ",use_color);
                cout<<use_color;
                for( i=0;i< color.size(); i++)
                {
                    if(use_color==color[i])
                    {
                        break;
                    }}
                }
                if( strcmp( lineHeader1, "f")==0)
                {count_face++;
                    glm::vec3 temp11;
                    glm::vec3 temp22;
                    glm::vec3 temp33;
                    float ndotl1;
                    float ndotl2;
                    float ndotl3;
                    ndotl1 = dot(normal[k],light);
                    k++;
                    ndotl2 = dot(normal[k],light);
                    k++;
                    ndotl3 = dot(normal[k],light);
                    k++;
                    if(ndotl1<0)
                    ndotl1=0;
                    if(ndotl2<0)
                    ndotl2=0;
                    if(ndotl3<0)
                    ndotl3=0;
                    temp11.x=ka[i].x + ndotl1 * kd[i].x;
                    temp11.y=ka[i].y + ndotl1 * kd[i].y;
                    temp11.z=ka[i].z + ndotl1 * kd[i].z;
                    temp22.x=ka[i].x + ndotl2 * kd[i].x;
                    temp22.y=ka[i].y + ndotl2 * kd[i].y;
                    temp22.z=ka[i].z + ndotl2 * kd[i].z;
                    temp33.x=ka[i].x + ndotl3 * kd[i].x;
                    temp33.y=ka[i].y + ndotl3 * kd[i].y;
                    temp33.z=ka[i].z + ndotl3 * kd[i].z;
                    char c[1];
                    int i2 =0;
                    int t2=0;
                    int brac=0;
                    int brac1=0;
                    c[0]=getc(file2);
                    do {
                        if(c[0] == '/'&& brac==1)
                        {brac1=1;
                        }
                        c[0]=fgetc(file2);
                        if(c[0] == '/')
                        {brac=1;
                            t2++;
                        }
                        else{brac=0;
                        }
                    } while(c[0] != ' ');
                    fseek(file2,-i2-1 ,SEEK_CUR);
                    switch(t2)
                    {
                        case 0:
                        fscanf(file2, "%d %d %d\n", &vertex_color[0], &vertex_color[1],&vertex_color[2] );
                        temp_color1.push_back(temp11.x);
                        temp_color1.push_back(temp11.y);
                        temp_color1.push_back(temp11.z);
                        temp_color1.push_back(temp22.x);
                        temp_color1.push_back(temp22.y);
                        temp_color1.push_back(temp22.z);
                        temp_color1.push_back(temp33.x);
                        temp_color1.push_back(temp33.y);
                        temp_color1.push_back(temp33.z );
                        break;
                        case 1:
                        fscanf(file2, " %d/%d %d/%d %d/%d\n", &vertex_color[0], &uv_color[0], &vertex_color[1], &uv_color[1], &vertex_color[2], &uv_color[2] );
                        temp_color1.push_back(temp11.x);
                        temp_color1.push_back(temp11.y);
                        temp_color1.push_back(temp11.z);
                        temp_color1.push_back(temp22.x);
                        temp_color1.push_back(temp22.y);
                        temp_color1.push_back(temp22.z);
                        temp_color1.push_back(temp33.x);
                        temp_color1.push_back(temp33.y);
                        temp_color1.push_back(temp33.z );
                        break;
                        case 2:
                        fscanf(file2, " %d//%d %d//%d %d//%d", &vertex_color[0], &normal_color[0], &vertex_color[1], &normal_color[1],&vertex_color[2], &normal_color[2]);
                        temp_color1.push_back(temp11.x);
                        temp_color1.push_back(temp11.y);
                        temp_color1.push_back(temp11.z);
                        temp_color1.push_back(temp22.x);
                        temp_color1.push_back(temp22.y);
                        temp_color1.push_back(temp22.z);
                        temp_color1.push_back(temp33.x);
                        temp_color1.push_back(temp33.y);
                        temp_color1.push_back(temp33.z );
                        break;
                    }
                }
            }//end of while
        }
        else
        {
            FILE * file8 = fopen(file_name, "r");
            if( file8 == NULL ){
                printf("Can't open the file............\n");
            }
            while( 1 ){
                char lineHeader22[128];
                // read the first word of the line
                int res = fscanf(file8, "%s", lineHeader22);
                if (res == EOF)
                    break; // EOF = End Of File. Quit the loop.
                if ( strcmp(lineHeader22, "f" ) == 0 ){
                    count_face++;
                    glm::vec3 temp11;
                    glm::vec3 temp22;
                    glm::vec3 temp33;
                    float ndotl1;
                    float ndotl2;
                    float ndotl3;
                    ndotl1 = dot(normal[k],light);
                    k++;
                    ndotl2 = dot(normal[k],light);
                    k++;
                    ndotl3 = dot(normal[k],light);
                    k++;
                    if(ndotl1<0)
                    ndotl1=0;
                    if(ndotl2<0)
                    ndotl2=0;
                    if(ndotl3<0)
                    ndotl3=0;
                    temp11.x=ka_def + ndotl1 * kd_def;
                    temp11.y=ka_def + ndotl1 * kd_def;
                    temp11.z=ka_def + ndotl1 * kd_def;
                    temp22.x=ka_def + ndotl2 * kd_def;
                    temp22.y=ka_def + ndotl2 * kd_def;
                    temp22.z=ka_def + ndotl2 * kd_def;
                    temp33.x=ka_def + ndotl3 * kd_def;
                    temp33.y=ka_def + ndotl3 * kd_def;
                    temp33.z=ka_def + ndotl3 * kd_def;
                    char c[1];
                    int i2 =0;
                    int t2=0;
                    c[0]=getc(file8);
                    int brac=0;
                    int brac1=0;
                    do {
                        if(c[0] == '/'&& brac==1)
                        {brac1=1;
                        }
                        c[0]=fgetc(file8);
                        if(c[0] == '/')
                        {brac=1;
                            t2++;
                        }
                        else{brac=0;
                        }
                    } while(c[0] != ' ');
                    fseek(file8,-i2-1 ,SEEK_CUR);
                    switch(t2)
                    {
                        case 0:
                        fscanf(file8, "%d %d %d\n", &vertex_color[0], &vertex_color[1],&vertex_color[2] );//check what to do here
                        temp_color1.push_back(temp11.x);
                        temp_color1.push_back(temp11.y);
                        temp_color1.push_back(temp11.z);
                        temp_color1.push_back(temp22.x);
                        temp_color1.push_back(temp22.y);
                        temp_color1.push_back(temp22.z);
                        temp_color1.push_back(temp33.x);
                        temp_color1.push_back(temp33.y);
                        temp_color1.push_back(temp33.z );
                        break;
                        case 1://sphere
                        fscanf(file8, " %d/%d %d/%d %d/%d\n", &vertex_color[0], &uv_color[0], &vertex_color[1], &uv_color[1], &vertex_color[2], &uv_color[2] );
                        temp_color1.push_back(temp11.x);
                        temp_color1.push_back(temp11.y);
                        temp_color1.push_back(temp11.z);
                        temp_color1.push_back(temp22.x);
                        temp_color1.push_back(temp22.y);
                        temp_color1.push_back(temp22.z);
                        temp_color1.push_back(temp33.x);
                        temp_color1.push_back(temp33.y);
                        temp_color1.push_back(temp33.z );
                        break;
                        case 2://cube_single
                        fscanf(file8, " %d//%d %d//%d %d//%d", &vertex_color[0], &normal_color[0], &vertex_color[1], &normal_color[1],&vertex_color[2], &normal_color[2]);
                        temp_color1.push_back(temp11.x);
                        temp_color1.push_back(temp11.y);
                        temp_color1.push_back(temp11.z);
                        temp_color1.push_back(temp22.x);
                        temp_color1.push_back(temp22.y);
                        temp_color1.push_back(temp22.z);
                        temp_color1.push_back(temp33.x);
                        temp_color1.push_back(temp33.y);
                        temp_color1.push_back(temp33.z );
                        break;
                    }
                }
            }
        }
    }
    void display (void )
    {
        glClear( GL_COLOR_BUFFER_BIT );
        glDrawArrays(GL_TRIANGLES, 0, 2000000);
        glFlush();
    }
    int main(int argc, char* argv[])
    {
        glutInit( &argc, argv );
        glutInitDisplayMode( GLUT_RGBA );
        glutInitWindowSize( 512, 512 );
        glutInitContextVersion( 4, 3 );
        glutInitContextProfile( GLUT_CORE_PROFILE );// GLUT_COMPATIBILITY_PROFILE );
        glutCreateWindow( argv[0] );
        glewExperimental = GL_TRUE; // added for glew to work!
        file2 = fopen(file_name, "r");
        if( file2 == NULL ){
            printf("Can't open the file\n");
        }
        if ( glewInit() )
        {
            cerr << "Unable to initialize GLEW ... exiting" << endl;
            exit (EXIT_FAILURE );
        }
        const GLubyte *renderer = glGetString( GL_RENDERER );
        const GLubyte *vendor = glGetString( GL_VENDOR );
        const GLubyte *version = glGetString( GL_VERSION );
        const GLubyte *glslVersion = glGetString (GL_SHADING_LANGUAGE_VERSION );
        GLint major, minor;
        glGetIntegerv( GL_MAJOR_VERSION, &major );
        glGetIntegerv( GL_MINOR_VERSION, &minor );
        cout << "GL Vendor :" << vendor << endl;
        cout << "GL Renderer :" << renderer << endl;
        cout << "GL Version (string) :" << version << endl;
        cout << "GL Version (integer) :" << major << " " << minor << endl;
        cout << "GLSL Version :" << glslVersion << endl;
        glClear( GL_COLOR_BUFFER_BIT );
        read_controlfile(argv[2]);
        init();
        glutDisplayFunc( display );
        glutSpecialFunc(SpecialInput);
        glutKeyboardFunc(MyKeyboardFunc);//TO HANDLE KEY PRESS
        glutMainLoop();
        return 0;
    }
    std::vector< glm::vec3 > read_file()
    {
        //reading from file
        glm::vec3 vertex;
        std::vector< glm::vec3 > temp_vertices;
        std::vector< glm::vec3 > vertices1;
        std::vector< glm::int32 > vertexIndices;
        unsigned int vertexIndex[3];
        unsigned int uvIndex[3];
        unsigned int normalIndex[3];
        FILE * file = fopen(file_name, "r");
        if( file == NULL ){
            printf("Can't open the file\n");
            //return false;
        }
        int i = 0;
        while( 1 ){
            char lineHeader[128];
            int res = fscanf(file, "%s", lineHeader);
            if (res == EOF)
                break;
            if ( strcmp( lineHeader, "v" ) == 0 ){
                fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z );
                temp_vertices.push_back(vertex);
            }
            else if ( strcmp( lineHeader, "f" ) == 0 )
            {
                char pattern[1500];
                char c[1];
                int i =0;
                int t=0;
                int brac=0;
                int brac1=1;
                c[0]=getc(file);
                do
                { if(c[0] == '/'&& brac==1)
                    {brac1=1;
                    }
                    c[0]=fgetc(file);
                    if(c[0] == '/')
                    {brac=1;
                        t++;
                    }
                    else{brac=0;
                    }
                    i++;
                }while(c[0] != ' ');
                int len = strlen(pattern);
                fseek(file,-i-1,SEEK_CUR);
                switch(t)
                {
                    case 0:
                    fscanf(file, " %d %d %d", &vertexIndex[0], &vertexIndex[1],&vertexIndex[2]);
                    vertexIndices.push_back(vertexIndex[0]);
                    vertexIndices.push_back(vertexIndex[1]);
                    vertexIndices.push_back(vertexIndex[2]);
                    break;
                    case 1:
                    fscanf(file, " %d/%d %d/%d %d/%d\n", &vertexIndex[0], &uvIndex[0], &vertexIndex[1], &uvIndex[1], &vertexIndex[2], &uvIndex[2] );
                    vertexIndices.push_back(vertexIndex[0]);
                    vertexIndices.push_back(vertexIndex[1]);
                    vertexIndices.push_back(vertexIndex[2]);
                    break;
                    case 2:
                    fscanf(file, " %d//%d %d//%d %d//%d", &vertexIndex[0], &normalIndex[0], &vertexIndex[1], &normalIndex[1],&vertexIndex[2], &normalIndex[2]);
                    vertexIndices.push_back(vertexIndex[0]);
                    vertexIndices.push_back(vertexIndex[1]);
                    vertexIndices.push_back(vertexIndex[2]);
                    break;
                }
            }
            else
            {}
        }
        for( unsigned int i=0; i<vertexIndices.size(); i++ )
        {
            unsigned int vertexIndex = vertexIndices[i];
            glm::vec3 vertex = temp_vertices[ vertexIndex - 1];
            vertices1.push_back(vertex);
        }
        int a = vertices1.size();
        return vertices1;
    }
    int count_faces()
    {
        FILE * file10 = fopen(file_name, "r");
        if( file10 == NULL ){
            printf("Can't open the file\n");
        }
        int count_face=0;
        while( 1 ){
            char lineHeader[128];
            int res = fscanf(file10, "%s", lineHeader);
            if (res == EOF)
                break; // EOF = End Of File. Quit the loop.
            if ( strcmp( lineHeader, "f" ) == 0 ){
                count_face++;
            }}
            count_face=count_face*3;
            return count_face;
        }
        void MyKeyboardFunc(unsigned char Key, int x, int y)
        {
            switch(Key)
            {
                case 'c': cam3=cam3-0.1f;
                // look3=look3+0.5;
                View = glm::lookAt(
                glm::vec3(cam1,cam2,cam3), // Camera is at (4,3,3), in World Space
                glm::vec3(look1,look2,look3), // and looks at the origin
                glm::vec3(up1,up2,up3) // Head is up (set to 0,-1,0 to look upside-down)
                );
                MVP = Projection * View * Model;
                MatrixID = glGetUniformLocation(program, "MVP");
                glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
                display();
                break;
                case 'v': cam3=cam3+0.1f;
                View = glm::lookAt(
                glm::vec3(cam1,cam2,cam3), // Camera is at (4,3,3), in World Space
                glm::vec3(look1,look2,look3), // and looks at the origin
                glm::vec3(up1,up2,up3) // Head is up (set to 0,-1,0 to look upside-down)
                );
                MVP = Projection * View * Model;
                MatrixID = glGetUniformLocation(program, "MVP");
                glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
                display();
                break;
                case 'd': look3=look3+0.1f;
                View = glm::lookAt(
                glm::vec3(cam1,cam2,cam3), // Camera is at (4,3,3), in World Space
                glm::vec3(look1,look2,look3), // and looks at the origin
                glm::vec3(up1,up2,up3) // Head is up (set to 0,-1,0 to look upside-down)
                );
                MVP = Projection * View * Model;
                MatrixID = glGetUniformLocation(program, "MVP");
                glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
                display();
                break;
                case 'f': look3=look3-0.1f;
                
                View = glm::lookAt(
                glm::vec3(cam1,cam2,cam3), // Camera is at (4,3,3), in World Space
                glm::vec3(look1,look2,look3), // and looks at the origin
                glm::vec3(up1,up2,up3) // Head is up (set to 0,-1,0 to look upside-down)
                );
                MVP = Projection * View * Model;
                MatrixID = glGetUniformLocation(program, "MVP");
                glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
                display();
                break;
                case 'z':degree = 0.1f;
                View = glm:: rotate(View, -degree, glm::vec3(cam1, cam2, cam3));
                MVP = Projection * View * Model;
                MatrixID = glGetUniformLocation(program, "MVP");
                glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
                display();
                break;
                case 'x': degree = 0.1f;
                View = glm:: rotate(View, degree, glm::vec3(cam1, cam2, cam3));
                MVP = Projection * View * Model;
                MatrixID = glGetUniformLocation(program, "MVP");
                glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
                display();
                break;
                case 'r':
                look1 = 0.0f, look2 = 0.0f, look3 = 0.0f;
                up1 = 0.0f, up2 = 0.0f, up3 = 1.0f;
                near = 0.1f, far = 100.0f;
                degree = 0.0f;
                degree1 = cam2 / cam1;
                View = glm:: rotate(View, -degree, glm::vec3(cam1, cam2, cam3));
                View = glm::lookAt(
                glm::vec3(cam1,cam2,cam3), // Camera is at (4,3,3), in World Space
                glm::vec3(look1,look2,look3), // and looks at the origin
                glm::vec3(up1,up2,up3) // Head is up (set to 0,-1,0 to look upside-down)
                );
                Projection = glm::frustum( -0.1f, 0.1f, -0.1f, 0.1f, near, far );
                MVP = Projection * View * Model;
                MatrixID = glGetUniformLocation(program, "MVP");
                glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
                display();
                break;
                case 's':
                glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
                display();
                break;
                case 'w':
                glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
                display();
                break;
                case 'q':
                exit(1);
                break;
            };
        }
        void SpecialInput(int key, int x, int y)
        {
            switch(key)
            {
                float radius;
                case GLUT_KEY_LEFT: 
                radius = sqrt(pow((cam1-look1),2)+pow((cam2-look2),2));
                degree1 = degree1 + (3.14 / 180);
                cam1 = radius * cos(degree1);
                cam2 = radius * sin(degree1);
                View = glm::lookAt(
                glm::vec3(cam1,cam2,cam3), // Camera is at (4,3,3), in World Space
                glm::vec3(look1,look2,look3), // and looks at the origin
                glm::vec3(up1,up2,up3) // Head is up (set to 0,-1,0 to look upside-down)
                );
                MVP = Projection * View * Model;
                MatrixID = glGetUniformLocation(program, "MVP");
                glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
                display();
                break;
                case GLUT_KEY_UP: 
                near=near+0.1;
                Projection = glm::frustum( -0.1f, 0.1f, -0.1f, 0.1f, near, far );
                MVP = Projection * View * Model;
                MatrixID = glGetUniformLocation(program, "MVP");
                glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
                display();
                break;
                case GLUT_KEY_DOWN:
                near=near-0.1;
                Projection = glm::frustum( -0.1f, 0.1f, -0.1f, 0.1f, near, far );
                MVP = Projection * View * Model; 
                MatrixID = glGetUniformLocation(program, "MVP");
                glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
                display();
                break;
                case GLUT_KEY_RIGHT: 
                radius = sqrt(pow((cam1-look1),2)+pow((cam2-look2),2));
                degree1 = degree1 - (3.14 / 180);
                cam1 = radius * cos(degree1);
                cam2 = radius * sin(degree1);
                View = glm::lookAt(
                glm::vec3(cam1,cam2,cam3), // Camera is at (4,3,3), in World Space
                glm::vec3(look1,look2,look3), // and looks at the origin
                glm::vec3(up1,up2,up3) // Head is up (set to 0,-1,0 to look upside-down)
                );
                MVP = Projection * View * Model;
                MatrixID = glGetUniformLocation(program, "MVP");
                glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
                display();
                break;
            };
        }
        void read_controlfile(char *namefile)
        {
            glm::vec3 rrr;
            glClear( GL_COLOR_BUFFER_BIT );
            FILE * file_control = fopen(namefile, "r");
            char header_control[128];
            int count_obj=0;
            int start=0;
            int final=0;
            int no_vertices=0;
            glm::vec3 vert;
            while(1)
            {
                int res = fscanf(file_control, "%s",header_control);
                if (res == EOF)
                    break;
                if ( strcmp(header_control, "obj" ) == 0 )
                {
                    count_obj++;
                    if(count_obj==0)
                    {start=0;}
                    else
                    {start=final;}
                    int res5= fscanf(file_control, " %s ",file_name);
                    init1();
                    no_vertices= count_faces();
                    final=final+ no_vertices;
                }
                if(strcmp(header_control, "t")==0)
                {
                    glm::vec3 ttt;
                    int res4= fscanf(file_control, "%f %f %f ",&ttt.x, &ttt.y,&ttt.z);
                    if (res4 == EOF)
                    {break;} // EOF = End Of File. Quit the loop.
                    t1=ttt.x;
                    t2=ttt.y;
                    t3=ttt.z;
                    for(int ll=start; ll<final;ll++)
                    {vert= vertices[ll];
                        glm::mat4 Model =glm::translate(glm::mat4(1.0f), glm::vec3(t1, t2, t3));
                        vertices[ll]= glm::vec3(Model* (glm::vec4(vertices[ll], 1.0f)));
                    }//end of for
                }
                if(strcmp(header_control, "s")==0)
                {
                    glm::vec3 sss;
                    int res4= fscanf(file_control, "%f %f %f ",&sss.x, &sss.y,&sss.z);
                    if (res4 == EOF)
                    {break;} // EOF = End Of File. Quit the loop.
                    s1=sss.x;
                    s2=sss.y;
                    s3=sss.z;
                    for(int ll=start; ll<final;ll++)
                    {
                        vert= vertices[ll];
                        glm::mat4 Model =glm::scale(glm::mat4(1.0f), glm::vec3(s1, s2, s3));
                        vertices[ll]= glm::vec3(Model* (glm::vec4(vertices[ll], 1.0f)));
                    }
                }
                if(strcmp(header_control, "rx")==0)
                {
                    int res4= fscanf(file_control, "%f ",&rrr.x);
                    if (res4 == EOF)
                    {break;} // EOF = End Of File. Quit the loop.
                    glm:: mat4 Model = glm:: rotate(glm::mat4(1.0f), rrr.x, glm::vec3(1.0f, 0.0f, 0.0f));
                    for(int ll=start; ll<final;ll=ll+1)
                    {
                        vertices[ll]= glm::vec3(Model* (glm::vec4(vertices[ll], 1.0f)));
                    }
                }
                if(strcmp(header_control, "ry")==0)
                {
                    int res4= fscanf(file_control, "%f ",&rrr.y);
                    if (res4 == EOF)
                    {break;} // EOF = End Of File. Quit the loop.
                    glm:: mat4 Model = glm:: rotate(glm::mat4(1.0f), rrr.y, glm::vec3(0.0f, 1.0f, 0.0f));
                    for(int ll=start; ll<final;ll=ll+1)
                    {
                        vertices[ll]= glm::vec3(Model* (glm::vec4(vertices[ll], 1.0f)));
                    }
                }
                if(strcmp(header_control, "rz")==0)
                {
                    int res4= fscanf(file_control, "%f ",&rrr.z);
                    if (res4 == EOF)
                    {break;} // EOF = End Of File. Quit the loop.
                    glm:: mat4 Model = glm:: rotate(glm::mat4(1.0f), rrr.z, glm::vec3(0.0f, 0.0f, 1.0f));
                    for(int ll=start; ll<final;ll=ll+1)
                    {
                        vertices[ll]= glm::vec3(Model* (glm::vec4(vertices[ll], 1.0f)));
                    }
                }
            }//end while
        }
        void init_display()
        {
            init();
            display();
        }
        void display_bind()
        {
            glGenBuffers(1, &colorbuffer);
            glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
            glBufferData(GL_ARRAY_BUFFER, temp_color1.size()*sizeof(glm::vec3), &temp_color1[0], GL_STATIC_DRAW);
            //check size
            glEnableVertexAttribArray(1);
            glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
            glVertexAttribPointer(
            1, // attribute. No particular reason for 1, but must match the layout in the shader.
            3, // size
            GL_FLOAT, // type
            GL_FALSE, // normalized?
            0, // stride
            (void*)0 // array buffer offset
            );
            glGenBuffers( NumBuffers, Buffers );
            glBindBuffer( GL_ARRAY_BUFFER, Buffers[ArrayBuffer] );
            glBufferData( GL_ARRAY_BUFFER, vertices.size()*sizeof(glm::vec3), &vertices[0], GL_STATIC_DRAW );
            ShaderInfo shaders[] = {
                { GL_VERTEX_SHADER, "triangles.vert" },
                { GL_FRAGMENT_SHADER, "triangles.frag" },
                { GL_NONE, NULL }
            };
            program = LoadShaders( shaders );
            glUseProgram( program );
            GLuint MatrixID = glGetUniformLocation(program, "MVP");
            glUniformMatrix4fv(MatrixID, 1, GL_FALSE, &MVP[0][0]);
            glVertexAttribPointer( vPosition, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));
            glEnableVertexAttribArray( 0 );
            glClear( GL_COLOR_BUFFER_BIT );
            glDrawArrays(GL_TRIANGLES, 0, 2000000);
            glFlush();
        }
