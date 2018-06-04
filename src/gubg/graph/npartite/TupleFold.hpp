#ifndef HEADER_gubg_graph_npartite_TupleFold_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_npartite_TupleFold_hpp_ALREADY_INCLUDED

#include <utility>

namespace gubg { namespace graph { namespace npartite {

    template <typename T, template<T> class Transform, typename TypeList> struct TupleFold;
    template <typename T, template<T> class Transform, T ... Elements> struct TupleFold<T, Transform, std::integer_sequence<T, Elements...>>
    {
        typedef std::tuple<Transform<Elements>...> type;
    };
    

} } }

#endif

