#ifndef HEADER_gubg_smooth_Quadratic_hpp_ALREADY_INCLUDED
#define HEADER_gubg_smooth_Quadratic_hpp_ALREADY_INCLUDED

#include <gubg/History.hpp>
#include <gubg/Matrix.hpp>
#include <gubg/hr.hpp>
#include <array>

namespace gubg { namespace smooth { 

	template <typename T>
	class Quadratic
	{
	public:
		Quadratic() {setup(1);}

		bool setup(unsigned int order)
		{
			if (order < 1)
				return false;

			order_ = order;
			const auto size = 2*order_+1;
			history_.resize(size);

			x_.resize(3, size);
			auto pow0 = x_.row(0);
			auto pow1 = x_.row(1);
			auto pow2 = x_.row(2);
			T ss{}, ssss{};
			for (auto ix = 0; ix < size; ++ix)
			{
				const auto x = (T(order_)-ix)/order_;
				pow0[ix] = 1;
				pow1[ix] = x;
				const auto square = x*x;
				pow2[ix] = square;
				ss += square;
				ssss += square*square;
			}

			const T det = size*ssss-ss*ss;

			// Ordinary least squares for quadratic curves
			// Independent variables: 1, X, X*X with X in [-1,1], size values
			// Coeffs = inv(Xt.X).(Xt.y)
			inv_xtx_.resize(3, 3);
			inv_xtx_.set(0,0, ssss/det);
			inv_xtx_.set(2,0, -ss/det);
			inv_xtx_.set(1,1, 1/ss);
			inv_xtx_.set(0,2, -ss/det);
			inv_xtx_.set(2,2, size/det);

			std::fill(abc_.data(), abc_.data()+abc_.size(), T{});

			return true;
		}

		unsigned int latency() const {return order_;}

		T value() const {return abc_[0];}
		T derivative() const {return abc_[1];}
		T curvature() const {return 2*abc_[2];}

		T operator()(T v)
		{
			history_.push_pop(v);
			auto range = history_.range();
			x_.multiply(xy_, range);
			inv_xtx_.multiply(abc_, xy_);
			return value();
		}

	private:
		unsigned int order_;
		History<T> history_;
		Matrix<T> x_;
		Matrix<T> inv_xtx_;
		std::array<T, 3> xy_;
		std::array<T, 3> abc_;
	};

} } 

#endif