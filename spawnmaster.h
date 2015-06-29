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

#pragma once

#include <Urho3D/Urho3D.h>
#include <Urho3D/Scene/Scene.h>

#include "mastercontrol.h"
#include "razor.h"
#include "spire.h"
#include "bullet.h"

class SpawnMaster : public Object
{
    OBJECT(SpawnMaster);
public:
    SpawnMaster(Context *context, MasterControl *masterControl);

    HashMap<unsigned, SharedPtr<Razor> > razors_;
    HashMap<unsigned, SharedPtr<Spire> > spires_;
    Vector<SharedPtr<Bullet> > bullets_;

    Vector3 CreateSpawnPoint();

    Bullet *SpawnBullet(Vector3 position);
    Bullet *RespawnBullet();
    int CountActiveRazors();
    int CountActiveSpires();
private:
    MasterControl* masterControl_;
    void HandleSceneUpdate(StringHash eventType, VariantMap &eventData);

    double razorInterval_;
    double sinceRazorSpawn_;

    double spireInterval_;
    double sinceSpireSpawn_;

    void SpawnRazor(Vector3 position);
    bool RespawnRazor(Vector3 position);
    void SpawnSpire(Vector3 position);
    bool RespawnSpire(Vector3 position);
};
