#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/InputEvents.h>
#include <memory>
#include "CEGuiProxy.h"

using namespace Urho3D;
class MyApp : public Application
{
public:
    MyApp(Context* context) :
        Application(context)
    {
    }
    virtual void Setup()
    {
		engineParameters_["FullScreen"] = false;
        // Called before engine initialization. engineParameters_ member variable can be modified here
    }
    virtual void Start()
    {
        // Called after engine initialization. Setup application & subscribe to events here
        SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(MyApp, HandleKeyDown));
    }
    virtual void Stop()
    {
        // Perform optional cleanup after main loop has terminated
    }
    void HandleKeyDown(StringHash eventType, VariantMap& eventData)
    {
        using namespace KeyDown;
        // Check for pressing ESC. Note the engine_ member variable for convenience access to the Engine object
        int key = eventData[P_KEY].GetInt();
        if (key == KEY_ESCAPE)
            engine_->Exit();
		
		CEGUI::Key::Scan ceguiKey = urho3DKeyToCeguiKey(Key(key));

// 		if (eventData[P_BUTTONS].i == GLFW_PRESS)
// 		{
// 			d_sampleApp->injectKeyDown(ceguiKey);
// 		}
// 		else if (action == GLFW_RELEASE)
// 		{
// 			d_sampleApp->injectKeyUp(ceguiKey);
// 	
    }
private:
	std::unique_ptr<CEGuiBaseApplication> cegui_proxy_;
};
URHO3D_DEFINE_APPLICATION_MAIN(MyApp)