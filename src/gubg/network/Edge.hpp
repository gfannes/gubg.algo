#ifndef HEADER_gubg_network_Edge_hpp_ALREADY_INCLUDED
#define HEADER_gubg_network_Edge_hpp_ALREADY_INCLUDED

namespace gubg { namespace network { 

    template <typename T>
    struct Edge
    {
        T *from = nullptr;
        T *to = nullptr;
    };

} } 

#endif
