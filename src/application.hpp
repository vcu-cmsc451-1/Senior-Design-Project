#ifndef _APPLICATION_H_
#define _APPLICATION_H_

#include <OgreCamera.h>
#include <OgreEntity.h>
#include <OgreLogManager.h>
#include <OgreRoot.h>
#include <OgreViewport.h>
#include <OgreSceneManager.h>
#include <OgreRenderWindow.h>
#include <OgreConfigFile.h>

#include <OISEvents.h>
#include <OISInputManager.h>
#include <OISKeyboard.h>
#include <OISMouse.h>

#include <SdkTrays.h>
#include <SdkCameraMan.h>

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

protected:
    virtual void create_scene();
    virtual void destroy_scene();

    bool setup();
    bool configure();
    void choose_scene_mgr();
    void create_camera();
    void create_frame_listener();
    void create_viewports();
    void setup_resources();
    void create_resource_listener();
    void load_resources();

    Ogre::Root* root;
    Ogre::Camera* cam;
    Ogre::SceneManager* scene_mgr;
    Ogre::RenderWindow* wnd;
    Ogre::String resource_cfg;
    Ogre::String plugin_cfg;
    Ogre::OverlaySystem* overlay;

    OgreBites::SdkTrayManager* tray_mgr;
    OgreBites::SdkCameraMan* cameraman;
    OgreBites::ParamsPanel* details;
    bool cursor_visible;
    bool shutdown;

    OIS::InputManager* input;
    OIS::Mouse* mouse;
    OIS::Keyboard* kbd;

    virtual bool frameRenderingQueued (Ogre::FrameEvent const&);

    virtual bool keyPressed  (OIS::KeyEvent const&);
    virtual bool keyReleased (OIS::KeyEvent const&);

    virtual bool mouseMoved    (OIS::MouseEvent const&);
    virtual bool mousePressed  (OIS::MouseEvent const&, OIS::MouseButtonID);
    virtual bool mouseReleased (OIS::MouseEvent const&, OIS::MouseButtonID);

    virtual void windowResized (Ogre::RenderWindow*);
    virtual void windowClosed  (Ogre::RenderWindow*);
};

application::application():
    resource_cfg {Ogre::StringUtil::BLANK},
    plugin_cfg  {Ogre::StringUtil::BLANK},
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
        wnd = root->initialise (true, "Render Window");
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
    inctx.mMouse    = mouse;
    inctx.mKeyboard = kbd;
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

void application::create_scene() { }
void application::destroy_scene() { }

void application::create_viewports()
{
    Ogre::Viewport* vp = wnd->addViewport(cam);
    vp->setBackgroundColour (Ogre::ColourValue {0, 0, 0});

    cam->setAspectRatio (Ogre::Real (vp->getActualWidth()) / Ogre::Real (vp->getActualHeight()));
}

void application::setup_resources()
{
    Ogre::ConfigFile conf;
    conf.load(resource_cfg);

    auto it = conf.getSectionIterator();

    while (it.hasMoreElements())
    {
        auto sect = it.peekNextKey();
        auto settings = it.getNext();
        for (auto i: *settings)
            Ogre::ResourceGroupManager::getSingleton()
                .addResourceLocation (i.second, i.first, sect);
    }
}

void application::create_resource_listener() { }

void application::load_resources()
{
    Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
}

void application::go()
{
    resource_cfg = "resources.cfg";
    plugin_cfg  = "plugins.cfg";

    if (!setup()) return;

    root->startRendering();
    destroy_scene();
}

bool application::setup()
{
    root = new Ogre::Root(plugin_cfg);

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

bool application::frameRenderingQueued (Ogre::FrameEvent const& e)
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

bool application::keyPressed (OIS::KeyEvent const& arg)
{
    if (tray_mgr->isDialogVisible()) return true;

    if (arg.key == OIS::KC_F)
    {
        tray_mgr->toggleAdvancedFrameStats();
    }
    else if (arg.key == OIS::KC_R)
    {
        Ogre::PolygonMode pm;

        switch (cam->getPolygonMode())
        {
        case Ogre::PM_SOLID:
            pm = Ogre::PM_WIREFRAME;
            break;

        case Ogre::PM_WIREFRAME:
            pm = Ogre::PM_POINTS;
            break;

        default:
            pm = Ogre::PM_SOLID;
            break;
        }

        cam->setPolygonMode(pm);
    }
    else if (arg.key == OIS::KC_F5)
    {
        Ogre::TextureManager::getSingleton().reloadAll();
    }
    else if (arg.key == OIS::KC_SYSRQ)
    {
        wnd->writeContentsToTimestampedFile ("screenshot", ".png");
    }
    else if (arg.key == OIS::KC_ESCAPE)
    {
        shutdown = true;
    }

    cameraman->injectKeyDown(arg);
    return true;
}

bool application::keyReleased (OIS::KeyEvent const& arg)
{
    cameraman->injectKeyUp(arg);
    return true;
}

bool application::mouseMoved (OIS::MouseEvent const& arg)
{
    if (tray_mgr->injectMouseMove(arg))
        return true;

    cameraman->injectMouseMove(arg);
    return true;
}

bool application::mousePressed (OIS::MouseEvent const& arg, OIS::MouseButtonID id)
{
    if (tray_mgr->injectMouseDown (arg, id))
        return true;

    cameraman->injectMouseDown (arg, id);
    return true;
}

bool application::mouseReleased (OIS::MouseEvent const& arg, OIS::MouseButtonID id)
{
    if (tray_mgr->injectMouseUp (arg, id))
        return true;

    cameraman->injectMouseUp (arg, id);
    return true;
}

void application::windowResized (Ogre::RenderWindow* wnd)
{
    unsigned width, height, depth;
    int left, top;

    wnd->getMetrics (width, height, depth, left, top);

    OIS::MouseState const& ms = mouse->getMouseState();
    ms.width = width;
    ms.height = height;
}

void application::windowClosed (Ogre::RenderWindow* wnd)
{
    if (wnd == this->wnd)
        if (input)
        {
            input->destroyInputObject(mouse);
            input->destroyInputObject(kbd);

            OIS::InputManager::destroyInputSystem(input);
            input = 0;
        }
}

#endif // _APPLICATION_H_
