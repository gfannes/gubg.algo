#ifndef HEADER_gubg_tree_sedes_hpp_ALREADY_INCLUDED
#define HEADER_gubg_tree_sedes_hpp_ALREADY_INCLUDED

#include <gubg/mss.hpp>
#include <gubg/naft/Document.hpp>
#include <gubg/naft/Range.hpp>
#include <gubg/Strange.hpp>
#include <gubg/map.hpp>
#include <sstream>

namespace gubg { namespace tree { 

    template <typename Forest, typename Ftor>
    bool serialize(std::ostream &os, const Forest &forest, Ftor &&ftor)
    {
        MSS_BEGIN(bool);
        gubg::naft::Document doc_naft{os};
        auto forest_naft = doc_naft.node("Forest");
        std::ostringstream oss;
        auto serialize_node = [&](const auto &node, const auto &path, unsigned int count)
        {
            if (count != 0)
                return;
            auto node_naft = forest_naft.node("Node");
            {
                auto path_naft = node_naft.node("Path");
                const auto size = path.size();
                path_naft.attr("size", size);
                oss.str("");
                for (auto ix = 0u; ix < size; ++ix)
                {
                    if (ix > 0)
                        oss << ',';
                    oss << path[ix];
                }
                path_naft.attr("values", oss.str());
            }
            {
                auto value_naft = node_naft.node("Value");
                ftor(value_naft, node.value);
            }
        };
        forest.dfs(serialize_node);
        MSS_END();
    }

    template <typename Forest, typename Ftor>
    bool deserialize(Forest &forest, const std::string &str, Ftor &&ftor)
    {
        MSS_BEGIN(bool);

        gubg::naft::Range doc_naft{str};

        MSS(doc_naft.pop_tag("Forest"));
        gubg::naft::Range forest_naft;
        Path path;
        if (doc_naft.pop_block(forest_naft))
        {
            while (forest_naft.pop_tag("Node"))
            {
                gubg::naft::Range node_naft;
                MSS(forest_naft.pop_block(node_naft));
                {
                    MSS(node_naft.pop_tag("Path"));

                    const auto attrs = node_naft.pop_attrs();
                    const auto size = std::stoul(gubg::value_or(std::string(), attrs, "size"));
                    path.resize(size);
                    Strange strange(gubg::value_or(std::string(), attrs, "values"));

                    for (auto ix = 0u; ix < size; ++ix)
                    {
                        if (ix > 0)
                            MSS(strange.pop_if(','));
                        MSS(strange.pop_decimal(path[ix]));
                    }
                }
                {
                    MSS(node_naft.pop_tag("Value"));
                    auto &node = *forest.find(path, true);
                    MSS(ftor(node.value, node_naft));
                }
            }
        }

        MSS_END();
    }

} } 

#endif
