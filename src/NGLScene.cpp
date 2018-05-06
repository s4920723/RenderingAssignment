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


NGLScene::NGLScene()
{
  setTitle( "Real-Time Rendering Assignment - s4920723" );
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
  initShader("jonPhong", "shaders/PhongVertex.glsl", "shaders/PhongFragment.glsl");
  initShader("multipassShader", "shaders/multipassVert.glsl", "shaders/multiPassFrag.glsl");
  ( *shader )[ "myPhong" ]->use();
  //CAMERA SETUP
  ngl::Vec3 from( 1, 1, 1 );
  ngl::Vec3 to( 0, 0, 0 );
  ngl::Vec3 up( 0, 1, 0 );
  m_cam.set( from, to, up );
  m_cam.setShape( 45.0f, 720.0f / 576.0f, 0.05f, 350.0f );
  shader->setUniform( "viewerPos", m_cam.getEye().toVec3());

  //CREATING LIGHTS
;
  initLights(ngl::Colour(1.0, 1.0, 1.0, 1.0), "myPhong");

  //SETTING UP GEOMETRY
  //m_harmonicaGeo.reset(  new ngl::Obj("data/harmonica_full.obj", "textures/onyxTiles/TilesOnyxOpaloBlack001_COL_2K.jpg"));
  // now we need to create this as a VAO so we can draw it
  //m_harmonicaGeo->createVAO();

  //SETTING UP TEXTURES
  m_texture.loadImage("textures/onyxTiles/TilesOnyxOpaloBlack001_COL_2K.jpg");
  m_texture.setTextureGL();

  createFBO();

  ngl::VAOPrimitives *prim=ngl::VAOPrimitives::instance();
  prim->createTrianglePlane("plane",2,2,1,1,ngl::Vec3(0,1,0));
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

void NGLScene::createFBO()
{
    glGenFramebuffers(1, &m_fboId);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
    glGenTextures(1, &m_fboTextureId);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_fboTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_fboTextureId, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE)
    {
        std::cout << "Frame buffer works!\n";
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void NGLScene::loadMatricesToShader(bool mouseControls)
{
  ngl::ShaderLib* shader = ngl::ShaderLib::instance();

  ngl::Mat4 MV;
  ngl::Mat4 MVP;
  ngl::Mat3 normalMatrix;
  ngl::Mat4 M;
  if (mouseControls)
  M            = m_mouseGlobalTX * m_modelTransform.getMatrix();
  else
  M            = m_modelTransform.getMatrix();
  MV           = m_cam.getViewMatrix() * M;
  MVP          = m_cam.getVPMatrix() * M;

  normalMatrix = MV;
  normalMatrix.inverse().transpose();
  shader->setUniform( "MV", MV );
  shader->setUniform( "MVP", MVP );
  shader->setUniform( "normalMatrix", normalMatrix );
  shader->setUniform( "M", M );
  shader->setUniform("diffuseColour", ngl::Vec3(1.0f, 0.6f, 0.15f));
  shader->setUniform("lightColour", ngl::Vec3(1.0f, 1.0f, 1.0f));
  shader->setUniform("lightPos", m_lightPos);
  shader->setUniform("camPos", m_cam.getEye());
}

void NGLScene::paintGL()
{
  glViewport( 0, 0, m_win.width, m_win.height );
  glClearColor(0.5, 0.65, 0.9, 1.0);
  // clear the screen and depth buffer
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  // grab an instance of the shader manager
  ngl::ShaderLib* shader = ngl::ShaderLib::instance();
  ( *shader )[ "myPhong" ]->use();

  // Rotation based on the mouse position for our global transform
  ngl::Mat4 rotX;
  ngl::Mat4 rotY;
  // create the rotation matrices
  rotX.rotateX( m_win.spinXFace );
  rotY.rotateY( m_win.spinYFace );
  // multiply the rotations
  m_mouseGlobalTX = rotX * rotY;
  // add the translations
  m_mouseGlobalTX.m_m[ 3 ][ 0 ] = m_modelPos.m_x;
  m_mouseGlobalTX.m_m[ 3 ][ 1 ] = m_modelPos.m_y;
  m_mouseGlobalTX.m_m[ 3 ][ 2 ] = m_modelPos.m_z;

  // get the VBO instance and draw the built in teapot
  ngl::VAOPrimitives* prim = ngl::VAOPrimitives::instance();
  prim->createSphere("sphere", 0.15, 64);
  prim->createDisk("surface", 1, 64);

  //switching framebuffer
  glBindFramebuffer(GL_FRAMEBUFFER, m_fboId);
  glClearColor(0.5, 0.65, 0.9, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // draw teapot
  m_modelTransform.setMatrix(1.0f);
  loadMatricesToShader(true);
  prim->draw( "teapot" );
  //m_harmonicaGeo->draw();
  //draw disk
  ( *shader )[ ngl::nglColourShader ]->use();
  shader->setUniform("Colour", 0.5f, 0.5f, 0.5f, 1.0f);
  m_modelTransform.setPosition(0.0, -0.5, 0.0);
  m_modelTransform.setRotation(90.0, 0.0, 0.0);
  loadMatricesToShader(true);
  prim->draw("surface");
  ( *shader )[ ngl::nglColourShader ]->use();
  shader->setUniform("Colour", 1.0f, 1.0f, 1.0f, 1.0f);
  m_modelTransform.setPosition(m_lightPos);
  loadMatricesToShader(false);
  prim->draw("sphere");


  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  (*shader)["multipassShader"]->use();
  GLint pid = shader->getProgramID("multipassShader");


  glActiveTexture(GL_TEXTURE1);
  glBindTexture(GL_TEXTURE_2D, m_fboTextureId);
  glUniform1i(glGetUniformLocation(pid, "fboTexture"), 1);

  loadMatricesToShader(true);
  prim->draw("plane");
  glBindTexture(GL_TEXTURE_2D, 0);

}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent( QKeyEvent* _event )
{
  // that method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
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
    default:
      break;
  }
  update();
}
