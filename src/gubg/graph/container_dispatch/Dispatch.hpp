#ifndef HEADER_gubg_graph_container_dispatch_Dispatch_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_container_dispatch_Dispatch_hpp_ALREADY_INCLUDED

namespace gubg { namespace graph { namespace container_dispatch {

    struct NoTransform
    {
        template <typename T> struct apply
        {
            using type = T;
        };
    };

    template <typename Tag, typename T, typename DescriptorTransformer = NoTransform> struct Dispatch;

} } }

#endif

