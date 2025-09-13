///////////////////////////////////////////////////////////////////////////////
// shadermanager.cpp
// ============
// manage the loading and rendering of 3D scenes
//
//  AUTHOR: Brian Battersby - SNHU Instructor / Computer Science
//  Created for CS-330-Computational Graphics and Visualization, Nov. 1st, 2023
///////////////////////////////////////////////////////////////////////////////

#include "SceneManager.h"

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#endif

#include <glm/gtx/transform.hpp>

// declaration of global variables and defines
namespace
{
    const char* g_ModelName = "model";
    const char* g_ColorValueName = "objectColor";
    const char* g_TextureValueName = "objectTexture";
    const char* g_UseTextureName = "bUseTexture";
    const char* g_UseLightingName = "bUseLighting";
}

/***********************************************************
 *  SceneManager()
 *
 *  The constructor for the class
 ***********************************************************/
SceneManager::SceneManager(ShaderManager* pShaderManager)
{
    m_pShaderManager = pShaderManager;
    m_basicMeshes = new ShapeMeshes();
}

/***********************************************************
 *  ~SceneManager()
 *
 *  The destructor for the class
 ***********************************************************/
SceneManager::~SceneManager()
{
    m_pShaderManager = NULL;
    delete m_basicMeshes;
    m_basicMeshes = NULL;
}

/***********************************************************
 *  SetTransformations()
 *
 *  This method is used for setting the transform buffer
 *  using the passed in transformation values.
 ***********************************************************/
void SceneManager::SetTransformations(
    glm::vec3 scaleXYZ,
    float XrotationDegrees,
    float YrotationDegrees,
    float ZrotationDegrees,
    glm::vec3 positionXYZ)
{
    // variables for this method
    glm::mat4 modelView;
    glm::mat4 scale;
    glm::mat4 rotationX;
    glm::mat4 rotationY;
    glm::mat4 rotationZ;
    glm::mat4 translation;

    // set the scale value in the transform buffer
    scale = glm::scale(scaleXYZ);
    // set the rotation values in the transform buffer
    rotationX = glm::rotate(glm::radians(XrotationDegrees), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationY = glm::rotate(glm::radians(YrotationDegrees), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationZ = glm::rotate(glm::radians(ZrotationDegrees), glm::vec3(0.0f, 0.0f, 1.0f));
    // set the translation value in the transform buffer
    translation = glm::translate(positionXYZ);

    // matrix math to calculate the final model matrix
    modelView = translation * rotationX * rotationY * rotationZ * scale;

    if (NULL != m_pShaderManager)
    {
        // pass the model matrix into the shader
        m_pShaderManager->setMat4Value(g_ModelName, modelView);
    }
}

/***********************************************************
 *  SetShaderColor()
 *
 *  This method is used for setting the passed in color
 *  into the shader for the next draw command
 ***********************************************************/
void SceneManager::SetShaderColor(
    float redColorValue,
    float greenColorValue,
    float blueColorValue,
    float alphaValue)
{
    // variables for this method
    glm::vec4 currentColor;

    currentColor.r = redColorValue;
    currentColor.g = greenColorValue;
    currentColor.b = blueColorValue;
    currentColor.a = alphaValue;

    if (NULL != m_pShaderManager)
    {
        // set the color values into the shader
        m_pShaderManager->setIntValue(g_UseTextureName, false);
        m_pShaderManager->setVec4Value(g_ColorValueName, currentColor);
    }
}

/**************************************************************/
/*** STUDENTS CAN MODIFY the code in the methods BELOW for  ***/
/*** preparing and rendering their own 3D replicated scenes.***/
/*** Please refer to the code in the OpenGL sample project  ***/
/*** for assistance.                                        ***/
/**************************************************************/

/***********************************************************
 *  PrepareScene()
 *
 *  This method is used for preparing the 3D scene by loading
 *  the shapes, textures in memory to support the 3D scene
 *  rendering
 ***********************************************************/
void SceneManager::PrepareScene()
{
    // only one instance of a particular mesh needs to be
    // loaded in memory no matter how many times it is drawn
    // in the rendered 3D scene
    m_basicMeshes->LoadPyramid4Mesh();
}

/***********************************************************
 *  RenderScene()
 *
 *  This method is used for rendering the 3D scene by
 *  transforming and drawing the basic 3D shapes
 ***********************************************************/
void SceneManager::RenderScene()
{
    // ---------- controls ----------
    const float  scaleXY = 2.40f;   // overall triangle size (uniform X/Y)
    const float  centerShiftY = 0.00f;   // move both triangles up/down together
    // ------------------------------

    // Make the pyramid effectively 2D so perspective can't skew sizes
    const glm::vec3 kScale = glm::vec3(scaleXY, scaleXY, 0.001f); // <- almost zero depth
    const float     kYOffset = 0.48f * scaleXY;   // tips meet cleanly without overlap
    const float     kFlipZDeg = 180.0f;            // bottom triangle points down

    // solid white, no texture
    SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f);

    // -------- TOP triangle (upright) --------
    SetTransformations(
        /*scale*/    kScale,
        /*rotX*/     0.0f,
        /*rotY*/     0.0f,
        /*rotZ*/     0.0f,
        /*position*/ glm::vec3(0.0f, -kYOffset + centerShiftY, 0.0f)
    );
    m_basicMeshes->DrawPyramid4Mesh();

    // -------- BOTTOM triangle (flipped 180° around Z) --------
    SetTransformations(
        /*scale*/    kScale,
        /*rotX*/     0.0f,
        /*rotY*/     0.0f,
        /*rotZ*/     kFlipZDeg,
        /*position*/ glm::vec3(0.0f, +kYOffset + centerShiftY, 0.0f)
    );
    m_basicMeshes->DrawPyramid4Mesh();
}





