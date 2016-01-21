#include "application.hpp"

class project: public application
{
    void create_scene()
    {
        scene_mgr -> setAmbientLight (Ogre::ColourValue {0.5, 0.5, 0.5});
        auto ogrehead = scene_mgr -> createEntity("ogrehead.mesh");
        auto ogrenode = scene_mgr -> getRootSceneNode() -> createChildSceneNode();
        ogrenode -> attachObject (ogrehead);
        auto light = scene_mgr -> createLight ("MainLight");
        light -> setPosition (20, 80, 50);
    }
};

int main()
{
    project app;

    try
    {
        app.go();
    }
    
    catch (Ogre::Exception const& e)
    {
        std::cerr << "An exception has occured: " << e.getFullDescription() << "\n";
    }
}
