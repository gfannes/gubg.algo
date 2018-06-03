#ifndef HEADER_gubg_graph_container_dispatch_Vector_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_container_dispatch_Vector_hpp_ALREADY_INCLUDED

#include "gubg/graph/container_dispatch/Tag.hpp"
#include "gubg/graph/container_dispatch/Dispatch.hpp"
#include "gubg/iterator/Transform.hpp"
#include <vector>

namespace gubg { namespace graph { namespace container_dispatch {

    template <typename T, typename DescriptorTransformer>
    struct Dispatch<VectorT, T, DescriptorTransformer>
    {
        struct type
        {
            using container_type = std::vector<T>;
            using element_iterator = typename container_type::const_iterator;
            using descriptor = typename DescriptorTransformer::template Apply<typename container_type::size_type>::type;
            
            struct Transform
            {
                Transform(element_iterator begin = element_iterator()) : begin_(begin) {}

                descriptor operator()(element_iterator it) const
                {
                    return it - begin_;
                }
                element_iterator begin_;
            };
            using descriptor_iterator = gubg::iterator::Transform<element_iterator, Transform, false>;

            gubg::Range<element_iterator> element_range() const
            {
                return gubg::make_range(ctr_);
            }
            gubg::Range<descriptor_iterator> descriptor_range() const
            {
                Transform transform(ctr_.begin());
                return gubg::make_range(
                                       descriptor_iterator(ctr_.begin(), transform),
                                       descriptor_iterator(ctr_.end(), transform)
                                       );
            }
            const T & get(descriptor d) const
            {
                return ctr_[d];
            }
            T & get(descriptor d)
            {
                return ctr_[d];
            }
            descriptor append(const T & t)
            {
                auto it = ctr_.insert(ctr_.end(), t);
                {
                    Transform transform(ctr_.begin());
                    return transform(it);
                }
            }
            descriptor append(T && t)
            {
                auto it = ctr_.insert(ctr_.end(), std::move(t));
                {
                    Transform transform(ctr_.begin());
                    return transform(it);
                }
            }

            std::vector<T> ctr_;
        };
    };

} } }

#endif

