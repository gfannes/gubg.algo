#ifndef HEADER_gubg_order1_ExponentialMA_hpp_ALREADY_INCLUDED
#define HEADER_gubg_order1_ExponentialMA_hpp_ALREADY_INCLUDED

namespace gubg { namespace order1 { 

	template <typename T>
	class ExponentialMA
	{
	public:
		ExponentialMA(T alpha): alpha_(alpha) {}

		T value() const {return value_;}

		T operator()(T value)
		{
			return value_ = one_minus_alpha_*value_ + alpha_*value;
		}

	private:
		T alpha_ = 0;
		T one_minus_alpha_ = 1.0-alpha_;
		T value_{};
	};

} } 

#endif