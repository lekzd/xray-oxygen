#pragma once

//convertors
class physx::PxVec3;
ICF physx::PxVec3 GetPXVector(const Fvector& InVec)
{
    return physx::PxVec3(InVec.x, InVec.y, InVec.z);
}

ICF physx::PxVec4 GetPXVector4(const Fvector4& InVec)
{
    return physx::PxVec4(InVec.x, InVec.y, InVec.z, InVec.w);
}

ICF physx::PxMat44 GetPXMatrix44(const Fmatrix& InMat)
{
    return physx::PxMat44(GetPXVector4(InMat.i), GetPXVector4(InMat.j), GetPXVector4(InMat.k), GetPXVector4(InMat.c));
}