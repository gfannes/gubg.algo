#ifndef HEADER_gubg_graph_detail_ContainerDispatch_hpp_ALREADY_INCLUDED
#define HEADER_gubg_graph_detail_ContainerDispatch_hpp_ALREADY_INCLUDED

#include "gubg/graph/Tag.hpp"
#include "gubg/iterator/Transform.hpp"
#include <vector>
#include <list>

namespace gubg { namespace graph { namespace detail {

    template <typename Container>
    typename Container::iterator remove_const(Container & c, typename Container::const_iterator it)
    {
        return c.erase(it, it);
    }
    template <typename ContainerTag, typename T> struct ContainerDispatch;
    
    template <typename T> struct ContainerDispatch<use_list, T>
    {
        struct type
        {
        public:
            using container_type = std::list<T>;
            using size_type = typename container_type::size_type;
            using descriptor = typename container_type::const_iterator;
            using element_iterator = typename container_type::const_iterator;

            struct Transform 
            {
                descriptor operator()(descriptor d) const 
                { 
                    return d; 
                } 
            };
            using descriptor_iterator = iterator::Transform<descriptor, Transform, false>;

            type() : size_(0) {}

            const T & get(descriptor d) const 
            { 
                return *d; 
            }

            T & get(descriptor d) 
            { 
                return *remove_const(container_, d); 
            }

            descriptor append(const T & t)
            {
                auto d = container_.insert(container_.end(), t);
                ++size_;
                return d;
            }

            size_type size() const
            {
                return size_;
            }

            void clear()
            {
                size_ = 0;
                container_.clear();
            }

            Range<descriptor_iterator> descriptor_range() const
            {
                return gubg::make_range(descriptor_iterator(container_.begin()), descriptor_iterator(container_.end()));
            }
            
            Range<element_iterator> element_range() const
            {
                return gubg::make_range(container_);
            }

        private:
            container_type container_;
            std::size_t size_;
        };
    };

    template <typename T> struct ContainerDispatch<use_vector, T>
    {
        struct type
        {
            using container_type = std::vector<T>;
            using size_type = typename container_type::size_type;
            using element_iterator = typename container_type::const_iterator;
            using descriptor = size_type;
            struct Transform 
            { 
                Transform(element_iterator begin = element_iterator())
                : begin_(begin) {}
                descriptor operator()(element_iterator it) const 
                {
                    return it - begin_;
                }
                element_iterator begin_;
            };
            using descriptor_iterator = gubg::iterator::Transform<element_iterator, Transform, false>;


            const T & get(descriptor d) const 
            { 
                return container_[d];
            }

            T & get(descriptor d) 
            { 
                return container_[d];
            }

            descriptor append(const T & t)
            {
                auto it = container_.insert(container_.end(), t);
                return it - container_.begin();
            }

            size_type size() const
            {
                return container_.size();
            }

            void clear()
            {
                container_.clear();
            }
            Range<descriptor_iterator> descriptor_range() const
            {
                Transform tr(container_.begin());
                return gubg::make_range(descriptor_iterator(container_.begin(), tr), descriptor_iterator(container_.end(), tr));
            }

            Range<element_iterator> element_range() const
            {
                return gubg::make_range(container_);
            }

        private:
            container_type container_;
        };
    };

} } }

#endif

