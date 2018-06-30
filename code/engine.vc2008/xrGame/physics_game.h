#pragma once
#include "../xrPhysics/PhysicsExternalCommon.h"
extern ContactCallbackFun *ContactShotMark;
extern ContactCallbackFun *CharacterContactShotMark;

void ShellApplyHit(const CPHShell* shell, const Fvector& pos, const Fvector& dir, float val, const u16 id, ALife::EHitType hit_type);