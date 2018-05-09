#include "NGLScene.h"
#include <QGuiApplication>
#include <QMouseEvent>

#include <ngl/Camera.h>
#include <ngl/Light.h>
#include <ngl/Material.h>
#include <ngl/NGLInit.h>
#include <ngl/NGLStream.h>
#include <ngl/ShaderLib.h>
#include <ngl/VAOPrimitives.h>
#include <ngl/Image.h>
#include <string>


NGLScene::NGLScene()
{
  setTitle( "Real-Time Rendering Assignment - s4920723");
}


NGLScene::~NGLScene()
{
  std::cout << "Shutting down NGL, removing VAO's and Shaders\n";
}


void NGLScene::resizeGL( int _w, int _h )
{
  m_cam.setShape( 45.0f, static_cast<float>( _w ) / _h, 0.05f, 350.0f );
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  m_isFBODirty = true;
}


void NGLScene::initializeGL()
{
  ngl::NGLInit::instance();
  glClearColor( 0.4f, 0.4f, 0.4f, 1.0f );
  glEnable( GL_DEPTH_TEST );
  glEnable(GL_TEXTURE_2D);
// enable multisampling for smoother drawing
#ifndef USINGIOS_
  glEnable( GL_MULTISAMPLE );
#endif

  //CREATING SHADERS
  ngl::ShaderLib* shader = ngl::ShaderLib::instance();
  initShader("myPhong", "shaders/myPhongVert.glsl", "shaders/myPhongFrag.glsl");
  initShader("myPBR", "shaders/myPBRVert.glsl", "shaders/myPBRFrag.glsl");
  initShader("jonPhong", "shaders/PhongVertex.glsl", "shaders/PhongFragment.glsl");
  initShader("envShader", "shaders/envVert.glsl", "shaders/envFrag.glsl");
  initShader("dopShader", "shaders/dopVert.glsl", "shaders/dopFrag.glsl");
  initShader("shadowShader", "shaders/shadowVert.glsl", "shaders/shadowFrag.glsl");

  ( *shader )[ "myPBR" ]->use();
  //CAMERA SETUP
  ngl::Vec3 from( 1, 1, 1 );
  ngl::Vec3 to( 0, 0, 0 );
  ngl::Vec3 up( 0, 1, 0 );
  m_cam.set( from, to, up );
  m_cam.setShape( 45.0f, 720.0f / 576.0f, 0.05f, 350.0f );
  shader->setUniform( "viewerPos", m_cam.getEye().toVec3());

  //CREATING LIGHTS
  m_lightPos.set(5.0f, 10.0f, 5.0f);
  initLights(ngl::Colour(1.0, 1.0, 1.0, 1.0), "myPBR");

  //SETTING UP GEOMETRY
  m_harmonicaTop.reset(  new ngl::Obj("data/harmonica02_top.obj"));
  m_harmonicaTop->createVAO();
  m_harmonicaBottom.reset(  new ngl::Obj("data/harmonica02_bottom.obj"));
  m_harmonicaBottom->createVAO();
  m_harmonicaMiddle.reset(  new ngl::Obj("data/harmonica02_middle.obj"));
  m_harmonicaMiddle->createVAO();


  //SETTING UP TEXTURES
  shader->use("myPBR");
  m_texture.loadImage("textures/harmonica/harmonica_top_albedo.jpg");
  m_texture.setMultiTexture(0);
  m_texture.setTextureGL();
  shader->setUniform("albedoMap", 0);
  m_texture.loadImage("textures/harmonica/harmonicaTop/top_albedo.jpg");
  m_texture.setMultiTexture(5);
  m_texture.setTextureGL();
  shader->setUniform("albedoMap2", 5);
  m_texture.loadImage("textures/harmonica/harmonicaTop/top_roughness.jpg");
  m_texture.setMultiTexture(1);
  m_texture.setTextureGL();
  shader->setUniform("roughnessMap", 1);
  m_texture.loadImage("textures/harmonica/harmonicaTop/top_metallic.jpg");
  m_texture.setMultiTexture(2);
  m_texture.setTextureGL();
  shader->setUniform("metallicMap", 2);
  m_texture.loadImage("textures/harmonica/harmonicaTop/top_normal.jpg");
  m_texture.setMultiTexture(3);
  m_texture.setTextureGL();
  shader->setUniform("normalMap", 3);
  m_texture.loadImage("textures/harmonica/harmonicaTop/top_ao.jpg");
  m_texture.setMultiTexture(4);
  m_texture.setTextureGL();
  shader->setUniform("aoMap", 4);

  //CREATE ENVIRONMENT MAP
  m_cubeTextures.push_back("textures/envTex/nissiBeach/posx.jpg");
  m_cubeTextures.push_back("textures/envTex/nissiBeach/negx.jpg");
  m_cubeTextures.push_back("textures/envTex/nissiBeach/negy.jpg");
  m_cubeTextures.push_back("textures/envTex/nissiBeach/posy.jpg");
  m_cubeTextures.push_back("textures/envTex/nissiBeach/posz.jpg");
  m_cubeTextures.push_back("textures/envTex/nissiBeach/negz.jpg");
  loadCubemap();

  //INITIALIZE GEOMTRY PRIMITIVES
  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  prim->createTrianglePlane("plane",2,2,1,1,ngl::Vec3(0,1,0));
  prim->createSphere("lightShape", 0.15f, 64);
  prim->createTrianglePlane("table", 3, 3, 1, 1, ngl::Vec3(0,1,0));
}


void NGLScene::initShader(std::string shaderProgramName, std::string vertSource, std::string fragSource)
{
    ngl::ShaderLib* shader = ngl::ShaderLib::instance();
    auto shaderProgram = shaderProgramName;
    auto vertexShader  = "Vertex";
    auto fragShader    = "Fragment";
    shader->createShaderProgram( shaderProgram );
    shader->attachShader( vertexShader, ngl::ShaderType::VERTEX );
    shader->attachShader( fragShader, ngl::ShaderType::FRAGMENT );
    shader->loadShaderSource( vertexShader, vertSource);
    shader->loadShaderSource( fragShader, fragSource);
    shader->compileShader( vertexShader );
    shader->compileShader( fragShader );
    shader->attachShaderToProgram( shaderProgram, vertexShader );
    shader->attachShaderToProgram( shaderProgram, fragShader );
    shader->linkProgramObject( shaderProgram );
}


void NGLScene::initLights(ngl::Colour _lightColour, std::string shaderName)
{
    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    shader->use(shaderName);

    ngl::Vec3 keyLightOffset;
    ngl::Vec3 fillLightOffset;
    ngl::Vec3 rimLightOffset;

    m_keyLight.enable();
    m_fillLight.enable();
    m_rimLight.enable();

    m_keyLight.setPosition(m_lightPos + keyLightOffset);
    m_fillLight.setPosition(m_lightPos + fillLightOffset);
    m_rimLight.setPosition(m_lightPos + rimLightOffset);

    m_keyLight.setColour(_lightColour);
    m_fillLight.setColour(_lightColour);
    m_rimLight.setColour(_lightColour);

    m_keyLight.setSpecColour(_lightColour);
    m_fillLight.setSpecColour(_lightColour);
    m_rimLight.setSpecColour(_lightColour);

    ngl::Mat4 iv = m_cam.getViewMatrix();
    iv.transpose();
    m_keyLight.setTransform(iv);
    m_fillLight.setTransform(iv);
    m_rimLight.setTransform(iv);

    m_keyLight.loadToShader("keyLight");
    m_fillLight.loadToShader("fillLight");
    m_rimLight.loadToShader("rimLight");
}


void NGLScene::createDopFBO(GLuint _colourUnit, GLuint _depthUnit)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER)==GL_FRAMEBUFFER_COMPLETE)
    {
        glDeleteTextures(1, &m_fboTextureId);
        glDeleteTextures(1, &m_fboDepthId);
        glDeleteFramebuffers(1, &m_fboId);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    //Create Colour Texture that the framebuffer will write to
    glGenTextures(1, &m_fboTextureId);
    glActiveTexture(GL_TEXTURE0+_colourUnit);
    glBindTexture(GL_TEXTURE_2D, m_fboTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_win.width, m_win.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Create Depth Texture that the framebuffer will write to
    glGenTextures(1, &m_fboDepthId);
    glActiveTexture(GL_TEXTURE0 + _depthUnit);
    glBindTexture(GL_TEXTURE_2D, m_fboDepthId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_win.width, m_win.height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);

    //Create framebuffer, bind it and attach the textures to the framebuffer
    glGenFramebuffers(1, &m_fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTextureId, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_fboDepthId, 0);
    GLenum drawBufs[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBufs);

    //Check if the framebuffer works and bind the revert back to the normal framebuffer
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "DOP frame buffer works!\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void NGLScene::createShadowFBO(GLuint _depthUnit)
{
    glGenTextures(1, &m_textureShadowId);
    glActiveTexture(GL_TEXTURE0+_depthUnit);
    glBindTexture(GL_TEXTURE_2D, m_textureShadowId);

    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, m_win.width, m_win.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // create our FBO
    glGenFramebuffers(1, &m_fboShadowId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboShadowId);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, m_textureShadowId, 0);
    //glDrawBuffer(GL_NONE);
    //glReadBuffer(GL_NONE);

    //Check if the framebuffer works and bind the revert back to the normal framebuffer
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Shadow frame buffer works!\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void NGLScene::loadCubemap()
{
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glGenTextures(1, &m_cubeMapId);

    glActiveTexture (GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_cubeMapId);

    for (unsigned int i = 0; i < m_cubeTextures.size(); i++)
    {
        ngl::Image img;
        img.load(m_cubeTextures[i]);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGB, static_cast<GLsizei>(img.width()), static_cast<GLsizei>(img.width()), 0, GL_RGB, GL_UNSIGNED_BYTE, img.getPixels());
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    ngl::ShaderLib *shader=ngl::ShaderLib::instance();
    shader->use("envShader");
    shader->setUniform("envMap", 0);
    shader->use("shadowShader");
    shader->setUniform("envMap", 0);
}


void NGLScene::loadMatricesToShader(bool mouseControls)
{
  ngl::ShaderLib* shader = ngl::ShaderLib::instance();

  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  if (mouseControls)
  M = m_mouseGlobalTX * m_modelTransform.getMatrix();
  else
  M = m_modelTransform.getMatrix();
  MV = m_cam.getViewMatrix() * M;
  MVP = m_cam.getVPMatrix() * M;

  normalMatrix = MV;
  normalMatrix.inverse().transpose();
  shader->setUniform( "MV", MV );
  shader->setUniform( "MVP", MVP );
  shader->setUniform( "normalMatrix", normalMatrix );
  shader->setUniform( "M", M );
  shader->setUniform("diffuseColour", ngl::Vec3(1.0f, 0.6f, 0.15f));
  shader->setUniform("lightCol", ngl::Vec3(1.0f, 1.0f, 1.0f));
  shader->setUniform("lightPos", m_lightPos);
  shader->setUniform("camPos", m_cam.getEye().toVec3());
}

void NGLScene::drawScene(bool _withLightGeo)
{
  ngl::ShaderLib* shader = ngl::ShaderLib::instance();
  ngl::VAOPrimitives* prim = ngl::VAOPrimitives::instance();

  // DRAW SCENE
  //drawing environment cube
  (*shader)[ "envShader" ]->use();
  m_modelTransform.reset();
  m_modelTransform.setScale(-100.0, -100.0, -100.0);
  loadMatricesToShader(true);
  prim->draw("cube");

  //drawing the harmonica (or for teapot for test purposes)

  ( *shader )["myPBR"]->use();
  m_modelTransform.setMatrix(1.0f);
  m_modelTransform.setScale(0.05f, 0.05f, 0.05f);
  loadMatricesToShader(true);
  m_harmonicaTop->draw();
  m_harmonicaMiddle->draw();
  m_harmonicaBottom->draw();


  //drawing other geomtry (ground plane and light spheres)
  ( *shader )[ ngl::nglColourShader ]->use();
  shader->setUniform("Colour", 0.0f, 1.0f, 0.0f, 1.0f);
  m_modelTransform.reset();
  m_modelTransform.setPosition(0.0f, -0.3f, 0.0f);
  loadMatricesToShader(true);
  prim->draw("table");

  if (_withLightGeo)
  {
    ( *shader )[ ngl::nglColourShader ]->use();
    shader->setUniform("Colour", 1.0f, 1.0f, 1.0f, 1.0f);
    m_modelTransform.setPosition(m_lightPos);
    loadMatricesToShader(true);
    prim->draw("lightShape");
  }
}

void NGLScene::paintGL()
{
  // Mouse control matricies
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  rotX.rotateX( m_win.spinXFace );
  rotY.rotateY( m_win.spinYFace );
  m_mouseGlobalTX = rotX * rotY;
  m_mouseGlobalTX.m_m[ 3 ][ 0 ] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[ 3 ][ 1 ] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[ 3 ][ 2 ] = m_modelPos.m_z;

  if (m_isFBODirty)
  {
      createDopFBO(6, 7);
      createShadowFBO(9);
      m_isFBODirty = false;
  }

  //switching to DOP framebuffer object and rendering geometry
  glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
  glViewport(0,0,m_win.width, m_win.height);
  glClearColor(0.5, 0.65f, 0.9f, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ngl::ShaderLib* shader = ngl::ShaderLib::instance();
  ngl::VAOPrimitives* prim = ngl::VAOPrimitives::instance();
  drawScene(true);

  //switching to shadow framebuffe object, moving camera, rendering geometry
  glBindFramebuffer(GL_FRAMEBUFFER, m_fboShadowId);
  glViewport(0,0,m_win.width, m_win.height);
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  ngl::Vec3 from = m_lightPos;
  from.clamp(0.1f, 10.f);
  m_cam.set(from, ngl::Vec3(0.0f, 0.0f, 0.0f), ngl::Vec3(0.0f, 1.0f, 0.0f));
  drawScene(false);
  m_cam.set(ngl::Vec3(1.0f, 1.0f, 1.0f), ngl::Vec3(0.0f, 0.0f, 0.0f), ngl::Vec3(0.0f, 1.0f, 0.0f));

  //Switch back to normal framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);

  //Shadowspass shader uniforms
  (*shader)["shadowShader"]->use();
  glActiveTexture(GL_TEXTURE9);
  glBindTexture(GL_TEXTURE_2D, m_fboShadowId);
  shader->setUniform("depthMap", 9);

  //DOP shader uniforms
  (*shader)["dopShader"]->use();
  glActiveTexture(GL_TEXTURE6);
  glBindTexture(GL_TEXTURE_2D, m_fboTextureId);
  shader->setUniform("fboTexture", 6);
  glActiveTexture(GL_TEXTURE7);
  glBindTexture(GL_TEXTURE_2D, m_fboDepthId);
  shader->setUniform("fboDepth", 7);
  shader->setUniform("fboSize", ngl::Vec2(m_win.width, m_win.height));
  shader->setUniform("focalDepth", m_focalDepth);
  shader->setUniform("blurSwitch", m_blurSwitch);
  shader->setUniform("blurRadius", 0.01f);

  (*shader)["dopShader"]->use();
  m_modelTransform.reset();
  m_modelTransform.addRotation(90.0, 0.0, 0.0);
  loadMatricesToShader(false);
  prim->draw("plane");
  glBindTexture(GL_TEXTURE_2D, 0);
}

void NGLScene::keyPressEvent( QKeyEvent* _event )
{
  switch ( _event->key() )
  {
    // escape key to quit
    case Qt::Key_Escape:
      QGuiApplication::exit( EXIT_SUCCESS );
      break;
// turn on wirframe rendering
#ifndef USINGIOS_
    case Qt::Key_W:
      glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );
      break;
    // turn off wire frame
    case Qt::Key_S:
      glPolygonMode( GL_FRONT_AND_BACK, GL_FILL );
      break;
#endif
    // show full screen
    case Qt::Key_F:
      showFullScreen();
      break;
    // show windowed
    case Qt::Key_N:
      showNormal();
      break;
    case Qt::Key_Space :
      m_win.spinXFace=0;
      m_win.spinYFace=0;
      m_modelPos.set(ngl::Vec3::zero());
    break;

    //light controls
    case Qt::Key_Right: m_lightPos+=ngl::Vec3(0.0f, 0.0f, 0.1f);
    break;
    case Qt::Key_Left: m_lightPos+=ngl::Vec3(0.0f, 0.0f, -0.1f);
    break;
    case Qt::Key_Up: m_lightPos+=ngl::Vec3(-0.1f, 0.0f, 0.0f);
    break;
    case Qt::Key_Down: m_lightPos+=ngl::Vec3(0.1f, 0.0f, 0.0f);
    break;
    case Qt::Key_Plus: m_lightPos+=ngl::Vec3(0.0f, 0.1f, 0.0f);
    break;
    case Qt::Key_Minus: m_lightPos+=ngl::Vec3(0.0f, -0.1f, 0.0f);
    break;
    //DOP controls
    case Qt::Key_B : if (m_blurSwitch) m_blurSwitch=false;
                     else m_blurSwitch=true;
    break;
    case Qt::Key_Comma: m_focalDepth -= 0.005f;
    break;
    case Qt::Key_Period: m_focalDepth += 0.005f;
    break;
    default:
    break;
  }
  update();
}
