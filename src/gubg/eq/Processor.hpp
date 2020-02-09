#ifndef HEADER_gubg_eq_Processor_hpp_ALREADY_INCLUDED
#define HEADER_gubg_eq_Processor_hpp_ALREADY_INCLUDED

#include <gubg/eq/Geno.hpp>
#include <gubg/biquad/Filter.hpp>
#include <gubg/History.hpp>
#include <gubg/mss.hpp>
#include <vector>
#include <algorithm>
#include <memory>
#include <cassert>

namespace gubg { namespace eq { 

    template <typename T>
    class Processor
    {
    public:
        bool initialize(const Geno &geno, size_t blocksize)
        {
            MSS_BEGIN(bool);
            blocksize_ = blocksize;
            std::vector<T *> stack;
            auto add_operations = [&](const auto &node, const auto &path, unsigned int count)
            {
                if (false) {}
                else if (auto bq = std::get_if<eq::Biquad>(&node.value))
                {
                    operations_.emplace_back(new Biquad{blocksize, bq->b0, bq->b1, bq->b2, bq->a1, bq->a2});
                }
                else if (auto dl = std::get_if<eq::Delay>(&node.value))
                {
                    operations_.emplace_back(new Delay{blocksize, dl->nr_samples});
                }
                else if (auto cs = std::get_if<eq::Cascade>(&node.value))
                {
                }
                else if (auto mx = std::get_if<eq::Mix>(&node.value))
                {
                    switch (count)
                    {
                        case 0:
                            operations_.emplace_back(new Copy{blocksize});
                            break;
                        case 1:
                            operations_.emplace_back(new Mix{blocksize, -2});
                            break;
                        case 2:
                            operations_.emplace_back(new Mix{blocksize, -1});
                            break;
                    }
                }
            };
            geno.dfs(add_operations);
            MSS_END();
        }

        bool process(T *begin, T *end)
        {
            MSS_BEGIN(bool);

            MSS((end-begin) == blocksize_);

            ptr_stack_.resize(1);
            ptr_stack_.front() = begin;

            for (auto &operation: operations_)
                operation->process(ptr_stack_);

            assert(ptr_stack_.size() == 1);
            assert(ptr_stack_.front() == begin);

            MSS_END();
        }

    private:
        using PtrStack = std::vector<T *>;
        PtrStack ptr_stack_;

        class Operation
        {
        public:
            size_t blocksize = 0;

            Operation() {}
            Operation(size_t bs): blocksize(bs) {}
            virtual ~Operation() {}
            virtual void process(PtrStack &stack) = 0;
        };
        std::vector<std::shared_ptr<Operation>> operations_;

        class Biquad: public Operation
        {
        public:
            Biquad(size_t bs, T b0, T b1, T b2, T a1, T a2): Operation(bs)
            {
                biquad::Coefficients<T> coeffs;
                coeffs.b0 = b0;
                coeffs.b1 = b1;
                coeffs.b2 = b2;
                coeffs.a1 = a1;
                coeffs.a2 = a2;
                filter_.set(coeffs);
            }
            virtual ~Biquad() {}
            void process(PtrStack &stack) override
            {
                auto ptr = stack.back();
                for (auto ix = 0u; ix < Operation::blocksize; ++ix)
                    ptr[ix] = filter_(ptr[ix]);
            }
        private:
            biquad::Filter<T> filter_;
        };
        class Delay: public Operation
        {
        public:
            Delay(size_t bs, size_t delay): Operation(bs), delay_(delay), history_(delay) {}
            virtual ~Delay() {}
            void process(PtrStack &stack) override
            {
                auto ptr = stack.back();
                if (delay_ > 0)
                    for (auto ix = 0u; ix < Operation::blocksize; ++ix)
                    {
                        const auto v = ptr[ix];
                        ptr[ix] = history_.back();
                        history_.push_pop(v);
                    }
            }
        private:
            size_t delay_;
            History<T> history_;
        };
        class Copy: public Operation
        {
        public:
            Copy(size_t bs): Operation(bs), buffer_a_(bs), buffer_b_(bs) {}
            virtual ~Copy() {}
            void process(PtrStack &stack) override
            {
                auto ptr = stack.back();
                std::copy(ptr, ptr+Operation::blocksize, buffer_a_.data());
                std::copy(ptr, ptr+Operation::blocksize, buffer_b_.data());
                stack.emplace_back(buffer_a_.data());
                stack.emplace_back(buffer_b_.data());
                for (auto ix = 0u; ix < Operation::blocksize; ++ix)
                    ptr[ix] = 0;
            }
        private:
            std::vector<T> buffer_a_;
            std::vector<T> buffer_b_;
        };
        class Mix: public Operation
        {
        public:
            Mix(size_t bs, int back_offset): Operation(bs), back_offset_(back_offset) {}
            virtual ~Mix() {}
            void process(PtrStack &stack) override
            {
                const auto back_ix = stack.size()-1;
                auto src = stack[back_ix];
                auto dst = stack[back_ix+back_offset_];
                for (auto ix = 0u; ix < Operation::blocksize; ++ix)
                    dst[ix] += src[ix];
                stack.pop_back();
            }
        private:
            int back_offset_ = 0;
        };

        size_t blocksize_ = 0;
    };

} } 

#endif
