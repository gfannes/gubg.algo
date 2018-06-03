#ifndef HEADER_gubg_graph_NPartite_TupleFold_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_NPartite_TupleFold_hpp_ALREADY_INCLUDED

#include <utility>

namespace gubg { namespace graph { namespace NPartite {

    template <typename T, template<T> class Transform, typename TypeList> struct TupleFold;
    template <typename T, template<T> class Transform, T ... Elements> struct TupleFold<T, Transform, std::integer_sequence<T, Elements...>>
    {
        typedef std::tuple<typename Transform<Elements>::type...> type;
    };
    

} } }

#endif

