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
 *  Renders two white “triangles” (using a flattened pyramid mesh)
 *  as perfect mirror images across the X-axis (horizontal midline).
 *
 *  Key idea:
 *    • Rotation (180° around Z) flips orientation but does NOT create a mirror.
 *    • A mirror is a reflection — achieved here by negating the Y scale on the
 *      second draw call (bottom triangle).
 *
 *  Notes:
 *    • We scale Z to ~0 so perspective won’t distort the apparent shape.
 *    • If back-face culling is enabled, a negative scale flips winding.
 *      If one triangle disappears, either disable culling or add a 180° Z
 *      rotation to re-normalize the winding for that instance.
 ***********************************************************/
void SceneManager::RenderScene()
{
    // ----- Layout controls (tweakables) --------------------------------------
    const float  scaleXY = 2.40f;   // uniform X/Y size of each triangle
    const float  centerShiftY = 0.90f;   // move both triangles up/down together
    const float  tipGapFactor = 0.50f;   // <0.50 keeps tips touching w/o overlap
    // -------------------------------------------------------------------------

    // Make the mesh effectively 2D so perspective doesn’t skew sizes
    const glm::vec3 kScale2D(scaleXY, scaleXY, 0.001f);
    const float     kYOffset = tipGapFactor * scaleXY;

    // Draw color: solid white, no texture/lighting
    SetShaderColor(1.0f, 1.0f, 1.0f, 1.0f);

    // -------- TOP triangle (upright) -----------------------------------------
    // Standard 2D scale; positioned so its tip meets the bottom triangle’s tip.
    SetTransformations(
        /*scale*/    kScale2D,
        /*rotX*/     0.0f,
        /*rotY*/     0.0f,
        /*rotZ*/     0.0f,
        /*position*/ glm::vec3(0.0f, -kYOffset + centerShiftY, 0.0f)
    );
    m_basicMeshes->DrawPyramid4Mesh();

    // -------- BOTTOM triangle (true mirror) ----------------------------------
    // Reflection across X-axis: negate Y scale (mirror), keep rotations at 0.
    // If GL_CULL_FACE is ON and this instance vanishes, either:
    //   1) glDisable(GL_CULL_FACE) before drawing, or
    //   2) add rotZ = 180.f here to restore front-face winding.
    SetTransformations(
        /*scale*/    glm::vec3(kScale2D.x, -kScale2D.y, kScale2D.z), // mirror on Y
        /*rotX*/     0.0f,
        /*rotY*/     0.0f,
        /*rotZ*/     0.0f,  // set to 180.f if you must fix winding with culling on
        /*position*/ glm::vec3(0.0f, +kYOffset + centerShiftY, 0.0f)
    );
    m_basicMeshes->DrawPyramid4Mesh();
}
