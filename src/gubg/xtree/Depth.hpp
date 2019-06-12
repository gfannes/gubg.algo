#ifndef HEADER_gubg_xtree_Depth_hpp_ALREADY_INCLUDED
#define HEADER_gubg_xtree_Depth_hpp_ALREADY_INCLUDED

#include <ostream>

namespace gubg { namespace xtree { 

    class Depth
    {
    public:
        //Depth before the first xlink
        unsigned int before_x = 0;
        //Depth after the first xlink
        unsigned int after_x = 0;

        unsigned int total() const {return before_x + after_x;}

        class Update
        {
        public:
            Update(Depth &depth, bool oc, bool as_child): oc_(oc), as_child_(as_child), count_(depth.after_x == 0 && as_child ? depth.before_x : depth.after_x)
            {
                if (oc_)
                    ++count_;
            }
            ~Update()
            {
                if (!oc_)
                    --count_;
            }
        private:
            const bool oc_;
            const bool as_child_;
            unsigned int &count_;
        };
    private:
        
    };
    inline std::ostream &operator<<(std::ostream &os, const Depth &depth)
    {
        os << "[depth](before:" << depth.before_x << ")(after:" << depth.after_x << ")";
        return os;
    }

} } 

#endif
