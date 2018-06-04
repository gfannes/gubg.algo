#ifndef HEADER_gubg_graph_npartite_Config_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_npartite_Config_hpp_ALREADY_INCLUDED

#include "gubg/graph/npartite/Forward.hpp"
#include "gubg/graph/npartite/TupleFold.hpp"
#include "gubg/graph/npartite/Label.hpp"
#include "gubg/graph/container_dispatch/Dispatch.hpp"

namespace gubg { namespace graph { namespace npartite {


    template <typename D, std::size_t ... P>
    struct Descriptor
    {
        Descriptor(D d = D()) : d_(d) {}
        operator D &() { return d_; }
        operator const D & () const { return d_; }
        D d_;
    };

    template <std::size_t ... P>
    struct DescriptorTransform
    {
        template <typename D> struct Apply
        {
            using type = Descriptor<D, P...>;
        };
    };

    template <typename Config, std::size_t SRC, std::size_t TGT> struct DefaultOutEdgeDispatch
    {
        using type = typename Config::template OutEdgeContainer<SRC, TGT>;
    };

    template <typename Config, std::size_t SRC> struct DefaultOutEdgeDispatch<Config, SRC, SRC>
    {
        struct type {};
    };
    
    template <typename Config, std::size_t SRC, std::size_t TGT> struct DefaultEdgeDispatch
    {
        using type = typename Config::template OutEdgeContainer<SRC, TGT>;
    };

    template <typename Config, std::size_t SRC> struct DefaultEdgeDispatch<Config, SRC, SRC>
    {
        struct type {};
    };

    template <std::size_t N, typename VertexLabels, typename EdgeLabels, typename DirectionMode, typename OutEdgeListT, typename VertexListT, typename EdgeListT> struct Config
    {
        using ME = Config<N, VertexLabels, EdgeLabels, DirectionMode, OutEdgeListT, VertexListT, EdgeListT>;
        template <std::size_t P> using VertexLabel = Label<VertexLabels, P>;
        template <std::size_t SRC, std::size_t TGT> using EdgeLabel = Label<EdgeLabels, SRC, TGT>;
        template <std::size_t P> using Vertex = npartite::Vertex<N, VertexLabels, EdgeLabels, DirectionMode, OutEdgeListT, VertexListT, EdgeListT, P>;
        template <std::size_t SRC, std::size_t TGT> using Edge = npartite::Edge<N, VertexLabels, EdgeLabels, DirectionMode, OutEdgeListT, VertexListT, EdgeListT, SRC, TGT>;

        template <std::size_t P> using VertexContainer = typename container_dispatch::Dispatch<VertexListT, Vertex<P>, DescriptorTransform<P>>::type;
        template <std::size_t P> using vertex_descriptor = typename VertexContainer<P>::descriptor;
        template <std::size_t SRC, std::size_t TGT> using EdgeContainer = typename container_dispatch::Dispatch<EdgeListT, Edge<SRC, TGT>, DescriptorTransform<SRC, TGT>>::type;
        template <std::size_t SRC, std::size_t TGT> using edge_descriptor = typename EdgeContainer<SRC, TGT>::descriptor; 
        template <std::size_t SRC, std::size_t TGT> using OutEdgeContainer = typename container_dispatch::Dispatch<OutEdgeListT, edge_descriptor<SRC, TGT>, DescriptorTransform<SRC, TGT>>::type;

        template <std::size_t SRC> struct OutEdgeDispatch
        {
            template <std::size_t DST> using type = typename DefaultOutEdgeDispatch<ME, SRC, DST>::type;
        };
        template <std::size_t DST> struct InEdgeDispatch
        {
            template <std::size_t SRC> using type = typename DefaultOutEdgeDispatch<ME, SRC, DST>::type;
        };
        template <std::size_t SRC> struct EdgeDispatch_1
        {
            template <std::size_t DST> using type = typename DefaultEdgeDispatch<ME, SRC, DST>::type;
        };
        
        template <std::size_t SRC>
        using EdgeDispatch = typename TupleFold<std::size_t, EdgeDispatch_1<SRC>::template type, std::make_index_sequence<N>>::type;


        template <std::size_t SRC> using OutEdgeTuple = typename TupleFold<std::size_t, OutEdgeDispatch<SRC>::template type, std::make_index_sequence<N>>::type;
        template <std::size_t SRC> using InEdgeTuple = typename TupleFold<std::size_t, InEdgeDispatch<SRC>::template type, std::make_index_sequence<N>>::type;
        using VertexTuple = typename TupleFold<std::size_t, Vertex, std::make_index_sequence<N>>::type;
        using EdgeTuple = typename TupleFold<std::size_t, EdgeDispatch, std::make_index_sequence<N>>::type;
    };

} } }

#endif

