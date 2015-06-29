/* heXon
// Copyright (C) 2015 LucKey Productions (luckeyproductions.nl)
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License along
// with this program; if not, write to the Free Software Foundation, Inc.,
// 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Node.h>
#include <Urho3D/Graphics/Graphics.h>
#include <Urho3D/Graphics/OctreeQuery.h>
#include <Urho3D/IO/FileSystem.h>

#include "inputmaster.h"
#include "tilemaster.h"
#include "hexocam.h"

InputMaster::InputMaster(Context* context, MasterControl* masterControl) : Object(context)
{
    masterControl_ = masterControl;
    input_ = GetSubsystem<Input>();
    Init();
}

InputMaster::~InputMaster()
{
    _controllerActions.Clear();
    _controllerAxis.Clear();
    _controllerAxisActions.Clear();
}

void InputMaster::Init()
{
    BindControllerAction(Urho3D::CONTROLLER_BUTTON_LEFTSTICK, ACTION_CROUCH);
    BindControllerAction(Urho3D::CONTROLLER_BUTTON_A, ACTION_JUMP);
    BindControllerAction(Urho3D::CONTROLLER_BUTTON_RIGHTSHOULDER, ACTION_SWITCH_ITEM);
    BindControllerAction(Urho3D::CONTROLLER_BUTTON_START, ACTION_PAUSE);

    BindControllerAxis(Urho3D::CONTROLLER_AXIS_RIGHTX, AXIS_LOOK_X);
    BindControllerAxis(Urho3D::CONTROLLER_AXIS_RIGHTY, AXIS_LOOK_Y);
    SetJoystickSensitivity(Urho3D::CONTROLLER_AXIS_RIGHTX, 10.f);
    SetJoystickSensitivity(Urho3D::CONTROLLER_AXIS_RIGHTY, 10.f);

    //BindControllerAxisAction(Urho3D::CONTROLLER_AXIS_LEFTY, ACTION_MOVE_FORWARD, -1, 0.5f);
    BindControllerAxisAction(Urho3D::CONTROLLER_AXIS_LEFTY, ACTION_MOVE_FORWARD, ACTION_MOVE_BACKWARD, 0.5f);
    BindControllerAxisAction(Urho3D::CONTROLLER_AXIS_LEFTX, ACTION_STRAFE_LEFT, ACTION_STRAFE_RIGHT, 0.5f);

    BindControllerAxisAction(Urho3D::CONTROLLER_AXIS_TRIGGERLEFT, -1, ACTION_PRIMARY, 0.5f);
    BindControllerAxisAction(Urho3D::CONTROLLER_AXIS_TRIGGERRIGHT, -1, ACTION_SECONDARY, 0.5f);

    SubscribeToEvents();
}

void InputMaster::SetMouseSensitivity(float sensitivity)
{
    _mouseSensitivity = sensitivity;
}

void InputMaster::SetJoystickSensitivity(int axis, float sensitivity)
{
    _joystickSensitivity[axis] = sensitivity;
}

void InputMaster::BindControllerAction(int button, int action)
{
    _controllerActions[button] = action;
}

void InputMaster::BindControllerAxis(int axis, int axisAction, float threshold)
{
    _controllerAxis[axis] = axisAction;
}

void InputMaster::BindControllerAxisAction(int axis, int negative_action, int positive_action, float threshold)
{
    ControllerAxisAction definition;
    definition.negative_action = negative_action;
    definition.positive_action = positive_action;
    definition.threshold = threshold;
    definition.overThreshold = false;

    _controllerAxisActions[axis] = definition;
}

void InputMaster::SubscribeToEvents()
{
    SubscribeToEvent(Urho3D::E_MOUSEBUTTONDOWN, HANDLER(InputMaster, HandleMouseButtonDown));
    SubscribeToEvent(Urho3D::E_MOUSEBUTTONUP, HANDLER(InputMaster, HandleMouseButtonUp));
    SubscribeToEvent(Urho3D::E_MOUSEMOVE, HANDLER(InputMaster, HandleMouseMove));
    SubscribeToEvent(Urho3D::E_MOUSEWHEEL, HANDLER(InputMaster, HandleMouseWheel));
    SubscribeToEvent(Urho3D::E_KEYDOWN, HANDLER(InputMaster, HandleKeyDown));
    SubscribeToEvent(Urho3D::E_KEYUP, HANDLER(InputMaster, HandleKeyUp));
    SubscribeToEvent(Urho3D::E_JOYSTICKBUTTONDOWN, HANDLER(InputMaster, HandleJoystickButtonDown));
    SubscribeToEvent(Urho3D::E_JOYSTICKBUTTONUP, HANDLER(InputMaster, HandleJoystickButtonUp));
    SubscribeToEvent(Urho3D::E_JOYSTICKAXISMOVE, HANDLER(InputMaster, HandleJoystickAxisMove));
}

void InputMaster::HandleMouseButtonUp(StringHash eventType, VariantMap &eventData)
{
}

void InputMaster::HandleMouseButtonDown(StringHash eventType, VariantMap &eventData)
{
    using namespace MouseButtonDown;
    int button = eventData[P_BUTTON].GetInt();
    if (button == MOUSEB_LEFT){
        //Set tile type
    }
    else if (button == MOUSEB_RIGHT){
        //Clear tile
    }
}

void InputMaster::HandleMouseMove(StringHash eventType, VariantMap &eventData)
{
}

void InputMaster::HandleMouseWheel(StringHash eventType, VariantMap &eventData)
{
}

void InputMaster::HandleKeyUp(StringHash eventType, VariantMap &eventData)
{
}

void InputMaster::AddSelection(SharedPtr<Tile> tile)
{
    DeselectAll();
    selectedTiles_ += tile;
    tile->Select();
}

void InputMaster::HandleMouseUp(StringHash eventType, VariantMap &eventData)
{
    using namespace MouseButtonUp;
    int button = eventData[P_BUTTON].GetInt();
    if (button == MOUSEB_LEFT) {}//Deselect when mouse did not move during click on N_VOID
}

void InputMaster::HandleKeyDown(StringHash eventType, VariantMap &eventData)
{
    //Get the triggering key
    using namespace KeyDown;
    int key = eventData[P_KEY].GetInt();

    //Exit when ESC is pressed
    if (key == KEY_ESC) masterControl_->Exit();

    //Take screenshot when 9 is pressed
    else if (key == KEY_9)
    {
        Graphics* graphics = GetSubsystem<Graphics>();
        Image screenshot(context_);
        graphics->TakeScreenShot(screenshot);
        //Here we save in the Data folder with date and time appended
        String fileName = GetSubsystem<FileSystem>()->GetProgramDir() + "Screenshots/Screenshot_" +
                Time::GetTimeStamp().Replaced(':', '_').Replaced('.', '_').Replaced(' ', '_')+".png";
        //Log::Write(1, fileName);
        screenshot.SavePNG(fileName);
    }
    //Pause/Unpause game on P or joystick Start
    else if (key == KEY_P)
    {
        masterControl_->SetPaused(!masterControl_->GetPaused());
    }
    //Enter edit mode on E
}

void InputMaster::DeselectAll()
{
    for (int i = 0; i < selectedTiles_.Length(); i++)
    {
        selectedTiles_[i]->Deselect();
    }
    selectedTiles_.Clear();
}

void InputMaster::HandleJoystickButtonDown(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    using namespace Urho3D::JoystickButtonDown;

    eventData[P_JOYSTICKID];		//int
    int button = eventData[P_BUTTON].GetInt();		//int
    // Process game event
    if(button == JB_START)
    {
        masterControl_->SetPaused(!masterControl_->GetPaused());
    }
}

void InputMaster::HandleJoystickButtonUp(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    using namespace Urho3D::JoystickButtonUp;

    int joyID = eventData[P_JOYSTICKID].GetInt();		//int
    int button = eventData[P_BUTTON].GetInt();		//int

    // Process game event
    if(_controllerActions.Contains(button))
    {
        //SendButtonUpEvent(_controllerActions[button]);
    }}

void InputMaster::HandleJoystickAxisMove(Urho3D::StringHash eventType, Urho3D::VariantMap& eventData)
{
    using namespace Urho3D::JoystickAxisMove;

    eventData[P_JOYSTICKID];		//int
    int axis = eventData[P_AXIS].GetInt();			//int
    float pos = eventData[P_POSITION].GetFloat();		//float

        // Process game event
    if(_controllerAxisActions.Contains(axis))
    {
        int action = -1;
        float newPos = pos;
        if(pos > 0)
        {
            action = _controllerAxisActions[axis].positive_action;
        }
        else if(pos < 0)
        {
            newPos = -pos; // Put into [0,1]
            action = _controllerAxisActions[axis].negative_action;
        }

        // No action specified
        if(action == -1)
            return;

        if(newPos >= _controllerAxisActions[axis].threshold && !_controllerAxisActions[axis].overThreshold)
        {
            _controllerAxisActions[axis].overThreshold = true;
           // SendButtonDownEvent(action);
        }
        else if(newPos < _controllerAxisActions[axis].threshold && _controllerAxisActions[axis].overThreshold)
        {
            _controllerAxisActions[axis].overThreshold = false;
           // SendButtonUpEvent(action);
        }
    }

    if(_controllerAxis.Contains(axis)) {}
        //SendJoystickAxisMoveEvent(_controllerAxis[axis], pos * _mouseSensitivity, 0.25f);

}
