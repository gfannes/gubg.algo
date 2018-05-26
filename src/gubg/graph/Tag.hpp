#ifndef HEADER_gubg_graph_Tag_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_Tag_hpp_ALREADY_INCLUDED

namespace gubg { namespace graph {

    struct no_label {};
    struct use_vector {};
    struct use_list {};
    struct undirected {};
    struct directed {};
    struct bidirectional {};

    namespace detail {

        template <typename Tag> struct EdgeInformation;
        template <> struct EdgeInformation<undirected>
        {
            static constexpr bool has_direction = false;
            static constexpr bool has_incoming = true;
        };
        template <> struct EdgeInformation<directed>
        {
            static constexpr bool has_direction = true;
            static constexpr bool has_incoming = false;
        };
        template <> struct EdgeInformation<bidirectional>
        {
            static constexpr bool has_direction = true;
            static constexpr bool has_incoming = true;
        };

        template <typename Label> struct LabelInfo
        {
            static constexpr bool has_label = true;
        };
        template <> struct LabelInfo<no_label>
        {
            static constexpr bool has_label = false;
        };
    }

} }

#endif

