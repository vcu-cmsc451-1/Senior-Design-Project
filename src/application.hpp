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

application::application():
    resource_cfg {Ogre::StringUtil::BLANK},
    plugins_cfg  {Ogre::StringUtil::BLANK},
    cursor_visible {false},
    shutdown       {false},
    root      {0},
    cam       {0},
    scene_mgr {0},
    wnd       {0},
    tray_mgr  {0},
    cameraman {0},
    input     {0},
    mouse     {0},
    kbd       {0}
{ }

application::~application()
{
    if (tray_mgr)  delete tray_mgr;
    if (cameraman) delete cameraman;

    Ogre::WindowEventUtilities::removeWindowEventListener (wnd, this);
    windowClosed(wnd);

    delete root;
}

bool application::configure()
{
    if ( root->showConfigDialog() )
    {
        wnd = root->initialize (true, "Render Window");
        return true;
    }

    else return false;
}

void application::choose_scene_mgr()
{
    scene_mgr = root->createSceneManager(Ogre::ST_GENERIC);
    overlay = new Ogre::OverlaySystem();
    scene_mgr->addRenderQueueListener(overlay);
}

void application::create_camera()
{
    cam = scene_mgr->createCamera("PlayerCam");
    cam->setPosition(Ogre::Vector3 {0, 0, 80});
    cam->lookAt(Ogre::Vector3 {0, 0, -300});
    cam->setNearClipDistance(5);

    cameraman = new OgreBites::SdkCameraMan(cam);
}

void application::create_frame_listener()
{
    Ogre::LogManager::getSingletonPtr() -> logMessage("Initializing OIS");
    OIS::ParamList pl;
    size_t wndhnd = 0;
    std::ostringstream wndhndstr;

    wnd->getCustomAttribute ("WINDOW", &wndhnd);
    wndhndstr << wndhnd;
    pl.insert (std::make_pair (std::string {"WINDOW"}, wndhndstr.str()));

    input = OIS::InputManager::createInputSystem(pl);

    kbd   = static_cast<OIS::Keyboard*> (input->createInputObject (OIS::OISKeyboard, true));
    mouse = static_cast<OIS::Mouse*>    (input->createInputObject (OIS::OISMouse,    true));

    mouse -> setEventCallback(this);
    kbd   -> setEventCallback(this);

    windowResized(wnd);

    Ogre::WindowEventUtilities::addWindowEventListener (wnd, this);

    OgreBites::InputContext inctx;
    inctx.mouse = mouse;
    inctx.kbd   = kbd;
    tray_mgr = new OgreBites::SdkTrayManager ("InterfaceName", wnd, inctx, this);
    tray_mgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
    tray_mgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
    tray_mgr->hideCursor();

    Ogre::StringVector items;
    items.push_back("cam.pX");
    items.push_back("cam.pY");
    items.push_back("cam.pZ");
    items.push_back("");
    items.push_back("cam.oW");
    items.push_back("cam.oX");
    items.push_back("cam.oY");
    items.push_back("cam.oZ");
    items.push_back("");
    items.push_back("Filtering");
    items.push_back("Poly Mode");

    root->addFrameListener(this);
}

application::destroy_scene() { }

application::create_viewports()
{
    Ogre::Viewport* vp = wnd->addViewport(cam);
    vp->setBackgroundColour (Ogre::ColourValue {0, 0, 0});

    cam->setAspectRatio (Ogre::Real (vp->getActualWidth()) / Ogre::Real (vp->getActualHeight()));
}

application::setup_resources()
{
    Ogre::ConfigFile conf;
    conf.load(resources_cfg);

    auto it = conf.getSectionIterator();

    while (it.hasMoreElements())
    {
        auto sect = it.peekNextKey();
        auto settings = it.getNext();
        for (auto i: *settings)
            Ogre::ResourceGroupManager::getSingleton()
                .addResourceLocation (i->second, i->first, sect);
    }
}

application::create_resource_listener() { }

application::load_resources()
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

application::go()
{
    resource_cfg = "resources.cfg";
    plugins_cfg  = "plugins.cfg";

    if (!setup()) return;

    root->startRendering();
    destroy_scene();
}

application::setup()
{
    root = new Ogre::Root(plugins_cfg);

    setup_resources();

    if (!configure()) return false;

    choose_scene_mgr();
    create_camera();
    create_viewports();

    Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);

    create_resource_listener();
    load_resources();

    create_scene();
    create_frame_listener();

    return true;
}

application::frameRenderingQueued (Ogre::FrameEvent const& e)
{
    if (wnd->isClosed()) return false;
    if (shutdown)        return false;

    kbd   -> capture();
    mouse -> capture();

    tray_mgr->frameRenderingQueued(e);

    if (!tray_mgr->isDialogVisible())
        cameraman->frameRenderingQueued(e);

    return true;
}

#endif // _APPLICATION_H_
