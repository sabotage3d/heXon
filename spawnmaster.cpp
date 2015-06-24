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

#include <Urho3D/Scene/SceneEvents.h>

#include "spawnmaster.h"
#include "tilemaster.h"
#include "tile.h"

SpawnMaster::SpawnMaster(Context *context, MasterControl *masterControl):
    Object(context),
    RazorInterval_{5.0},
    sinceRazorSpawn_{RazorInterval_-2.0f},
    SpireInterval_{23.0},
    sinceSpireSpawn_{SpireInterval_}
{
    masterControl_ = masterControl;

    SubscribeToEvent(E_SCENEUPDATE, HANDLER(SpawnMaster, HandleSceneUpdate));
}

Vector3 SpawnMaster::CreateSpawnPoint()
{
    Tile* randomTile = masterControl_->tileMaster_->GetRandomTile();
    if (randomTile) {
        Vector3 tilePosition = randomTile->rootNode_->GetPosition();
        return Vector3(tilePosition.x_, -23.0f, tilePosition.z_);
    }
    else return Vector3(Random(-5.0f, 5.0f), -23.0f, Random(-5.0f, 5.0f));
}

void SpawnMaster::HandleSceneUpdate(StringHash eventType, VariantMap &eventData)
{
    using namespace SceneUpdate;

    double timeStep = eventData[P_TIMESTEP].GetFloat();

    sinceRazorSpawn_ += timeStep;
    sinceSpireSpawn_ += timeStep;

    if (sinceRazorSpawn_ > RazorInterval_) SpawnRazor(CreateSpawnPoint());
    if (sinceSpireSpawn_ > SpireInterval_) SpawnSpire(CreateSpawnPoint());
}

void SpawnMaster::SpawnRazor(Vector3 position)
{
    sinceRazorSpawn_ = 0.0;
    Razor* newRazor = new Razor(context_, masterControl_, position);
    razors_[newRazor->rootNode_->GetID()] = SharedPtr<Razor>(newRazor);
}

void SpawnMaster::SpawnSpire(Vector3 position)
{
    sinceSpireSpawn_ = 0.0;
    Spire* newSpire = new Spire(context_, masterControl_, position);
    spires_[newSpire->rootNode_->GetID()] = SharedPtr<Spire>(newSpire);
}
