// ===================================================================
//                             FOR TASK 1
// ===================================================================

// ==================================================
//  Include additional headers
// ==================================================

#include "asstcommon.h"
#include "scenegraph.h"
#include "drawer.h"
#include "picker.h"

// ==================================================================
// ShaderState, and sendModelViewNormalMatrix have been moved to
// asstcommon.h, so remove them from your asst4.cpp. Also remove the
// 'static' modifier from g_Gl2Compatible to make this variable
// accessible to other compilation units.
// =================================================================

// remove: static const bool g_Gl2Compatible = false;
const bool g_Gl2Compatible = false;

// =================================================================
// Reference the newly provided picker shaders by adding/modifying
// the following:
// =================================================================

static const int PICKING_SHADER = 2; // index of the picking shader is g_shaerFiles
static const int g_numShaders = 3; // 3 shaders instead of 2
static const char * const g_shaderFiles[g_numShaders][2] = {
  ...
  {"./shaders/basic-gl3.vshader", "./shaders/pick-gl3.fshader"}
};
static const char * const g_shaderFilesGl2[g_numShaders][2] = {
  ...
  {"./shaders/basic-gl2.vshader", "./shaders/pick-gl2.fshader"}
};

// =================================================================
// After the definition of Geometry, add a typedef that declares
// our own Shape node which draws using our Geometry.
// =================================================================
class Geometry {
  ...
};

typedef SgGeometryShapeNode<Geometry> MyShapeNode;


// ===================================================================
// Declare the scene graph and pointers to suitable nodes in the scene
// graph
// ===================================================================

static shared_ptr<SgRootNode> g_world;
static shared_ptr<SgRbtNode> g_skyNode, g_groundNode, g_robot1Node, g_robot2Node;
static shared_ptr<SgRbtNode> g_currentPickedRbtNode; // used later when you do picking

// =================================================================
// Insert the following constructRobot and initScene() after the
// initGeometry() function, and call initScene() after initGeometry()
// in your main()
// =================================================================

static void constructRobot(shared_ptr<SgTransformNode> base, const Cvec3& color) {

  const double ARM_LEN = 0.7,
               ARM_THICK = 0.25,
               TORSO_LEN = 1.5,
               TORSO_THICK = 0.25,
               TORSO_WIDTH = 1;
  const int NUM_JOINTS = 3,
            NUM_SHAPES = 3;

  struct JointDesc {
    int parent;
    float x, y, z;
  };

  JointDesc jointDesc[NUM_JOINTS] = {
    {-1}, // torso
    {0,  TORSO_WIDTH/2, TORSO_LEN/2, 0}, // upper right arm
    {1,  ARM_LEN, 0, 0}, // lower right arm
  };

  struct ShapeDesc {
    int parentJointId;
    float x, y, z, sx, sy, sz;
    shared_ptr<Geometry> geometry;
  };

  ShapeDesc shapeDesc[NUM_SHAPES] = {
    {0, 0,         0, 0, TORSO_WIDTH, TORSO_LEN, TORSO_THICK, g_cube}, // torso
    {1, ARM_LEN/2, 0, 0, ARM_LEN, ARM_THICK, ARM_THICK, g_cube}, // upper right arm
    {2, ARM_LEN/2, 0, 0, ARM_LEN, ARM_THICK, ARM_THICK, g_cube}, // lower right arm
  };

  shared_ptr<SgTransformNode> jointNodes[NUM_JOINTS];

  for (int i = 0; i < NUM_JOINTS; ++i) {
    if (jointDesc[i].parent == -1)
      jointNodes[i] = base;
    else {
      jointNodes[i].reset(new SgRbtNode(RigTForm(Cvec3(jointDesc[i].x, jointDesc[i].y, jointDesc[i].z))));
      jointNodes[jointDesc[i].parent]->addChild(jointNodes[i]);
    }
  }
  for (int i = 0; i < NUM_SHAPES; ++i) {
    shared_ptr<MyShapeNode> shape(
      new MyShapeNode(shapeDesc[i].geometry,
                      color,
                      Cvec3(shapeDesc[i].x, shapeDesc[i].y, shapeDesc[i].z),
                      Cvec3(0, 0, 0),
                      Cvec3(shapeDesc[i].sx, shapeDesc[i].sy, shapeDesc[i].sz)));
    jointNodes[shapeDesc[i].parentJointId]->addChild(shape);
  }
}

static void initScene() {
  g_world.reset(new SgRootNode());

  g_skyNode.reset(new SgRbtNode(RigTForm(Cvec3(0.0, 0.25, 4.0))));

  g_groundNode.reset(new SgRbtNode());
  g_groundNode->addChild(shared_ptr<MyShapeNode>(
                           new MyShapeNode(g_ground, Cvec3(0.1, 0.95, 0.1))));

  g_robot1Node.reset(new SgRbtNode(RigTForm(Cvec3(-2, 1, 0))));
  g_robot2Node.reset(new SgRbtNode(RigTForm(Cvec3(2, 1, 0))));

  constructRobot(g_robot1Node, Cvec3(1, 0, 0)); // a Red robot
  constructRobot(g_robot2Node, Cvec3(0, 0, 1)); // a Blue robot

  g_world->addChild(g_skyNode);
  g_world->addChild(g_groundNode);
  g_world->addChild(g_robot1Node);
  g_world->addChild(g_robot2Node);
}

// =================================================================
// Use the scene graph to draw the scene by doing the following:
//
// Modify the drawStuff call to take in a 'curSS' (current shader
// state) and 'picking' as arguments. 'picking' will always be false
// for now. This is to make your later job of implementing picking
// easier.
// =================================================================

static void drawStuff(const ShaderState& curSS, bool picking) // { ...

// =================================================================
// Inside drawStuff, remove the line:
//
//    const ShaderState& curSS = *g_shaderStates[g_activeShader];
//
// So now whenever curSS is referred inside drawStuff, the passed
// in argument is used.
//
// Replace the code for drawing the ground the two cubes with:
// =================================================================

if (!picking) {
  Drawer drawer(invEyeRbt, curSS);
  g_world->accept(drawer);

  // draw arcball as part of asst3
  ...
}
else {
  Picker picker(invEyeRbt, curSS);
  g_world->accept(picker);
  glFlush();
  g_currentPickedRbtNode = picker.getRbtNodeAtXY(g_mouseClickX, g_mouseClickY);
  if (g_currentPickedRbtNode == g_groundNode)
    g_currentPickedRbtNode = shared_ptr<SgRbtNode>();   // set to NULL
}

// =================================================================
// In display(), replace the drawStuff call with:
// =================================================================

drawStuff(*g_shaderStates[g_activeShader], false);





// ===================================================================
//                             FOR TASK2:
// ===================================================================

// ===================================================================
// Once you have finished most of task 2, you can use the
// following function to perform picking. it will set
// g_currentPickedRbtNode according to g_mouseClickX and g_mouseClickY
// ===================================================================

static void pick() {
  // We need to set the clear color to black, for pick rendering.
  // so let's save the clear color
  GLdouble clearColor[4];
  glGetDoublev(GL_COLOR_CLEAR_VALUE, clearColor);

  glClearColor(0, 0, 0, 0);

  // using PICKING_SHADER as the shader
  glUseProgram(g_shaderStates[PICKING_SHADER]->program);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  drawStuff(*g_shaderStates[PICKING_SHADER], true);

  // Uncomment below and comment out the glutPostRedisplay in mouse(...) call back
  // to see result of the pick rendering pass
  // glutSwapBuffers();

  //Now set back the clear color
  glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

  checkGlErrors();
}


