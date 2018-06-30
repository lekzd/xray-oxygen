#pragma once
#include "Geometry.h"
#include "phdefs.h"
#include "PhysicsCommon.h"
#include "../xrserverentities/PHSynchronize.h"
#include "PHDisabling.h"
#include "PHGeometryOwner.h"
#include "PHInterpolation.h"
#include "PHFracture.h"
#include "physics_scripted.h"
#include "PhysicsShell.h"

class physx::PxRigidDynamic;

class PXElement :
    public	IPhysicsElementEx,
    public	CPHSynchronize,
    public	CPHDisablingFull,
    public	CPHGeometryOwner,
    public	cphysics_scripted
{
    physx::PxRigidDynamic* m_body = nullptr;
    Fmatrix m_XFORM;
    Flags8						m_flags;					//
    enum
    {
        flActive = 1 << 0,
        flActivating = 1 << 1,
        flUpdate = 1 << 2,
        flWasEnabledBeforeFreeze = 1 << 3,
        flEnabledOnStep = 1 << 4,
        flFixed = 1 << 5,
        flAnimated = 1 << 6
    };

public:
    PXElement();
    virtual void Fix() override;
    virtual CPHFracture & Fracture(u16 num) override;
    virtual void GetGlobalTransformDynamic(Fmatrix* m) const override;
    virtual void GetPointVel(Fvector &res_vel, const Fvector & point) const override;
    virtual IPhysicsShellEx * PhysicsShell() override;
    virtual void ReleaseFixed() override;
    virtual void add_Mass(const SBoneShape& shape, const Fmatrix& offset, const Fvector& mass_center, float mass, CPHFracture* fracture = NULL) override;
    virtual void addInertia(const dMass& M) override;
    virtual void applyImpulseTrace(const Fvector& pos, const Fvector& dir, float val, const u16 id) override;
    virtual void applyImpulseVsGF(const Fvector& pos, const Fvector& dir, float val) override;
    virtual void applyImpulseVsMC(const Fvector& pos, const Fvector& dir, float val) override;
    virtual CODEGeom* geometry(u16 i) override;
    virtual const IPhysicsGeometry* geometry(u16 i) const override;
    virtual dMass * getMassTensor() override;
    virtual bool isFixed() override;
    virtual const Fvector & mass_Center() const override;
    virtual IPhysicsElementEx* parent_element() override;
    virtual void set_BoxMass(const Fobb& box, float mass) override;
    virtual void set_ParentElement(IPhysicsElementEx* p) override;
    virtual void set_local_mass_center(const Fvector &mc) override;
    virtual void setDensityMC(float M, const Fvector& mass_center) override;
    virtual u16 setGeomFracturable(CPHFracture &fracture) override;
    virtual void setInertia(const dMass& M) override;
    virtual void setMassMC(float M, const Fvector& mass_center) override;
    virtual void setQuaternion(const Fquaternion& quaternion) override;
    virtual void Activate(bool disable = false, bool not_set_bone_callbacks = false) override;
    virtual void Activate(const Fmatrix &transform, const Fvector& lin_vel, const Fvector& ang_vel, bool disable = false) override;
    virtual void Activate(const Fmatrix& form, bool disable = false) override;
    virtual void Activate(const Fmatrix& m0, float dt01, const Fmatrix& m2, bool disable = false) override;
    virtual void Deactivate() override;
    virtual void Enable() override;
    virtual void GetAirResistance(float &linear, float &angular) override;
    virtual void GetGlobalPositionDynamic(Fvector* v) override;
    virtual void InterpolateGlobalPosition(Fvector* v) override;
    virtual void _BCL InterpolateGlobalTransform(Fmatrix* m) override;
    virtual void SetAirResistance(float linear = default_k_l, float angular = default_k_w) override;
    virtual void SetAnimated(bool v) override;
    virtual void SetTransform(const Fmatrix& m0, motion_history_state history_state) override;
    virtual void TransformPosition(const Fmatrix &form, motion_history_state history_state) override;
    virtual void applyForce(const Fvector& dir, float val) override;
    virtual void applyForce(float x, float y, float z) override;
    virtual void _BCL applyGravityAccel(const Fvector& accel) override;
    virtual void applyImpulse(const Fvector& dir, float val) override;
    virtual void dbg_draw_force(float scale, u32 color) override;
    virtual void dbg_draw_geometry(float scale, u32 color, Flags32 flags = Flags32().assign(0)) const override;
    virtual void dbg_draw_velocity(float scale, u32 color) override;
    virtual bool get_ApplyByGravity() override;
    virtual float getDensity() override;
    virtual float getMass() override;
    virtual float getVolume() override;
    virtual bool isActive() const override;
    virtual bool isBreakable() override;
    virtual bool isEnabled() const override;
    virtual bool isFullActive() const override;
    virtual void set_AngularVel(const Fvector& velocity) override;
    virtual void set_ApplyByGravity(bool flag) override;
    virtual void set_DynamicLimits(float l_limit = default_l_limit, float w_limit = default_w_limit) override;
    virtual void set_DynamicScales(float l_scale = default_l_scale, float w_scale = default_w_scale) override;
    virtual void set_LinearVel(const Fvector& velocity) override;
    virtual void setDensity(float M) override;
    virtual void setForce(const Fvector& force) override;
    virtual void setMass(float M) override;
    virtual void setTorque(const Fvector& torque) override;
    virtual iphysics_scripted & get_scripted() override;
    virtual void get_AngularVel(Fvector& velocity) const override;
    virtual void get_LinearVel(Fvector& velocity) const override;
    virtual void cv2bone_Xfrom(const Fquaternion& q, const Fvector& pos, Fmatrix& xform) override;
    virtual void cv2obj_Xfrom(const Fquaternion& q, const Fvector& pos, Fmatrix& xform) override;
    virtual void get_State(SPHNetState& state) override;
    virtual void set_State(const SPHNetState& state) override;

protected:
    virtual void Disable() override;
    virtual void ReEnable() override;
    virtual dBodyID get_body() override;
    };