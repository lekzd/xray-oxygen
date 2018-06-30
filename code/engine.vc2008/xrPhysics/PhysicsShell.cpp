#include "stdafx.h"
#pragma hdrstop
#include "physicsshell.h"
#include "PHDynamicData.h"
#include "Physics.h"
#include "PHJoint.h"
#include "PHShell.h"
#include "PHJoint.h"
#include "PHJointDestroyInfo.h"
#include "PHSplitedShell.h"

#include "iphysicsshellholder.h"

#include "phvalide.h"

#include "../Include/xrRender/Kinematics.h"
#include "../xrengine/xr_object.h"
#include "../xrengine/bone.h"

extern CPHWorld *ph_world;
IPhysicsShellEx::~IPhysicsShellEx()
{
}

void ApplySpawnIniToPhysicShell( CInifile const * ini, IPhysicsShellEx* physics_shell, bool fixed )
{
	if (!ini)
		return;
	if (ini->section_exist("physics_common"))
	{
		fixed = fixed || (ini->line_exist("physics_common", "fixed_bones"));
#pragma todo("not ignore static if non realy fixed! ")
		fix_bones(ini->r_string("physics_common", "fixed_bones"), physics_shell);
	}
	if (ini->section_exist("collide"))
	{
		if ((ini->line_exist("collide", "ignore_static") && fixed) || (ini->line_exist("collide", "ignore_static") && ini->section_exist("animated_object")))
		{
			physics_shell->SetIgnoreStatic();
		}
		if (ini->line_exist("collide", "small_object"))
		{
			physics_shell->SetSmall();
		}
		if (ini->line_exist("collide", "ignore_small_objects"))
		{
			physics_shell->SetIgnoreSmall();
		}
		if (ini->line_exist("collide", "ignore_ragdoll"))
		{
			physics_shell->SetIgnoreRagDoll();
		}

		//If need, then show here that it is needed to ignore collisions with "animated_object"
		if (ini->line_exist("collide", "ignore_animated_objects"))
		{
			physics_shell->SetIgnoreAnimated();
		}
	}
	//If next section is available then given "PhysicShell" is classified
	//as animated and we read options for his animation

	if (ini->section_exist("animated_object"))
	{
		//Show that given "PhysicShell" animated
		physics_shell->CreateShellAnimator(ini, "animated_object");
	}
}

void	get_box(const IPhysicsBase*	shell, const	Fmatrix& form, Fvector&	sz, Fvector&	c)
{
	t_get_box(shell, form, sz, c);
}

void __stdcall	destroy_physics_shell(IPhysicsShellEx* &p)
{
	if (p)
		p->Deactivate();
	xr_delete(p);
}

bool bone_has_pysics(IKinematics& K, u16 bone_id)
{
	return K.LL_GetBoneVisible(bone_id) && shape_is_physic(K.GetBoneData(bone_id).get_shape());
}

bool has_physics_collision_shapes(IKinematics& K)
{
	u16 nbb = K.LL_BoneCount();
	for (u16 i = 0; i < nbb; ++i)
		if (bone_has_pysics(K, i))
			return true;
	return false;
}

float NonElasticCollisionEnergy(IPhysicsElementEx *e1, IPhysicsElementEx *e2, const Fvector &norm)// norm - from 2 to 1
{
	VERIFY(e1);
	VERIFY(e2);
	dBodyID b1 = static_cast<CPHElement*> (e1)->get_body();
	VERIFY(b1);
	dBodyID b2 = static_cast<CPHElement*> (e2)->get_body();
	VERIFY(b2);
	return E_NL(b1, b2, cast_fp(norm));
}

void	StaticEnvironmentCB(bool& do_colide, bool bo1, dContact& c, SGameMtl* material_1, SGameMtl* material_2)
{
	dJointID contact_joint = dJointCreateContact(0, ContactGroup, &c);

	if (bo1)
	{
		((CPHIsland*)(retrieveGeomUserData(c.geom.g1)->callback_data))->DActiveIsland()->ConnectJoint(contact_joint);
		dJointAttach(contact_joint, dGeomGetBody(c.geom.g1), 0);
	}
	else
	{
		((CPHIsland*)(retrieveGeomUserData(c.geom.g2)->callback_data))->DActiveIsland()->ConnectJoint(contact_joint);
		dJointAttach(contact_joint, 0, dGeomGetBody(c.geom.g2));
	}
	do_colide = false;
}