/****************************************************************************************

Copyright (C) 2015 Autodesk, Inc.
All rights reserved.

Use of this software is subject to the terms of the Autodesk license agreement
provided at the time of installation or download, or which otherwise accompanies
this software in either electronic or hard copy form.

****************************************************************************************/

/////////////////////////////////////////////////////////////////////////
//
// This example illustrates how to display the content of a FBX or a OBJ file
// in a graphical window. This program is based on the OpenGL Utility Toolkit 
// (GLUT). Start the program on the command line by providing a FBX or a 
// OBJ file name. A menu is provided to select the current camera and the current 
// animation stack.
//
// Among other things, the example illustrates how to:
// 1)  Use a custom memory allocator
// 2)  Import a scene from a .FBX, .DAE or .OBJ file;
// 3)  Convert the nurbs and patch attribute types of a scene into mesh 
//     node attributes; And trianglate all meshes.
// 4)  Get the list of all the cameras in the scene;
// 5)  Find the current camera;
// 6)  Get the relevant settings of a camera depending on it's projection
//     type and aperture mode;
// 7)  Compute the local and global positions of a node;
// 8)  Compute the orientation of a camera;
// 9)  Compute the orientation of a light;
// 10)  Compute the shape deformation of mesh vertices;
// 11) Compute the link deformation of mesh vertices.
// 12) Display the point cache simulation of a mesh.
// 13) Get the list of all pose in the scene;
// 14) Show the scene using at a specific pose.
//
/////////////////////////////////////////////////////////////////////////

#include "SceneContext.h"
#include <GL4D/gl4du.h>
#include <GL4D/gl4dg.h>
#include <GL4D/gl4duw_SDL2.h>

static void init(void);
static void resize(int w, int h);
void ExitFunction();
void TimerCallback(int);
void DisplayCallback();
void ReshapeCallback(int pWidth, int pHeight);
void KeyboardCallback(unsigned char pKey, int, int);
void MouseCallback(int button, int state, int x, int y);
void MotionCallback(int x, int y);

SceneContext * gSceneContext;

// Menu item ids.
#define PRODUCER_PERSPECTIVE_ITEM   100
#define PRODUCER_TOP_ITEM           101
#define PRODUCER_BOTTOM_ITEM        102
#define PRODUCER_FRONT_ITEM         103
#define PRODUCER_BACK_ITEM          104
#define PRODUCER_RIGHT_ITEM         105
#define PRODUCER_LEFT_ITEM          106
#define CAMERA_SWITCHER_ITEM        107
#define PLAY_ANIMATION              200


const int MENU_SHADING_MODE_WIREFRAME = 300;
const int MENU_SHADING_MODE_SHADED = 301;
const char * MENU_STRING_SHADING_MODE_WIREFRAME = "Wireframe";
const char * MENU_STRING_SHADING_MODE_SHADED = "Shaded";

const int MENU_ZOOM_FOCAL_LENGTH =          401;
const int MENU_ZOOM_POSITION     =          402;

const int MENU_EXIT = 400;

const int DEFAULT_WINDOW_WIDTH = 720;
const int DEFAULT_WINDOW_HEIGHT = 486;

GLuint _pId = 0;
GLuint _quad = 0;

class MyMemoryAllocator
{
public:
	static void* MyMalloc(size_t pSize)
    {
        char *p = (char*)malloc(pSize + FBXSDK_MEMORY_ALIGNMENT);
		memset(p, '#', FBXSDK_MEMORY_ALIGNMENT);
        return p + FBXSDK_MEMORY_ALIGNMENT;
    }

	static void* MyCalloc(size_t pCount, size_t pSize)
    {
        char *p = (char*)calloc(pCount, pSize + FBXSDK_MEMORY_ALIGNMENT);
		memset(p, '#', FBXSDK_MEMORY_ALIGNMENT);
        return p + FBXSDK_MEMORY_ALIGNMENT;
    }

	static void* MyRealloc(void* pData, size_t pSize)
    {
        if (pData)
        {
            FBX_ASSERT(*((char*)pData-1)=='#');
            if (*((char*)pData-1)=='#')
            {
                char *p = (char*)realloc((char*)pData - FBXSDK_MEMORY_ALIGNMENT, pSize + FBXSDK_MEMORY_ALIGNMENT);
				memset(p, '#', FBXSDK_MEMORY_ALIGNMENT);
                return p + FBXSDK_MEMORY_ALIGNMENT;
            }
            else
            {   // Mismatch
                char *p = (char*)realloc((char*)pData, pSize + FBXSDK_MEMORY_ALIGNMENT);
				memset(p, '#', FBXSDK_MEMORY_ALIGNMENT);
                return p + FBXSDK_MEMORY_ALIGNMENT;
            }
        }
        else
        {
            char *p = (char*)realloc(NULL, pSize + FBXSDK_MEMORY_ALIGNMENT);
			memset(p, '#', FBXSDK_MEMORY_ALIGNMENT);
            return p + FBXSDK_MEMORY_ALIGNMENT;
        }
    }

	static void MyFree(void* pData)
    {
        if (pData==NULL)
            return;
        FBX_ASSERT(*((char*)pData-1)=='#');
        if (*((char*)pData-1)=='#')
        {
            free((char*)pData - FBXSDK_MEMORY_ALIGNMENT);
        }
        else
        {   // Mismatch
            free(pData);
        }
    }
};

static bool gAutoQuit = false;

int main(int argc, char** argv)
{
    // Set exit function to destroy objects created by the FBX SDK.
    atexit(ExitFunction);

	// Use a custom memory allocator
    FbxSetMallocHandler(MyMemoryAllocator::MyMalloc);
    FbxSetReallocHandler(MyMemoryAllocator::MyRealloc);
    FbxSetFreeHandler(MyMemoryAllocator::MyFree);
    FbxSetCallocHandler(MyMemoryAllocator::MyCalloc);

	// glut initialisation
    // glutInit(&argc, argv);
    // glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
    // glutInitWindowSize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT); 
    // glutInitWindowPosition(100, 100);
    // glutCreateWindow("ViewScene");

    if (!gl4duwCreateWindow(argc, argv, "GL4Dummies", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                            DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE | SDL_WINDOW_SHOWN))
        return 1;

    // Initialize OpenGL.
    const bool lSupportVBO = InitializeOpenGL();
    init();

	// set glut callbacks 
    // glutDisplayFunc(DisplayCallback); 
    // glutReshapeFunc(ReshapeCallback);
    // glutKeyboardFunc(KeyboardCallback);
    // glutMouseFunc(MouseCallback);
    // glutMotionFunc(MotionCallback);
    gl4duwResizeFunc(ReshapeCallback);
    gl4duwDisplayFunc(DisplayCallback);

	FbxString lFilePath("");
	for( int i = 1, c = argc; i < c; ++i )
	{
		if( FbxString(argv[i]) == "-test" ) gAutoQuit = true;
		else if( lFilePath.IsEmpty() ) lFilePath = argv[i];
	}

	gSceneContext = new SceneContext(!lFilePath.IsEmpty() ? lFilePath.Buffer() : NULL, DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT, lSupportVBO);

	// glutMainLoop();
    gl4duwMainLoop();

    return 0;
}


static void init(void)
{
    _pId = gl4duCreateProgram("<vs>../shaders/basic.vs", "<fs>../shaders/basic.fs", NULL);
    glClearColor(1.0f, 0.2f, 0.2f, 0.0f);

    gl4duGenMatrix(GL_FLOAT, "modelViewMatrix");
    gl4duGenMatrix(GL_FLOAT, "projectionMatrix");
    gl4duBindMatrix("projectionMatrix");
    gl4duLoadIdentityf();
    gl4duFrustumf(-0.5, 0.5, -0.5, 0.5, 1, 1000);
    gl4duBindMatrix("modelViewMatrix");

    _quad = gl4dgGenQuadf();

    resize(DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT);
}


static void resize(int w, int h)
{
    glViewport(0, 0, w, h);
}


// Function to destroy objects created by the FBX SDK.
void ExitFunction()
{
    delete gSceneContext;
}


// Trigger the display of the current frame.
void TimerCallback(int)
{
    // Ask to display the current frame only if necessary.
    if (gSceneContext->GetStatus() == SceneContext::MUST_BE_REFRESHED)
    {
        // glutPostRedisplay();
    }

    gSceneContext->OnTimerClick();

    // Call the timer to display the next frame.
    // glutTimerFunc((unsigned int)gSceneContext->GetFrameTime().GetMilliSeconds(), TimerCallback, 0);
}


// Refresh the application window.
void DisplayCallback()
{
    gSceneContext->OnDisplay();

    // glutSwapBuffers();

    // Import the scene if it's ready to load.
    if (gSceneContext->GetStatus() == SceneContext::MUST_BE_LOADED)
    {

        // This function is only called in the first display callback
        // to make sure that the application window is opened and a 
        // status message is displayed before.
        gSceneContext->LoadFile();
        // Call the timer to display the first frame.
        // glutTimerFunc((unsigned int)gSceneContext->GetFrameTime().GetMilliSeconds(), TimerCallback, 0);
    }

	if( gAutoQuit ) exit(0);
}


// Resize the application window.
void ReshapeCallback(int pWidth, int pHeight)
{
    gSceneContext->OnReshape(pWidth, pHeight);
}

// Exit the application from the keyboard.
void KeyboardCallback(unsigned char pKey, int /*pX*/, int /*pY*/)
{
    // Exit on ESC key.
    if (pKey == 27)
    {
        exit(0);
    }

    gSceneContext->OnKeyboard(pKey);
}

void MouseCallback(int button, int state, int x, int y)
{
    gSceneContext->OnMouse(button, state, x, y);
}


void MotionCallback(int x, int y)
{
    gSceneContext->OnMouseMotion(x, y);
}




