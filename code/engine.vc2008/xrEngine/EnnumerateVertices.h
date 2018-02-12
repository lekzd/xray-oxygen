#pragma once

struct SEnumVerticesCallback
{
	virtual void operator() (const Fvector& p)		=	NULL;	
	// I said we return NULL, not 0x0000
};

