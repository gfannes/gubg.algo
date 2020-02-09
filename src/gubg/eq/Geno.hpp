#ifndef HEADER_gubg_eq_Geno_hpp_ALREADY_INCLUDED
#define HEADER_gubg_eq_Geno_hpp_ALREADY_INCLUDED

#include <gubg/tree/Forest.hpp>
#include <gubg/tree/sedes.hpp>
#include <variant>

namespace gubg { namespace eq { 

    struct Cascade
    {
    };
    struct Mix
    {
    };
    struct Biquad
    {
        float b0 = 0;
        float b1 = 0;
        float b2 = 0;
        float a1 = 0;
        float a2 = 0;

        Biquad() {}
        Biquad(float b0, float b1, float b2, float a1, float a2): b0(b0), b1(b1), b2(b2), a1(a1), a2(a2) {}
    };
    struct Delay
    {
        unsigned int nr_samples = 0;

        Delay() {}
        Delay(unsigned int nr_samples): nr_samples(nr_samples) {}
    };

    using Node = std::variant<Mix, Cascade, Biquad, Delay>;

    using Geno = tree::Forest<Node>;

    inline bool serialize(std::ostream &os, const Geno &geno)
    {
        auto ftor = [&](auto &naft, const auto &value)
        {
            if (false) {}
            else if (auto bq = std::get_if<gubg::eq::Biquad>(&value))
            {
                auto bq_naft = naft.node("Biquad");
                bq_naft.attr("b0", bq->b0);
                bq_naft.attr("b1", bq->b1);
                bq_naft.attr("b2", bq->b2);
                bq_naft.attr("a1", bq->a1);
                bq_naft.attr("a2", bq->a2);
            }
            else if (auto dl = std::get_if<gubg::eq::Delay>(&value))
            {
                auto dl_naft = naft.node("Delay");
                dl_naft.attr("nr_samples", dl->nr_samples);
            }
            else if (auto cs = std::get_if<gubg::eq::Cascade>(&value))
            {
                auto cs_naft = naft.node("Cascade");
            }
            else if (auto mx = std::get_if<gubg::eq::Mix>(&value))
            {
                auto mx_naft = naft.node("Mix");
            }
        };
        return gubg::tree::serialize(os, geno, ftor);
    }

    inline bool deserialize(Geno &geno, const std::string &str)
    {
        MSS_BEGIN(bool);
        auto ftor = [&](auto &value, auto &naft)
        {
            MSS_BEGIN(bool);
            gubg::naft::Range block;
            MSS(naft.pop_block(block));
            std::string tag;
            MSS(block.pop_tag(tag));
            if (false) {}
            else if (tag == "Biquad")
            {
                Biquad bq;
                const auto attrs = block.pop_attrs();
                bq.b0 = std::stod(gubg::value_or(std::string(), attrs, "b0"));
                bq.b1 = std::stod(gubg::value_or(std::string(), attrs, "b1"));
                bq.b2 = std::stod(gubg::value_or(std::string(), attrs, "b2"));
                bq.a1 = std::stod(gubg::value_or(std::string(), attrs, "a1"));
                bq.a2 = std::stod(gubg::value_or(std::string(), attrs, "a2"));
                value = bq;
            }
            else if (tag == "Delay")
            {
                Delay dl;
                const auto attrs = block.pop_attrs();
                dl.nr_samples = std::stoul(gubg::value_or(std::string(), attrs, "nr_samples"));
                value = dl;
            }
            else if (tag == "Cascade")
            {
                value = Cascade{};
            }
            else if (tag == "Mix")
            {
                value = Mix{};
            }
            else MSS(false, std::cout << "Error: unknown EQ geno type " << tag << std::endl);
            MSS_END();
        };
        return gubg::tree::deserialize(geno, str, ftor);
        MSS_END();
    }

} } 

#endif
