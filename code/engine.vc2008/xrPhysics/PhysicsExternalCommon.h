#pragma once
struct dContactGeom;
struct dContact;
struct SGameMtl;

namespace CDB
{
	class TRI;
}

enum xrPhysicsEngine
{
    XPE_ODE,
    XPE_PHYSX
};

extern xrPhysicsEngine g_PhysicsEngine;

typedef void 	ContactCallbackFun(CDB::TRI* T,dContactGeom* c);
typedef	void	ObjectContactCallbackFun(bool& do_colide,bool bo1,dContact& c,SGameMtl* material_1,SGameMtl* material_2);
typedef void	BoneCallbackFun(CBoneInstance* B);
typedef void	PhysicsStepTimeCallback(u32 step_start,u32 step_end);

struct dxGeomUserData;
struct dContactGeom;
XRPHYSICS_API	bool ContactShotMarkGetEffectPars(dContactGeom *c, dxGeomUserData* &data, float &vel_cret, bool &b_invert_normal);

template <typename geom_type>
void t_get_box(const geom_type*	shell, const	Fmatrix& form, Fvector&	sz, Fvector&	c)
{
	c.set(0, 0, 0);
	VERIFY(sizeof(form.i) + sizeof(form._14_) == 4 * sizeof(float));
	for (int i = 0; 3 > i; ++i)
	{
		float lo, hi;
		const Fvector &ax = cast_fv(((const	float*)&form + i * 4));
		shell->get_Extensions(ax, 0, lo, hi);
		sz[i] = hi - lo; c.add(Fvector().mul(ax, (lo + hi) / 2));
	}
}

enum ERestrictionType
{
	rtStalker = 0,
	rtStalkerSmall,
	rtMonsterMedium,
	rtNone,
	rtActor
};
<<<<<<< HEAD
#endif
=======

#include "..\xrEngine\IPhysicsShell.h"
#include "PhysicsShell.h"

struct dContact;
struct SGameMtl;
XRPHYSICS_API void	StaticEnvironmentCB(bool& do_colide, bool bo1, dContact& c, SGameMtl* material_1, SGameMtl* material_2);

// Implementation creator
XRPHYSICS_API	IPhysicsJoint*				P_create_Joint(IPhysicsJoint::enumType type, IPhysicsElementEx* first, IPhysicsElementEx* second);
XRPHYSICS_API	IPhysicsElementEx*			P_create_Element();
XRPHYSICS_API	IPhysicsShellEx*				P_create_Shell();
XRPHYSICS_API	IPhysicsShellEx*				P_create_splited_Shell();
XRPHYSICS_API	IPhysicsShellEx*				P_build_Shell(IPhysicsShellHolder* obj, bool not_active_state, LPCSTR	fixed_bones);
XRPHYSICS_API	IPhysicsShellEx*				P_build_Shell(IPhysicsShellHolder* obj, bool not_active_state, U16Vec& fixed_bones);
XRPHYSICS_API	IPhysicsShellEx*				P_build_Shell(IPhysicsShellHolder* obj, bool not_active_state, BONE_P_MAP* bone_map, LPCSTR fixed_bones);

extern "C" XRPHYSICS_API	IPhysicsShellEx*		__stdcall		P_build_Shell(IPhysicsShellHolder* obj, bool not_active_state, BONE_P_MAP* bone_map = 0, bool not_set_bone_callbacks = false);

XRPHYSICS_API	IPhysicsShellEx*				P_build_SimpleShell(IPhysicsShellHolder* obj, float mass, bool not_active_state);
XRPHYSICS_API	void						ApplySpawnIniToPhysicShell(CInifile const * ini, IPhysicsShellEx* physics_shell, bool fixed);
void						fix_bones(LPCSTR	fixed_bones, IPhysicsShellEx* shell);

extern "C" XRPHYSICS_API	void	__stdcall			destroy_physics_shell(IPhysicsShellEx* &p);

extern "C" XRPHYSICS_API	bool	__stdcall			can_create_phys_shell(string1024 &reason, IPhysicsShellHolder& O);

struct	NearestToPointCallback
{
    virtual	bool operator() (IPhysicsElementEx* e) = 0;
};
bool				shape_is_physic(const SBoneShape& shape);
bool				has_physics_collision_shapes(IKinematics& K);
XRPHYSICS_API		void				phys_shell_verify_object_model(IPhysicsShellHolder& O);



void				phys_shell_verify_model(IKinematics& K);
>>>>>>> upstream/PhysX
