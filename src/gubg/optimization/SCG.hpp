#ifndef HEADER_gubg_optimization_SCG_hpp_ALREADY_INCLUDED
#define HEADER_gubg_optimization_SCG_hpp_ALREADY_INCLUDED

namespace gubg { namespace optimization { 

    namespace details { 
        struct Default
        {
        };
    } 

    template <typename Float, typename Param, typename Outer = details::Default>
    class SCG
    {
    public:
        SCG(): outer_(default_outer_) {}
        SCG(Outer &outer): outer_(outer) {}

        void operator()()
        {
            sigma_ = 0.001;
            k_ = 0;
        }

    private:
        Float sigma_;
        unsigned int k_;
        typename Param::Type w_;

        details::Default default_outer_;
        Outer &outer_;
    };

} } 

#endif
