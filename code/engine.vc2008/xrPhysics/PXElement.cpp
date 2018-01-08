#include "stdafx.h"
#include "PXElement.h"
#include "PhysX/PXWorld.h"
#include "PhysX/PhysX_helpers.h"
#include "PHDynamicData.h"

PXElement::PXElement()
{
    m_flags.assign(0);
}

void PXElement::Fix()
{
    //flag
    m_flags.set(flFixed, TRUE);

    //get mass for fixed body

    //set new mass

    //disable gravity

    //zeroing all velocities
}

CPHFracture & PXElement::Fracture(u16 num)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::GetGlobalTransformDynamic(Fmatrix* m) const
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::GetPointVel(Fvector &res_vel, const Fvector & point) const
{
    throw std::logic_error("The method or operation is not implemented.");
}

IPhysicsShellEx * PXElement::PhysicsShell()
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::ReleaseFixed()
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::add_Mass(const SBoneShape& shape, const Fmatrix& offset, const Fvector& mass_center, float mass, CPHFracture* fracture /*= NULL*/)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::addInertia(const dMass& M)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::applyImpulseTrace(const Fvector& pos, const Fvector& dir, float val, const u16 id)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::applyImpulseVsGF(const Fvector& pos, const Fvector& dir, float val)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::applyImpulseVsMC(const Fvector& pos, const Fvector& dir, float val)
{
    throw std::logic_error("The method or operation is not implemented.");
}

CODEGeom* PXElement::geometry(u16 i)
{
    throw std::logic_error("The method or operation is not implemented.");
}

const IPhysicsGeometry* PXElement::geometry(u16 i) const
{
    throw std::logic_error("The method or operation is not implemented.");
}

dMass * PXElement::getMassTensor()
{
    throw std::logic_error("The method or operation is not implemented.");
}

bool PXElement::isFixed()
{
    throw std::logic_error("The method or operation is not implemented.");
}

const Fvector & PXElement::mass_Center() const
{
    throw std::logic_error("The method or operation is not implemented.");
}

IPhysicsElementEx* PXElement::parent_element()
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::set_BoxMass(const Fobb& box, float mass)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::set_ParentElement(IPhysicsElementEx* p)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::set_local_mass_center(const Fvector &mc)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::setDensityMC(float M, const Fvector& mass_center)
{
    throw std::logic_error("The method or operation is not implemented.");
}

u16 PXElement::setGeomFracturable(CPHFracture &fracture)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::setInertia(const dMass& M)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::setMassMC(float M, const Fvector& mass_center)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::setQuaternion(const Fquaternion& quaternion)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::Activate(bool disable /*= false*/, bool not_set_bone_callbacks /*= false*/)
{
    Fvector lvel, avel;
    lvel.set(0.f, 0.f, 0.f);
    avel.set(0.f, 0.f, 0.f);
    Activate(mXFORM, lvel, avel, disable);
}

void PXElement::Activate(const Fmatrix &transform, const Fvector& lin_vel, const Fvector& ang_vel, bool disable /*= false*/)
{
    m_XFORM = transform;
    physx::PxMat44 XTransform = GetPXMatrix44(m_XFORM);
    physx::PxTransform BodyTransform(XTransform);
    
    m_body = inl_px_world().Physics().createRigidDynamic(BodyTransform);

    //set mass

    //set geoms

    //set sky

    //set allah

    if (disable)
    {
        //#TODO: There is another purpose for this function, reimplement later
        m_body->putToSleep();
    }
    else
    {
        m_body->setLinearVelocity(GetPXVector(lin_vel));
        m_body->setAngularVelocity(GetPXVector(ang_vel));
    }

    m_flags.set(flActive, TRUE);
    m_flags.set(flActivating, TRUE);
}

void PXElement::Activate(const Fmatrix& form, bool disable /*= false*/)
{
    VERIFY(_valid(form));
    Fmatrix globe;
    globe.mul_43(form, mXFORM);

    Fvector lvel, avel;
    lvel.set(0.f, 0.f, 0.f);
    avel.set(0.f, 0.f, 0.f);
    Activate(globe, lvel, avel, disable);
}

void PXElement::Activate(const Fmatrix& m0, float dt01, const Fmatrix& m2, bool disable /*= false*/)
{
    Fvector lvel, avel;
    lvel.set(m2.c.x - m0.c.x, m2.c.y - m0.c.y, m2.c.z - m0.c.z);
    avel.set(0.f, 0.f, 0.f);
    Activate(m0, lvel, avel, disable);
}

void PXElement::Deactivate()
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::Enable()
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::GetAirResistance(float &linear, float &angular)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::GetGlobalPositionDynamic(Fvector* v)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::InterpolateGlobalPosition(Fvector* v)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void _BCL PXElement::InterpolateGlobalTransform(Fmatrix* m)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::SetAirResistance(float linear /*= default_k_l*/, float angular /*= default_k_w*/)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::SetAnimated(bool v)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::SetTransform(const Fmatrix& m0, motion_history_state history_state)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::TransformPosition(const Fmatrix &form, motion_history_state history_state)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::applyForce(const Fvector& dir, float val)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::applyForce(float x, float y, float z)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void _BCL PXElement::applyGravityAccel(const Fvector& accel)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::applyImpulse(const Fvector& dir, float val)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::dbg_draw_force(float scale, u32 color)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::dbg_draw_geometry(float scale, u32 color, Flags32 flags /*= Flags32().assign(0)*/) const
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::dbg_draw_velocity(float scale, u32 color)
{
    throw std::logic_error("The method or operation is not implemented.");
}

bool PXElement::get_ApplyByGravity()
{
    throw std::logic_error("The method or operation is not implemented.");
}

float PXElement::getDensity()
{
    throw std::logic_error("The method or operation is not implemented.");
}

float PXElement::getMass()
{
    throw std::logic_error("The method or operation is not implemented.");
}

float PXElement::getVolume()
{
    throw std::logic_error("The method or operation is not implemented.");
}

bool PXElement::isActive() const
{
    throw std::logic_error("The method or operation is not implemented.");
}

bool PXElement::isBreakable()
{
    throw std::logic_error("The method or operation is not implemented.");
}

bool PXElement::isEnabled() const
{
    throw std::logic_error("The method or operation is not implemented.");
}

bool PXElement::isFullActive() const
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::set_AngularVel(const Fvector& velocity)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::set_ApplyByGravity(bool flag)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::set_DynamicLimits(float l_limit /*= default_l_limit*/, float w_limit /*= default_w_limit*/)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::set_DynamicScales(float l_scale /*= default_l_scale*/, float w_scale /*= default_w_scale*/)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::set_LinearVel(const Fvector& velocity)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::setDensity(float M)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::setForce(const Fvector& force)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::setMass(float M)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::setTorque(const Fvector& torque)
{
    throw std::logic_error("The method or operation is not implemented.");
}

iphysics_scripted & PXElement::get_scripted()
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::get_AngularVel(Fvector& velocity) const
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::get_LinearVel(Fvector& velocity) const
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::cv2bone_Xfrom(const Fquaternion& q, const Fvector& pos, Fmatrix& xform)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::cv2obj_Xfrom(const Fquaternion& q, const Fvector& pos, Fmatrix& xform)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::get_State(SPHNetState& state)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::set_State(const SPHNetState& state)
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::Disable()
{
    throw std::logic_error("The method or operation is not implemented.");
}

void PXElement::ReEnable()
{
    throw std::logic_error("The method or operation is not implemented.");
}

dBodyID PXElement::get_body()
{
    throw std::logic_error("The method or operation is not implemented.");
}

