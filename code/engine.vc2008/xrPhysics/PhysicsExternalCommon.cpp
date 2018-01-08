#include "stdafx.h"
#include "physicsexternalcommon.h"
#include "extendedgeom.h"
#include "mathutilsode.h"
#include "PhysicsShell.h"

//ODE
#include "PHElement.h"
#include "PHShell.h"
#include "PHSplitedShell.h"
#include "IPhysicsShellHolder.h"
#include "../Include/xrRender/Kinematics.h"
#include "../xrEngine/bone.h"
#include "phvalide.h"

xrPhysicsEngine g_PhysicsEngine = XPE_ODE;

bool ContactShotMarkGetEffectPars(dContactGeom* c, dxGeomUserData* &data, float &vel_cret, bool &b_invert_normal )
{
	dBodyID b=dGeomGetBody(c->g1);

	b_invert_normal=false;
	if(!b) 
	{
		b=dGeomGetBody(c->g2);
		data=dGeomGetUserData(c->g2);
		b_invert_normal=true;
	}
	else
	{
		data=dGeomGetUserData(c->g1);
	}
	if(!b) 
		return false;

	dVector3 vel;
	dMass m;
	dBodyGetMass(b,&m);
	dBodyGetPointVel(b,c->pos[0],c->pos[1],c->pos[2],vel);
	vel_cret=_abs(dDOT(vel,c->normal))* _sqrt(m.mass);
	return true;
}

IPhysicsElementEx*			P_create_Element()
{
    
    CPHElement* element = xr_new<CPHElement>();
    return element;
}

IPhysicsShellEx*				P_create_Shell()
{
    IPhysicsShellEx* shell = xr_new<CPHShell>();
    return shell;
}

IPhysicsShellEx*				P_create_splited_Shell()
{
    IPhysicsShellEx* shell = xr_new<CPHSplitedShell>();
    return shell;
}

IPhysicsJoint*				P_create_Joint(IPhysicsJoint::enumType type, IPhysicsElementEx* first, IPhysicsElementEx* second)
{
    IPhysicsJoint* joint = xr_new<CPHJoint>(type, first, second);
    return joint;
}


IPhysicsShellEx*	__stdcall P_build_Shell(IPhysicsShellHolder* obj, bool not_active_state, BONE_P_MAP* bone_map, bool not_set_bone_callbacks)
{
    VERIFY(obj);
    phys_shell_verify_object_model(*obj);
    //IRenderVisual*	V = obj->ObjectVisual();
    //IKinematics* pKinematics=smart_cast<IKinematics*>(V);
    //IKinematics* pKinematics	=  V->dcast_PKinematics			();
    IKinematics* pKinematics = obj->ObjectKinematics();

    IPhysicsShellEx* pPhysicsShell = P_create_Shell();
#ifdef DEBUG
    pPhysicsShell->dbg_obj = obj;
#endif
    pPhysicsShell->build_FromKinematics(pKinematics, bone_map);

    pPhysicsShell->set_PhysicsRefObject(obj);
    pPhysicsShell->mXFORM.set(obj->ObjectXFORM());
    pPhysicsShell->Activate(not_active_state, not_set_bone_callbacks);//,
                                                                      //m_pPhysicsShell->SmoothElementsInertia(0.3f);
    pPhysicsShell->SetAirResistance();//0.0014f,1.5f

    return pPhysicsShell;
}

void	fix_bones(LPCSTR	fixed_bones, IPhysicsShellEx* shell)
{
    VERIFY(fixed_bones);
    VERIFY(shell);
    IKinematics	*pKinematics = shell->PKinematics();
    VERIFY(pKinematics);
    int count = _GetItemCount(fixed_bones);
    for (int i = 0; i < count; ++i)
    {
        string64					fixed_bone;
        _GetItem(fixed_bones, i, fixed_bone);
        u16 fixed_bone_id = pKinematics->LL_BoneID(fixed_bone);
        R_ASSERT2(BI_NONE != fixed_bone_id, "wrong fixed bone");
        IPhysicsElementEx* E = shell->get_Element(fixed_bone_id);
        if (E)
            E->Fix();
    }
}
IPhysicsShellEx*	P_build_Shell(IPhysicsShellHolder* obj, bool not_active_state, BONE_P_MAP* p_bone_map, LPCSTR	fixed_bones)
{
    IPhysicsShellEx* pPhysicsShell = 0;
    //IKinematics* pKinematics=smart_cast<IKinematics*>(obj->ObjectVisual());
    IKinematics* pKinematics = obj->ObjectKinematics();
    if (fixed_bones)
    {


        int count = _GetItemCount(fixed_bones);
        for (int i = 0; i < count; ++i)
        {
            string64					fixed_bone;
            _GetItem(fixed_bones, i, fixed_bone);
            u16 fixed_bone_id = pKinematics->LL_BoneID(fixed_bone);
            R_ASSERT2(BI_NONE != fixed_bone_id, "wrong fixed bone");
            p_bone_map->insert(std::make_pair(fixed_bone_id, physicsBone()));
        }

        pPhysicsShell = P_build_Shell(obj, not_active_state, p_bone_map);

        //m_pPhysicsShell->add_Joint(P_create_Joint(CPhysicsJoint::enumType::full_control,0,fixed_element));
    }
    else
        pPhysicsShell = P_build_Shell(obj, not_active_state);


    auto i = p_bone_map->begin(), e = p_bone_map->end();
    if (i != e) pPhysicsShell->SetPrefereExactIntegration();
    for (; i != e; i++)
    {
        IPhysicsElementEx* fixed_element = i->second.element;
        R_ASSERT2(fixed_element, "fixed bone has no physics");
        //if(!fixed_element) continue;
        fixed_element->Fix();
    }
    return pPhysicsShell;
}

IPhysicsShellEx*	P_build_Shell(IPhysicsShellHolder* obj, bool not_active_state, LPCSTR	fixed_bones)
{
    U16Vec f_bones;
    if (fixed_bones) {
        //IKinematics* K		= smart_cast<IKinematics*>(obj->ObjectVisual());
        IKinematics* K = obj->ObjectKinematics();
        VERIFY(K);
        int count = _GetItemCount(fixed_bones);
        for (int i = 0; i < count; ++i) {
            string64		fixed_bone;
            _GetItem(fixed_bones, i, fixed_bone);
            f_bones.push_back(K->LL_BoneID(fixed_bone));
            R_ASSERT2(BI_NONE != f_bones.back(), "wrong fixed bone");
        }
    }
    return P_build_Shell(obj, not_active_state, f_bones);
}

static BONE_P_MAP bone_map = BONE_P_MAP();
IPhysicsShellEx*	P_build_Shell(IPhysicsShellHolder* obj, bool not_active_state, U16Vec& fixed_bones)
{
    bone_map.clear();
    IPhysicsShellEx*			pPhysicsShell = 0;
    if (!fixed_bones.empty())
        for (auto it = fixed_bones.begin(); it != fixed_bones.end(); it++)
            bone_map.insert(std::make_pair(*it, physicsBone()));
    pPhysicsShell = P_build_Shell(obj, not_active_state, &bone_map);

    // fix bones
    auto i = bone_map.begin(), e = bone_map.end();
    if (i != e)
        pPhysicsShell->SetPrefereExactIntegration();
    for (; i != e; i++)
    {
        IPhysicsElementEx* fixed_element = i->second.element;
        //R_ASSERT2(fixed_element,"fixed bone has no physics");
        if (!fixed_element) continue;
        fixed_element->Fix();
    }
    return pPhysicsShell;
}

IPhysicsShellEx*	P_build_SimpleShell(IPhysicsShellHolder* obj, float mass, bool not_active_state)
{
    IPhysicsShellEx* pPhysicsShell = P_create_Shell();
#ifdef DEBUG
    pPhysicsShell->dbg_obj = (obj);
#endif
    //Fobb obb; obj->ObjectVisual()->getVisData().box.get_CD( obb.m_translate, obb.m_halfsize );
    VERIFY(obj);
    VERIFY(obj->ObjectKinematics());

    Fobb obb; obj->ObjectKinematics()->GetBox().get_CD(obb.m_translate, obb.m_halfsize);
    obb.m_rotate.identity();
    IPhysicsElementEx* E = P_create_Element(); R_ASSERT(E); E->add_Box(obb);
    pPhysicsShell->add_Element(E);
    pPhysicsShell->setMass(mass);
    pPhysicsShell->set_PhysicsRefObject(obj);
    if (!obj->has_parent_object())
        pPhysicsShell->Activate(obj->ObjectXFORM(), 0, obj->ObjectXFORM(), not_active_state);
    return pPhysicsShell;
}


void	phys_shell_verify_model(IKinematics& K)
{
    //IRenderVisual* V = K.dcast_RenderVisual();
    //VERIFY( V );
    VERIFY2(has_physics_collision_shapes(K), make_string("Can not create physics shell for model %s because it has no physics collision shapes set", K.getDebugName().c_str()));
}

void	phys_shell_verify_object_model(IPhysicsShellHolder& O)
{
    //IRenderVisual	*V = O.ObjectVisual();

    //VERIFY2( V, make_string( "Can not create physics shell for object %s it has no model", O.ObjectName() )/*+ make_string("\n object dump: \n") + dbg_object_full_dump_string( &O )*/ );

    //IKinematics		*K = V->dcast_PKinematics();

    IKinematics* K = O.ObjectKinematics();

    VERIFY2(K, make_string("Can not create physics shell for object %s, model %s is not skeleton", O.ObjectName(), O.ObjectNameVisual()));

    VERIFY2(has_physics_collision_shapes(*K), make_string("Can not create physics shell for object %s, model %s has no physics collision shapes set", O.ObjectName(), O.ObjectNameVisual())/*+ make_string("\n object dump: \n") + dbg_object_full_dump_string( &O )*/);

    VERIFY2(_valid(O.ObjectXFORM()), make_string("create physics shell: object matrix is not valid") /*+ make_string("\n object dump: \n") + dbg_object_full_dump_string( &O )*/);

    VERIFY2(valid_pos(O.ObjectXFORM().c), dbg_valide_pos_string(O.ObjectXFORM().c, &O, "create physics shell"));
}

bool __stdcall	can_create_phys_shell(string1024 &reason, IPhysicsShellHolder& O)
{
    xr_strcpy(reason, "ok");
    bool result = true;
    IKinematics* K = O.ObjectKinematics();
    if (!K)
    {
        xr_strcpy(reason, make_string("Can not create physics shell for object %s, model %s is not skeleton", O.ObjectName(), O.ObjectNameVisual()).c_str());
        return false;
    }
    if (!has_physics_collision_shapes(*K))
    {
        xr_strcpy(reason, make_string("Can not create physics shell for object %s, model %s has no physics collision shapes set", O.ObjectName(), O.ObjectNameVisual()).c_str());
        return false;
    }
    if (!_valid(O.ObjectXFORM()))
    {
        xr_strcpy(reason, make_string("create physics shell: object matrix is not valid").c_str());
        return false;
    }
    if (!valid_pos(O.ObjectXFORM().c))
    {
#ifdef	DEBUG
        xr_strcpy(reason, dbg_valide_pos_string(O.ObjectXFORM().c, &O, "create physics shell").c_str());
#else
        xr_strcpy(reason, make_string("create physics shell: object position is not valid").c_str());
#endif
        return false;
    }
    return result;
}
