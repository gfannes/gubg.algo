#ifndef HEADER_gubg_graph_Traits_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_Traits_hpp_ALREADY_INCLUDED

#include "gubg/graph/Tag.hpp"

namespace gubg { namespace graph {

    template <typename Graph> struct Traits
    {
        using vertex_descriptor     = typename Graph::vertex_descriptor;
        using edge_descriptor       = typename Graph::edge_descriptor;
        using degree_type           = typename Graph::degree_type;
        using vertices_size_type    = typename Graph::vertices_size_type;
        using edges_size_type       = typename Graph::edges_size_type;
        using vertex_iterator       = typename Graph::vertex_iterator;
        using edge_iterator         = typename Graph::edge_iterator;
        using out_edge_iterator     = typename Graph::out_edge_iterator;
        using adjacent_iterator     = typename Graph::adjacent_iterator;
        using in_edge_iterator      = typename Graph::in_edge_iterator;
        using inv_adjacent_iterator = typename Graph::inv_adjacent_iterator;
        using direction_tag         = typename Graph::direction_tag;
        using vertex_label_type     = typename Graph::vertex_label_type;
        using edge_label_type       = typename Graph::edge_label_type;

        static constexpr bool has_direction = detail::EdgeInformation<direction_tag>::has_direction;
        static constexpr bool has_incoming_edge_info = detail::EdgeInformation<direction_tag>::has_incoming;
        static constexpr bool has_vertex_label = detail::LabelInfo<vertex_label_type>::has_label;
        static constexpr bool has_edge_label = detail::LabelInfo<edge_label_type>::has_label;
    };
} }

#endif

