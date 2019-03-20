#include <Urho3D/Engine/Application.h>
#include <Urho3D/Engine/Engine.h>
#include <Urho3D/Input/InputEvents.h>
#include <Urho3D/Input/Input.h>
#include <memory>
#include "CEGuiProxy.h"

using namespace Urho3D;

static MouseButton MakeTouchIDMask(int id)
{
	return static_cast<MouseButton>(1u << static_cast<MouseButtonFlags::Integer>(id)); // NOLINT(misc-misplaced-widening-cast)
}

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
		InitCEGuiProxy();

        // Called after engine initialization. Setup application & subscribe to events here
		SubscribeToEvent(E_MOUSEBUTTONDOWN, URHO3D_HANDLER(MyApp, HandleMouseButtonDown));
		SubscribeToEvent(E_MOUSEBUTTONUP, URHO3D_HANDLER(MyApp, HandleMouseButtonUp));
		//SubscribeToEvent(E_MOUSEMOVE, URHO3D_HANDLER(MyApp, HandleMouseMove));
		SubscribeToEvent(E_MOUSEWHEEL, URHO3D_HANDLER(MyApp, HandleMouseWheel));
		SubscribeToEvent(E_TOUCHBEGIN, URHO3D_HANDLER(MyApp, HandleTouchBegin));
		SubscribeToEvent(E_TOUCHEND, URHO3D_HANDLER(MyApp, HandleTouchEnd));
		SubscribeToEvent(E_TOUCHMOVE, URHO3D_HANDLER(MyApp, HandleTouchMove));
		SubscribeToEvent(E_KEYDOWN, URHO3D_HANDLER(MyApp, HandleKeyDown));
		SubscribeToEvent(E_KEYUP, URHO3D_HANDLER(MyApp, HandleKeyUp));
		SubscribeToEvent(E_TEXTINPUT, URHO3D_HANDLER(MyApp, HandleTextInput));
    }
    virtual void Stop()
    {
        // Perform optional cleanup after main loop has terminated
    }

	void InitCEGuiProxy()
	{
		cegui_proxy_ = std::make_unique<CEGuiBaseApplication>();
		cegui_proxy_->init(nullptr, "CEGUI.log", CEGUI::String());
	}

	void OnMouseButtonDown(MouseButton mouseButtons)
	{
		CEGUI::MouseButton mb{ CEGUI::MouseButton::Invalid };
		if (mouseButtons == MOUSEB_LEFT) {
			mb = CEGUI::MouseButton::Left;
		} else if (mouseButtons == MOUSEB_RIGHT) {
			mb = CEGUI::MouseButton::Right;
		} else if (mouseButtons == MOUSEB_MIDDLE) {
			mb = CEGUI::MouseButton::Middle;
		}
		cegui_proxy_->injectMouseButtonDown(mb);
	}

	void OnMouseButtonUp(MouseButton mouseButtons)
	{
		CEGUI::MouseButton mb{ CEGUI::MouseButton::Invalid };
		if (mouseButtons == MOUSEB_LEFT) {
			mb = CEGUI::MouseButton::Left;
		} else if (mouseButtons == MOUSEB_RIGHT) {
			mb = CEGUI::MouseButton::Right;
		} else if (mouseButtons == MOUSEB_MIDDLE) {
			mb = CEGUI::MouseButton::Middle;
		}
		cegui_proxy_->injectMouseButtonUp(mb);
	}

	void HandleMouseButtonDown(StringHash eventType, VariantMap& eventData)
	{
		using namespace MouseButtonDown;

		auto mouseButtons = MouseButtonFlags(eventData[P_BUTTONS].GetUInt());

		OnMouseButtonDown(MouseButton(eventData[P_BUTTON].GetUInt()));
	}

	void HandleMouseButtonUp(StringHash eventType, VariantMap& eventData)
	{
		using namespace MouseButtonUp;

		auto mouseButtons = MouseButtonFlags(eventData[P_BUTTONS].GetUInt());

		OnMouseButtonUp(MouseButton(eventData[P_BUTTONS].GetUInt()));
	}

	void HandleMouseWheel(StringHash eventType, VariantMap& eventData)
	{
		auto* input = GetSubsystem<Input>();
		if (input->IsMouseGrabbed())
			return;

		using namespace MouseWheel;

		auto mouseButtons = MouseButtonFlags(eventData[P_BUTTONS].GetInt());
		cegui_proxy_->injectMouseWheelChange(eventData[P_WHEEL].GetInt());
	}

	void HandleTouchBegin(StringHash eventType, VariantMap& eventData)
	{
		auto* input = GetSubsystem<Input>();
		if (input->IsMouseGrabbed())
			return;

		using namespace TouchBegin;

		IntVector2 pos(eventData[P_X].GetInt(), eventData[P_Y].GetInt());
// 		pos.x_ = int(pos.x_ / uiScale_);
// 		pos.y_ = int(pos.y_ / uiScale_);
// 		usingTouchInput_ = true;
// 
 		MouseButton touchId = MakeTouchIDMask(eventData[P_TOUCHID].GetInt());
// 		WeakPtr<UIElement> element(GetElementAt(pos));
// 
// 		if (element)
// 		{
// 			ProcessClickBegin(pos, touchId, touchDragElements_[element], QUAL_NONE, nullptr, true);
// 			touchDragElements_[element] |= touchId;
// 		}
// 		else
// 			ProcessClickBegin(pos, touchId, touchId, QUAL_NONE, nullptr, true);
		OnMouseButtonDown(touchId);
	}

	void HandleTouchEnd(StringHash eventType, VariantMap& eventData)
	{
		using namespace TouchEnd;

		IntVector2 pos(eventData[P_X].GetInt(), eventData[P_Y].GetInt());
// 		pos.x_ = int(pos.x_ / uiScale_);
// 		pos.y_ = int(pos.y_ / uiScale_);

		// Get the touch index
		MouseButton touchId = MakeTouchIDMask(eventData[P_TOUCHID].GetInt());
		OnMouseButtonUp(touchId);
// 		// Transmit hover end to the position where the finger was lifted
// 		WeakPtr<UIElement> element(GetElementAt(pos));
// 
// 		// Clear any drag events that were using the touch id
// 		for (auto i = touchDragElements_.Begin(); i != touchDragElements_.End();)
// 		{
// 			const MouseButtonFlags touches = i->second_;
// 			if (touches & touchId)
// 				i = touchDragElements_.Erase(i);
// 			else
// 				++i;
// 		}
// 
// 		if (element && element->IsEnabled())
// 			element->OnHover(element->ScreenToElement(pos), pos, 0, 0, nullptr);
// 
// 		ProcessClickEnd(pos, touchId, MOUSEB_NONE, QUAL_NONE, nullptr, true);
	}

	void HandleTouchMove(StringHash eventType, VariantMap& eventData)
	{
		using namespace TouchMove;

		IntVector2 pos(eventData[P_X].GetInt(), eventData[P_Y].GetInt());
		IntVector2 deltaPos(eventData[P_DX].GetInt(), eventData[P_DY].GetInt());

		MouseButton touchId = MakeTouchIDMask(eventData[P_TOUCHID].GetInt());
	}
	
    void HandleKeyDown(StringHash eventType, VariantMap& eventData)
    {
        using namespace KeyDown;
        // Check for pressing ESC. Note the engine_ member variable for convenience access to the Engine object
        int key = eventData[P_KEY].GetInt();
        if (key == KEY_ESCAPE)
            engine_->Exit();
		
		cegui_proxy_->injectKeyDown(urho3DKeyToCeguiKey(Key(key)));
    }
	
	void HandleKeyUp(StringHash eventType, VariantMap& eventData)
	{
		using namespace KeyDown;
		int key = eventData[P_KEY].GetInt();

		cegui_proxy_->injectKeyUp(urho3DKeyToCeguiKey(Key(key)));
	}

	void HandleTextInput(StringHash eventType, VariantMap& eventData)
	{
		using namespace TextInput;
		auto str = eventData[P_TEXT].GetString();
		cegui_proxy_->injectChar(str[0]);
	}

private:
	std::unique_ptr<CEGuiBaseApplication> cegui_proxy_;
};
URHO3D_DEFINE_APPLICATION_MAIN(MyApp)