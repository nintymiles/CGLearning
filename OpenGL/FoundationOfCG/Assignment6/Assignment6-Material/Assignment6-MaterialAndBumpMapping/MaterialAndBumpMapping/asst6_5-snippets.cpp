//==================================================================
// STEP 0: Include the newly provided "geometry.h"
//==================================================================
#include "geometry.h"

//==================================================================
// STEP 1: Replace the "------- Shaders" section with the following.
//         In particular you need to delete g_numShaders, g_shaderFiles,
//         g_shaderFilesGl2, g_shaderStates, and so on
//==================================================================

// --------- Materials
// This should replace all the contents in the Shaders section, e.g., g_numShaders, g_shaderFiles, and so on
static shared_ptr<Material> g_redDiffuseMat,
                            g_blueDiffuseMat,
                            g_bumpFloorMat,
                            g_arcballMat,
                            g_pickingMat,
                            g_lightMat;

shared_ptr<Material> g_overridingMaterial;

//==================================================================
// STEP 2: Replace the "------- Geometry" section with the following:
// 
//         In particular you need to delete the FIELD_OFFSET macro, 
//         struct VertexPN, and struct Geometry since they are now
//         superseded by the provided geometry header.
//
//         Then change the typedef of MyShapeNode to
//         SgGeometryShapeNode instead of
//         SgGeometryShapeNode<Geometry> since SgGeometryShapeNode
//         in scenegraph.h no longer takes a template type now
//         that we have a predefined Geometry type
//=================================================================

// --------- Geometry
typedef SgGeometryShapeNode MyShapeNode;


//======================================================================
// STEP 3: Replace initGround(), initCube(), and  initSphere() functions
//         with the following defintion. This ensures VertexPNTBX and
//         SimpleIndexedGeometryPNTBX are used, which provides extra
//         vertex attributes used for Bump Mapping later
//=======================================================================
static void initGround() {
  int ibLen, vbLen;
  getPlaneVbIbLen(vbLen, ibLen);

  // Temporary storage for cube Geometry
  vector<VertexPNTBX> vtx(vbLen);
  vector<unsigned short> idx(ibLen);

  makePlane(g_groundSize*2, vtx.begin(), idx.begin());
  g_ground.reset(new SimpleIndexedGeometryPNTBX(&vtx[0], &idx[0], vbLen, ibLen));
}

static void initCubes() {
  int ibLen, vbLen;
  getCubeVbIbLen(vbLen, ibLen);

  // Temporary storage for cube Geometry
  vector<VertexPNTBX> vtx(vbLen);
  vector<unsigned short> idx(ibLen);

  makeCube(1, vtx.begin(), idx.begin());
  g_cube.reset(new SimpleIndexedGeometryPNTBX(&vtx[0], &idx[0], vbLen, ibLen));
}

static void initSphere() {
  int ibLen, vbLen;
  getSphereVbIbLen(20, 10, vbLen, ibLen);

  // Temporary storage for sphere Geometry
  vector<VertexPNTBX> vtx(vbLen);
  vector<unsigned short> idx(ibLen);
  makeSphere(1, 20, 10, vtx.begin(), idx.begin());
  g_sphere.reset(new SimpleIndexedGeometryPNTBX(&vtx[0], &idx[0], vtx.size(), idx.size()));
}

//======================================================================
// STEP 4: Changes the definition of sendProjectionMatrix to use Uniforms
//         instead of ShaderState
//=======================================================================

// takes a projection matrix and send to the the shaders
inline void sendProjectionMatrix(Uniforms& uniforms, const Matrix4& projMatrix) {
  uniforms.put("uProjMatrix", projMatrix);
}

//=========================================================================
// STEP 5: Simplify your arcball drawing and drawStuff() by using materials
//         In particular, all occurence of
//             const ShaderState& curSS
//         should probably be replaced with
//             Uniforms& uniforms
//         and occurences of `curSS' be replaced with `uniforms'
//=========================================================================

//----------------------
// For arcball drawing:
//----------------------

// calculate arcball MVM as usual and store, say in, MVM
...  

// Use uniforms as opposed to curSS
sendModelViewNormalMatrix(uniforms, MVM, normalMatrix(MVM));

// No more glPolygonMode calls

g_arcballMat->draw(*g_sphere, uniforms);

// No more glPolygonMode calls



//-------------------------
// For drawStuff(...)
//-------------------------

// drawStuff just takes in a picking flag, and no curSS
static void drawStuff(bool picking) {

  // Declare an empty uniforms
  Uniforms uniforms;

  // Get your projection matrix into proj mat as usual
  ...

  // send proj. matrix to be stored by uniforms,
  // as opposed to the current vtx shader
  sendProjectionMatrix(uniforms, projmat);


  // get your eyeRbt, invEyeRbt and stuff as usual
  ...

  // get the eye space coordinates of the two light as usual
  // suppose they are stored as Cvec3 eyeLight1 and eyeLight2      
  ... 

  // send the eye space coordinates of lights to uniforms
  uniforms.put("uLight", eyeLight1);
  uniforms.put("uLight2", eyeLight2);

  if (!picking) {
    // initialize the drawer with our uniforms, as opposed to curSS
    Drawer drawer(invEyeRbt, uniforms);

    // draw as before
    g_world->accept(drawer);

    ... // draw arcball if neccessary
  }
  else {
    ...
    // intialize the picker with our uniforms, as opposed to curSS
    Picker picker(invEyeRbt, uniforms);

    // set overiding material to our picking material
    g_overridingMaterial = g_pickingMat;

    g_world->accept(picker);

    // unset the overriding material
    g_overridingMaterial.reset();

    ... // Do glFlush() and picker.getRbtNodeAtXY() and so on as usuall
  }
}


//=========================================================================
// STEP 6: display() and pick() should now use the simplified drawStuff()
//         as follows
//=========================================================================


static void display() {
  // No more glUseProgram
    
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  drawStuff(false); // no more curSS

  glutSwapBuffers();

  checkGlErrors();
}

static void pick() {
  // We need to set the clear color to black, for pick rendering.
  // so let's save the clear color
  GLdouble clearColor[4];
  glGetDoublev(GL_COLOR_CLEAR_VALUE, clearColor);

  glClearColor(0, 0, 0, 0);

  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // No more glUseProgram
  drawStuff(true); // no more curSS

  // Uncomment below and comment out the glutPostRedisplay in mouse(...) call back
  // to see result of the pick rendering pass
  // glutSwapBuffers();

  //Now set back the clear color
  glClearColor(clearColor[0], clearColor[1], clearColor[2], clearColor[3]);

  checkGlErrors();
}

//=========================================================================
// STEP 7: Remove the initShaders() function and replace it with the
//         following initMaterials(), and call it in main()
//=========================================================================
static void initMaterials() {
  // Create some prototype materials
  Material diffuse("./shaders/basic-gl3.vshader", "./shaders/diffuse-gl3.fshader");
  Material solid("./shaders/basic-gl3.vshader", "./shaders/solid-gl3.fshader");

  // copy diffuse prototype and set red color
  g_redDiffuseMat.reset(new Material(diffuse));
  g_redDiffuseMat->getUniforms().put("uColor", Cvec3f(1, 0, 0));

  // copy diffuse prototype and set blue color
  g_blueDiffuseMat.reset(new Material(diffuse));
  g_blueDiffuseMat->getUniforms().put("uColor", Cvec3f(0, 0, 1));

  // normal mapping material
  g_bumpFloorMat.reset(new Material("./shaders/normal-gl3.vshader", "./shaders/normal-gl3.fshader"));
  g_bumpFloorMat->getUniforms().put("uTexColor", shared_ptr<ImageTexture>(new ImageTexture("Fieldstone.ppm", true)));
  g_bumpFloorMat->getUniforms().put("uTexNormal", shared_ptr<ImageTexture>(new ImageTexture("FieldstoneNormal.ppm", false)));

  // copy solid prototype, and set to wireframed rendering
  g_arcballMat.reset(new Material(solid));
  g_arcballMat->getUniforms().put("uColor", Cvec3f(0.27f, 0.82f, 0.35f));
  g_arcballMat->getRenderStates().polygonMode(GL_FRONT_AND_BACK, GL_LINE);

  // copy solid prototype, and set to color white
  g_lightMat.reset(new Material(solid));
  g_lightMat->getUniforms().put("uColor", Cvec3f(1, 1, 1));

  // pick shader
  g_pickingMat.reset(new Material("./shaders/basic-gl3.vshader", "./shaders/pick-gl3.fshader"));
};

...

int main(int argc, char * argv[]) {
  ...
  initGLState();

  // remove initShaders() and add initMaterials();
  // initShaders();
  initMaterials();

  initGeometry();
  initScene();
  ...
}


//=========================================================================
// STEP 8: Fix scene construction to use Materials. In particular, alter
//         the definition for constructRobot and initScene() as below
//=========================================================================

// constructRobot takes in a material as opposed to a color
static void constructRobot(shared_ptr<SgTransformNode> base, shared_ptr<Material> material) {
  ...

  // The new MyShapeNode takes in a material as opposed to color
  for (int i = 0; i < NUM_SHAPES; ++i) {
    shared_ptr<SgGeometryShapeNode> shape(
      new MyShapeNode(shapeDesc[i].geometry,
                      material, // USE MATERIAL as opposed to color
                      Cvec3(shapeDesc[i].x, shapeDesc[i].y, shapeDesc[i].z),
                      Cvec3(0, 0, 0),
                      Cvec3(shapeDesc[i].sx, shapeDesc[i].sy, shapeDesc[i].sz)));
    jointNodes[shapeDesc[i].parentJointId]->addChild(shape);
  }

  ...
}

// Change the old initScene() to supply various materials when creating MyShapeNode
static void initScene() {

  ...
    
  g_groundNode->addChild(shared_ptr<MyShapeNode>(
                           new MyShapeNode(g_ground, g_bumpFloorMat, Cvec3(0, g_groundY, 0))));

  ...
    
  constructRobot(g_robot1Node, g_redDiffuseMat); // a Red robot
  constructRobot(g_robot2Node, g_blueDiffuseMat); // a Blue robot

  ...
}


//=========================================================================
// STEP 9: Minor fix up of keyboard(): In keyboard(), remove the handling
//         of 'f' key, since we no longer have g_activeShader and so on.
//         You could have achived the same effect using
//         g_overridingMaterial, but we'll pass.
//=========================================================================

static void keyboard(const unsigned char key, const int x, const int y) {
  ...
  // Remove the following:
  //case 'f':
  //  g_activeShader = (g_activeShader + 1) % g_numRegularShaders;
  //  break;
  ...
}


//=========================================================================
// CONGRATULATIONS! YOUR PROGRAM SHOULD NOW COMPILE AND RUN
//=========================================================================
