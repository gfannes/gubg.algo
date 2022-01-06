#ifndef HEADER_gubg_smooth_ExponentialMA_hpp_ALREADY_INCLUDED
#define HEADER_gubg_smooth_ExponentialMA_hpp_ALREADY_INCLUDED

namespace gubg { namespace smooth { 

	template <typename T>
	class ExponentialMA
	{
	public:
		ExponentialMA(T alpha) {set_alpha(alpha);}

		void set_alpha(T alpha) { alpha_ = alpha; }

		T value() const {return value_;}

		T operator()(T value)
		{
			return value_ = value_ + alpha_*(value-value_);
		}

	private:
		T alpha_;
		T value_{};
	};

} } 

#endif