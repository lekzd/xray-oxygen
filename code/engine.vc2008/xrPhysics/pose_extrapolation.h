#pragma once

#include "../xrphysics/CycleConstStorage.h"
namespace extrapolation
{
	class XRPHYSICS_API pose
	{
		Fvector p;
		Fquaternion 	r;
	public:
		pose & set(const Fmatrix &m);
		Fmatrix &get(Fmatrix &m)		const;
		pose&	mul(float v);
		pose&	add(const pose &pose_);
		pose&	invert();
		pose&	identity();
		pose();
	};

	class XRPHYSICS_API point
	{
	public:
		point() : m_time(-FLT_MAX) { }
		inline const pose&	pose()	const { return p; }
		inline		 float	time()	const { return m_time; }
		inline		 point&	set(const extrapolation::pose &p_, float time) { p = p_; m_time = time; return *this; }
	private:
		extrapolation::pose		p;
		float		m_time;
	};

	class XRPHYSICS_API points
	{
	public:
		points() : last_update(u32(-1)) {}
		void	update(const Fmatrix &m);
		void	init(const Fmatrix &m);
		void	extrapolate(Fmatrix &m, float time) const;
	private:

	private:

	private:
		static const u16 point_num = 2;

		typedef CCycleConstStorage<extrapolation::point, point_num> container;
	private:
		u32			last_update;
		container	m_points;
	};
}