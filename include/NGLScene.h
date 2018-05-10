#ifndef NGLSCENE_H_
#define NGLSCENE_H_
#include "WindowParams.h"
#include <ngl/Camera.h>
#include <ngl/Colour.h>
#include <ngl/Light.h>
#include <ngl/Text.h>
#include <QOpenGLWindow>
#include <ngl/Transformation.h>
#include <string>
#include <ngl/Texture.h>
#include <ngl/Obj.h>
#include <memory>
#include <vector>
//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.h
/// @brief this class inherits from the Qt OpenGLWindow and allows us to use NGL to draw OpenGL
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/9/13
/// Revision History :
/// This is an initial version used for the new NGL6 / Qt 5 demos
/// @class NGLScene
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file
//----------------------------------------------------------------------------------------------------------------------

class NGLScene : public QOpenGLWindow
{
  Q_OBJECT
public:
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief ctor for our NGL drawing class
  /// @param [in] parent the parent window to the class
  //----------------------------------------------------------------------------------------------------------------------
  NGLScene();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief dtor must close down ngl and release OpenGL resources
  //----------------------------------------------------------------------------------------------------------------------
  ~NGLScene();
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the initialize class is called once when the window is created and we have a valid GL context
  /// use this to setup any default GL stuff
  //----------------------------------------------------------------------------------------------------------------------
  void initializeGL() override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this is called everytime we want to draw the scene
  //----------------------------------------------------------------------------------------------------------------------
  void paintGL() override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this is called everytime we want to draw the scene
  //----------------------------------------------------------------------------------------------------------------------
  void resizeGL(int _w, int _h) override;

  void initShader(std::string shaderProgramName, std::string vertSource, std::string fragSource);
  void initLights(ngl::Colour _lightColour, std::string shaderName);
private:
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the windows params such as mouse and rotations etc
  //----------------------------------------------------------------------------------------------------------------------
  WinParams m_win;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief used to store the global mouse transforms
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Mat4 m_mouseGlobalTX;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Our Camera
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Camera m_cam;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief the model position for mouse movement
  //----------------------------------------------------------------------------------------------------------------------
  ngl::Vec3 m_modelPos;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief method to load transform matrices to the shader
  //----------------------------------------------------------------------------------------------------------------------
  void loadMatricesToShader(bool mouseControls);
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief Qt Event called when a key is pressed
  /// @param [in] _event the Qt event to query for size etc
  //----------------------------------------------------------------------------------------------------------------------
  void keyPressEvent(QKeyEvent *_event) override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called every time a mouse is moved
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void mouseMoveEvent(QMouseEvent *_event) override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called everytime the mouse button is pressed
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void mousePressEvent(QMouseEvent *_event) override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called everytime the mouse button is released
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void mouseReleaseEvent(QMouseEvent *_event) override;
  //----------------------------------------------------------------------------------------------------------------------
  /// @brief this method is called everytime the mouse wheel is moved
  /// inherited from QObject and overridden here.
  /// @param _event the Qt Event structure
  //----------------------------------------------------------------------------------------------------------------------
  void wheelEvent(QWheelEvent *_event) override;

  void createDopFBO(GLuint _colourUnit, GLuint _depthUnit);
  void loadCubemap();
  void createTexturePack();
  void createShadowFBO(GLuint _depthUnit);
  void drawScene(bool _withLightGeo);
  void setPBRTextures(std::string _shaderName, std::vector<std::string> _textures);

  ngl::Transformation m_modelTransform;
  ngl::Vec3 m_lightPos;

  ngl::Light m_keyLight;
  ngl::Light m_fillLight;
  ngl::Light m_rimLight;

  ngl::Texture m_texture;
  std::vector<std::string> m_cubeTextures;

  //FBO uniform values
  bool m_isFBODirty = true;
  bool m_blurSwitch = false;
  float m_focalDepth = 0.75f;

  //Object IDs
  GLuint m_fboShadowId;
  GLuint m_textureShadowId;
  GLuint m_fboId;
  GLuint m_fboTextureId;
  GLuint m_fboDepthId;
  GLuint m_cubeMapId;

  //Texture arrays
  std::vector<std::string> m_harmonicaTopTextures;
  std::vector<std::string> m_harmonicaBottomTextures;
  std::vector<std::string> m_harmonicaMiddleTextures;
  std::vector<std::string> m_woodTextures;

  std::unique_ptr<ngl::Obj> m_harmonicaTop;
  std::unique_ptr<ngl::Obj> m_harmonicaBottom;
  std::unique_ptr<ngl::Obj> m_harmonicaMiddle;
};


#endif
