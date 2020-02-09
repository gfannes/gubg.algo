#ifndef HEADER_gubg_gp_support_Formula_hpp_ALREADY_INCLUDED
#define HEADER_gubg_gp_support_Formula_hpp_ALREADY_INCLUDED

#include <gubg/tree/Forest.hpp>
#include <gubg/tree/stream.hpp>
#include <gubg/mss.hpp>
#include <vector>
#include <cmath>
#include <sstream>

namespace support { 

    namespace geno { 
        using Forest = gubg::tree::Forest<std::string>;
        inline std::string hr(const Forest &forest)
        {
            std::ostringstream oss;
            gubg::tree::stream(oss, forest);
            return oss.str();
        }
    } 

    namespace pheno { 
        struct Operation
        {
            static const constexpr size_t Plus = 0;
            static const constexpr size_t Mul = 1;
            static const constexpr size_t Sine = 2;
            static const constexpr size_t Cosine = 3;
            static const constexpr size_t Nr_ = 4;
        };
        using Formula = std::vector<size_t>;

        struct IX
        {
            static const size_t A = 0;
            static const size_t B = 1;
            static const size_t C = 2;
            static const size_t X = 3;
            static const size_t Nr_ = 4;

            static const size_t data(size_t offset) {return offset+Nr_;}
        };
        using Tape = std::vector<double>;

        template <bool SafeMode>
        bool process(Tape &tape, const Formula &formula)
        {
            MSS_BEGIN(bool);

            const auto fsize = formula.size();

            size_t tix = IX::Nr_;
            for (auto fix = 0u; fix < fsize; ++tix)
            {
                if (SafeMode)
                    if (tix >= tape.size())
                        tape.resize(tix+1);
                auto &dst = tape[tix];

                const auto operation = formula[fix++];
                const auto fsize_left = fsize-fix;
                switch (operation)
                {
                    case Operation::Plus:
                        {
                            if (SafeMode) MSS(fsize_left >= 2);
                            const auto aix = formula[fix++];
                            const auto bix = formula[fix++];
                            if (SafeMode) MSS(aix < tix && bix < tix);
                            dst = tape[aix]+tape[bix];
                        }
                        break;
                    case Operation::Mul:
                        {
                            if (SafeMode) MSS(fsize_left >= 2);
                            const auto aix = formula[fix++];
                            const auto bix = formula[fix++];
                            if (SafeMode) MSS(aix < tix && bix < tix);
                            dst = tape[aix]*tape[bix];
                        }
                        break;
                    case Operation::Sine:
                        {
                            if (SafeMode) MSS(fsize_left >= 1);
                            const auto aix = formula[fix++];
                            if (SafeMode) MSS(aix < tix);
                            dst = std::sin(tape[aix]);
                        }
                        break;
                    case Operation::Cosine:
                        {
                            if (SafeMode) MSS(fsize_left >= 1);
                            const auto aix = formula[fix++];
                            if (SafeMode) MSS(aix < tix);
                            dst = std::cos(tape[aix]);
                        }
                        break;
                    default:
                        MSS(false, std::cout << "Error: unknown operation " << (int)operation << std::endl);
                        break;
                }
            }

            MSS_END();
        }
    } 

} 

#endif
