#ifndef _APPLICATION_H_
#define _APPLICATION_H_

class application:
    Ogre::FrameListener,
    Ogre::WindowEventListener,
    OIS::MouseListener,
    OIS::KeyListener,
    OgreBites::SdkTrayListener
{
public:
    application();
    ~application();
    void go();

private:
    bool setup();
    bool configure();
    bool choose_scene_mgr();
    bool create_camera();
    bool create_frame_listener();
    bool create_scene();
    bool destroy_scene();
    bool create_viewports();
    bool setup_resources();
    bool create_resource_listener();
    bool load_resources();

    Ogre::Root root;
    Ogre::Camera cam;
    Ogre::SceneManager scene_mgr;
    Ogre::RenderWindow wnd;
    Ogre::String resource_cfg;
    Ogre::String plugin_cfg;
    Ogre::OverlaySystem overlay;

    OgreBites::SdkTrayManager tray_mgr;
    OgreBites::SdkCameraMan cameraman;
    OgreBites::ParamsPanel details;
    virtual bool cursor_visible;
    virtual bool shutdown;

    OIS::InputManager input;
    OIS::Mouse mouse;
    OIS::Keyboard kbd;

    virtual bool frameRenderingQueued (Ogre::FrameEvent const&);

    virtual bool keyPressed  (OIS::KeyEvent const&);
    virtual bool keyReleased (OIS::KeyEvent const&);

    virtual bool mouseMoved    (OIS::MouseEvent const&);
    virtual bool mousePressed  (OIS::MouseEvent const&);
    virtual bool mouseReleased (OIS::MouseEvent const&);

    virtual void windowResized (Ogre::RenderWindow*);
    virtual void windowClosed  (Ogre::RenderWindow*);
};

#endif // _APPLICATION_H_
