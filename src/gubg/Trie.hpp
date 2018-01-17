#ifndef HEADER_gubg_Trie_hpp_ALREADY_INCLUDED
#define HEADER_gubg_Trie_hpp_ALREADY_INCLUDED

#include "gubg/Range.hpp"
#include <map>

namespace gubg {

template <typename _Key, typename _Tp = std::nullptr_t>
class Trie
{
public:
    using trie_type = Trie<_Key, _Tp>;
    using key_type = _Key;
    using value_type = _Tp;

    const value_type & value() const    { return value_; }
    value_type & value()                { return value_; }
    void set_value(const value_type & value) { value_ = value; }
    void set_value(value_type && value) { value_ = std::move(value); }

    trie_type * find(const key_type & key)
    {
        auto it = children_.find(key);
        return (it == children_.end()) ? 0 : &it->second;
    }

    const trie_type * find(const key_type & key) const
    {
        auto it = children_.find(key);
        return (it == children_.end()) ? 0 : &it->second;
    }

    trie_type & operator[](const key_type & key)
    {
        return children_[key];
    }

    const trie_type & operator[](const key_type & key) const
    {
        return children_[key];
    }

private:
    value_type value_;
    std::map<key_type, Trie> children_;
};

template <typename _KeyIt, typename _Key, typename _Tp>
Trie<_Key, _Tp> & path_goc(const gubg::Range<_KeyIt> & path, Trie<_Key, _Tp> & trie)
{
    auto * cur = &trie;
    for(const auto & key : path)
        cur = &cur->operator [](key);

    return *cur;
}

template <typename _Key, typename _Tp>
Trie<_Key, _Tp> & path_goc(const std::initializer_list<_Key> & path, Trie<_Key, _Tp> & trie)
{
    return path_goc(gubg::make_range(path), trie);
}


template <typename _KeyIt, typename _Key, typename _Tp>
std::pair<const Trie<_Key, _Tp> *, _KeyIt>
path_traverse(const gubg::Range<_KeyIt> & path, const Trie<_Key, _Tp> & trie)
{
    const auto * cur = &trie;
    for(auto it = path.begin(); it != path.end(); ++it)
    {
        const auto * nxt = cur->find(*it);
        if (nxt == nullptr)
            return std::make_pair(cur, it);
        cur = nxt;
    }

    return std::make_pair(cur, path.end());
}

template <typename _KeyIt, typename _Key, typename _Tp>
std::pair<Trie<_Key, _Tp> *, _KeyIt>
path_traverse(const gubg::Range<_KeyIt> & path, Trie<_Key, _Tp> & trie)
{
    auto * cur = &trie;
    for(auto it = path.begin(); it != path.end(); ++it)
    {
        auto * nxt = cur->find(*it);
        if (nxt == nullptr)
            return std::make_pair(cur, it);
        cur = nxt;
    }

    return std::make_pair(cur, path.end());
}

}






//    using  = std::vector<key_type>;

//    void insert(const Path)



//    void add(const Path & path)
//    {
//        add_(path.begin(), path.end());
//    }
//    void add(const std::initializer_list<Key> & path, const Value & value)
//    {
//        add_(path.begin(), path.end()).value_ = value;
//    }
//    void add(const std::initializer_list<Key> & path, Value && value)
//    {
//        add_(path.begin(), path.end()).value_ = std::move(value);
//    }

//    template <typename It> void add(It first, It last, const Value & value)
//    {
//        add_(first, last).value_ = value;
//    }
//    template <typename It> void add(It first, It last, Value && value)
//    {
//        add_(first, last).value_ = std::move(value);
//    }

//    Trie * find()





//private:
//    template <typename It> Trie & add_(It first, It last)
//    {
//        Trie * current = this;
//        for(; first != last; ++first)
//            current = &(current->children_[*first]);
//        return *current;
//    }


//        bool check(key_list path) {
//        return this->resolve(path);
//    }

//    detail::optional_ptr<Value> get() {
//        if ( this->value_.first ) {
//            return detail::optional_ptr<Value>(&this->value_.second);
//        } else {
//            return detail::optional_ptr<Value>();
//        }
//    }

//    detail::optional_ptr<Value> get(key_list path) {
//        if ( const auto source = this->resolve(path) ) {
//            return source.get()->get();
//        } else {
//            return detail::optional_ptr<Value>();
//        }
//    }

//    void set(const Value& value) {
//        this->value_.first  = true;
//        this->value_.second = value;
//    }

//    bool set(key_list path, const Value& value) {
//        if ( auto target = this->resolve(path) ) {
//            target.get()->set(value);

//            return true;
//        } else {
//            return false;
//        }
//    }


//private:
//    std::optional<Value> value_;
//    std::map<Key, Trie> children_;

//    Trie* add(
//        typename key_list::const_iterator current,
//        typename key_list::const_iterator end
//    ) {
//        if ( current != end ) {
//            auto element = *current;
//            return this->children_[element].add(++current, end);
//        } else {
//            return this;
//        }
//    }

//    void remove(
//        typename key_list::const_iterator current,
//        typename key_list::const_iterator end
//    ) {
//        if ( current != end ) {
//            typename std::map<Key, Trie>::iterator matching(
//                this->children_.find(*current)
//            );

//            if ( matching != this->children_.cend() ) {
//                typename key_list::const_iterator next(
//                    ++current
//                );

//                if ( next == end ) {
//                    this->children_.erase(matching);
//                } else {
//                    (*matching).second.remove(next, end);
//                }
//            }
//        }
//    }

//    detail::optional_ptr<Trie> resolve(key_list path) {
//        return this->resolve(path.cbegin(), path.cend());
//    }

//    detail::optional_ptr<Trie> resolve(
//        typename key_list::const_iterator current,
//        typename key_list::const_iterator end
//    ) {
//        typename std::map<Key, Trie>::iterator matching(
//            this->children_.find(*current)
//        );

//        if ( matching != this->children_.cend() ) {
//            typename key_list::const_iterator next(
//                ++current
//            );

//            if ( next == end ) {
//                return detail::optional_ptr<Trie>(
//                    &(*matching).second
//                );
//            } else {
//                return (*matching).second.resolve(next, end);
//            }
//        } else {
//            return detail::optional_ptr<Trie>();
//        }
//    }

//};

//}


//class Trie {
//    public:
//        typedef std::vector<Key> key_list;

//        Trie():
//            value_(false, Value{}),
//            children_() { }

//        void add(key_list path) {
//            this->add(path.cbegin(), path.cend());
//        }

//        void add(key_list path, Value value) {
//            Trie* const target = this->add(path.cbegin(), path.cend());

//            target->value_.first  = true;
//            target->value_.second = value;
//        }

//        void remove(key_list path) {
//            this->remove(path.cbegin(), path.cend());
//        }

//        bool check(key_list path) {
//            return this->resolve(path);
//        }

//        detail::optional_ptr<Value> get() {
//            if ( this->value_.first ) {
//                return detail::optional_ptr<Value>(&this->value_.second);
//            } else {
//                return detail::optional_ptr<Value>();
//            }
//        }

//        detail::optional_ptr<Value> get(key_list path) {
//            if ( const auto source = this->resolve(path) ) {
//                return source.get()->get();
//            } else {
//                return detail::optional_ptr<Value>();
//            }
//        }

//        void set(const Value& value) {
//            this->value_.first  = true;
//            this->value_.second = value;
//        }

//        bool set(key_list path, const Value& value) {
//            if ( auto target = this->resolve(path) ) {
//                target.get()->set(value);

//                return true;
//            } else {
//                return false;
//            }
//        }

//    private:
//        std::pair<bool, Value> value_;
//        std::map<Key, Trie> children_;

//        Trie* add(
//            typename key_list::const_iterator current,
//            typename key_list::const_iterator end
//        ) {
//            if ( current != end ) {
//                auto element = *current;
//                return this->children_[element].add(++current, end);
//            } else {
//                return this;
//            }
//        }

//        void remove(
//            typename key_list::const_iterator current,
//            typename key_list::const_iterator end
//        ) {
//            if ( current != end ) {
//                typename std::map<Key, Trie>::iterator matching(
//                    this->children_.find(*current)
//                );

//                if ( matching != this->children_.cend() ) {
//                    typename key_list::const_iterator next(
//                        ++current
//                    );

//                    if ( next == end ) {
//                        this->children_.erase(matching);
//                    } else {
//                        (*matching).second.remove(next, end);
//                    }
//                }
//            }
//        }

//        detail::optional_ptr<Trie> resolve(key_list path) {
//            return this->resolve(path.cbegin(), path.cend());
//        }

//        detail::optional_ptr<Trie> resolve(
//            typename key_list::const_iterator current,
//            typename key_list::const_iterator end
//        ) {
//            typename std::map<Key, Trie>::iterator matching(
//                this->children_.find(*current)
//            );

//            if ( matching != this->children_.cend() ) {
//                typename key_list::const_iterator next(
//                    ++current
//                );

//                if ( next == end ) {
//                    return detail::optional_ptr<Trie>(
//                        &(*matching).second
//                    );
//                } else {
//                    return (*matching).second.resolve(next, end);
//                }
//            } else {
//                return detail::optional_ptr<Trie>();
//            }
//        }

//};

#endif
