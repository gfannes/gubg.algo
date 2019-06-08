#ifndef HEADER_gubg_planning_Task_hpp_ALREADY_INCLUDED
#define HEADER_gubg_planning_Task_hpp_ALREADY_INCLUDED

#include "gubg/planning/Types.hpp"
#include "gubg/planning/Day.hpp"
#include "gubg/tree/dfs/Iterate.hpp"
#include <string>
#include <vector>
#include <list>
#include <map>
#include <ostream>
#include <algorithm>

namespace gubg { namespace planning {

    class Task;
    typedef std::shared_ptr<Day> Day_ptr;
    struct CompareDeadlines
    {
        bool operator()(const Day_ptr & lhs, const Day_ptr & rhs) const
        {
            return *lhs < *rhs;
        }
    };
    typedef std::multimap<Day_ptr, std::shared_ptr<Task>, CompareDeadlines> TasksPerDeadline;

    namespace priv
    {
        class Printer
        {
            public:
                Printer(std::ostream &os):os_(os){}
                template <typename NPtr, typename P>
                    bool open(NPtr ptr, P &p) const
                    {
                        auto &n = *ptr;
                        if (p.empty())
                            os_ << n.fullName() << std::endl;
                        else
                        {
                            os_ << std::string(2*p.size(), ' ') << n.name;
                            if (n.deadline)
                                os_	<< " deadline: " << *n.deadline;
                            if (n.workers && !n.workers->empty())
                            {
                                os_ << " workers: ";
                                for (auto w: *n.workers)
                                    os_ << " " << w;
                            }
                            os_ << " effort: " << n.sweat << std::endl;
                        }
                        return true;
                    }
                template <typename NPtr, typename P>
                    void close(NPtr ptr, P &p) const
                    {
                    }
            private:
                std::ostream &os_;
        };
        class DistributeWorkers
        {
            public:
                template <typename NPtr, typename P>
                    bool open(NPtr ptr, P &p) const
                    {
                        auto &n = *ptr;
                        if (!n.workers && !p.empty())
                            n.workers = p.back()->workers;
                        return true;
                    }
                template <typename NPtr, typename P>
                    void close(NPtr ptr, P &p) const
                    {
                    }
        };
        class DistributeDeadline
        {
            public:
                template <typename NPtr, typename P>
                    bool open(NPtr ptr, P &p) const
                    {
                        auto &n = *ptr;
                        if (!n.deadline && !p.empty())
                            n.deadline = p.back()->deadline;
                        return true;
                    }
                template <typename NPtr, typename P>
                    void close(NPtr ptr, P &p) const
                    {
                    }
        };
        class DistributeStartdate
        {
            public:
                template <typename NPtr, typename P>
                    bool open(NPtr ptr, P &p) const
                    {
                        auto &n = *ptr;
                        if (!n.startdate && !p.empty())
                            n.startdate = p.back()->startdate;
                        return true;
                    }
                template <typename NPtr, typename P>
                    void close(NPtr ptr, P &p) const
                    {
                    }
        };
        class DistributeMode
        {
            public:
                template <typename NPtr, typename P>
                    bool open(NPtr ptr, P &p)
                    {
                        auto &n = *ptr;
                        if (n.mode == Mode::Unset)
                            //When no mode was set, we take the mode from the parent, if any
                            n.mode = (!p.empty() ? p.back()->mode : Mode::Async);

                        n.mode_transition = (n.mode != mode_);

                        const auto is_leaf = n.childs.empty();
                        if (is_leaf)
                            mode_ = n.mode;

                        return true;
                    }
                template <typename NPtr, typename P>
                    void close(NPtr ptr, P &p)
                    {
                    }
            private:
                Mode mode_ = Mode::Unset;
        };
        class CheckDeadlines
        {
            public:
                mutable bool same;
                mutable Day_ptr deadline;
                CheckDeadlines():same(true){}
                template <typename NPtr, typename P>
                    bool open(NPtr ptr, P &p) const
                    {
                        auto &n = *ptr;
                        if (!deadline)
                            deadline = n.deadline;
                        else if (deadline != n.deadline)
                        {
                            same = false;
                            return false;
                        }
                        return true;
                    }
                template <typename NPtr, typename P>
                    void close(NPtr ptr, P &p) const
                    {
                    }
        };
        class AggregateSweat
        {
            public:
                template <typename NPtr, typename P>
                    bool open(NPtr ptr, P &p) const
                    {
                        auto &n = *ptr;
                        n.cumulSweat = n.sweat;
                        return true;
                    }
                template <typename NPtr, typename P>
                    void close(NPtr ptr, P &p) const
                    {
                        auto &n = *ptr;
                        if (p.empty())
                            return;
                        p.back()->cumulSweat += n.cumulSweat;
                    }
        };
        class AggregateStartStop
        {
            public:
                template <typename NPtr, typename P>
                    bool open(NPtr ptr, P &p) const
                    {
                        return true;
                    }
                template <typename NPtr, typename P>
                    void close(NPtr ptr, P &p) const
                    {
                        auto &n = *ptr;
                        if (p.empty())
                            return;
                        auto &parent = *p.back();
                        if (!parent.start.is_valid() || n.start < parent.start)
                            parent.start = n.start;
                        if (!parent.stop.is_valid() || parent.stop < n.stop)
                            parent.stop = n.stop;
                    }
        };
        class MaxStop
        {
            public:
                bool all_leafs_are_planned = true;
                Day stop;

                template <typename NPtr, typename P>
                    bool open(NPtr ptr, P &p)
                    {
                        return true;
                    }
                template <typename NPtr, typename P>
                    void close(NPtr ptr, P &p)
                    {
                        auto &n = *ptr;
                        const bool is_leaf = n.childs.empty();
                        if (is_leaf)
                        {
                            all_leafs_are_planned && (all_leafs_are_planned = n.stop.is_valid());
                            if (!stop.is_valid() || stop < n.stop)
                                stop = n.stop;
                        }
                    }
        };
        class CollectTasksPerDeadline
        {
            public:
                CollectTasksPerDeadline(TasksPerDeadline &tpd):tpd_(tpd){}
                template <typename NPtr, typename P>
                    bool open(NPtr ptr, P &p) const
                    {
                        auto &n = *ptr;
                        CheckDeadlines checkDeadlines;
                        tree::dfs::iterate_ptr(ptr, checkDeadlines);
                        if (checkDeadlines.same)
                        {
                            tpd_.insert(std::make_pair(n.deadline, n.shared_from_this()));
                            return false;
                        }
                        return true;
                    }
                template <typename NPtr, typename P>
                    void close(NPtr ptr, P &p) const
                    {
                    }
            private:
                TasksPerDeadline &tpd_;
        };
    }

    class Task: public std::enable_shared_from_this<Task>
    {
        public:
            typedef std::string Name;
            typedef std::string Category;
            typedef std::shared_ptr<Task> Ptr;
            typedef std::weak_ptr<Task> WPtr;
            typedef std::shared_ptr<Task> Child;
            typedef std::vector<Child> Childs;
            typedef std::vector<Ptr> Dependencies;

            Name name;
            Category category;
            Sweat sweat;
            Sweat cumulSweat;
            Day start;
            Day stop;
            Day_ptr deadline;
            Day_ptr startdate;
            WorkersPtr workers;
            Childs childs;
            WPtr parent;
            Mode mode = Mode::Unset;
            bool mode_transition = false;
            Dependencies dependencies;

            static Ptr create(Name n){return Ptr(new Task(n));}

            size_t id() const {return (size_t)this;}

            void setCategory(std::string cat) {category = std::move(cat);}

            Ptr addChild(Name n)
            {
                assert(invariants_());
                Ptr child;
                if (sweat == 0)
                {
                    //We only allow leafs to have sweat
                    childs.push_back(child = Task::create(n));
                    child->parent = shared_from_this();
                    assert(child->invariants_());
                }
                assert(invariants_());
                return child;
            }
            void addDependency(const Ptr &ptr)
            {
                S("Task");L(STREAM(dependencies.size()) << " adding " << ptr->fullName() << " to " << fullName());
                dependencies.push_back(ptr);
            }
            void setDeadline(Day day)
            {
                assert(invariants_());
                deadline.reset(new Day(day));
                assert(invariants_());
            }
            void setStartdate(Day day)
            {
                assert(invariants_());
                startdate.reset(new Day(day));
                assert(invariants_());
            }
            void setMode(Mode m)
            {
                assert(invariants_());
                mode = m;
                assert(invariants_());
            }
            void setWorkers(Workers ws)
            {
                assert(invariants_());
                workers.reset(new Workers(ws));
                assert(invariants_());
            }
            void setSweat(Sweat sw)
            {
                assert(invariants_());
                sweat = sw;
                assert(invariants_());
            }

            void distributeWorkers()
            {
                tree::dfs::iterate_ptr(this, priv::DistributeWorkers());
            }
            void distributeDeadlines()
            {
                tree::dfs::iterate_ptr(this, priv::DistributeDeadline());
            }
            void distributeStartdates()
            {
                tree::dfs::iterate_ptr(this, priv::DistributeStartdate());
            }
            void distributeModes()
            {
                priv::DistributeMode ftor;
                tree::dfs::iterate_ptr(this, ftor);
            }
            void aggregateSweat()
            {
                tree::dfs::iterate_ptr(this, priv::AggregateSweat());
            }
            void aggregateStartStop()
            {
                tree::dfs::iterate_ptr(this, priv::AggregateStartStop());
            }
            void get_max_stop(bool &all_leafs_are_planned, Day &day)
            {
                priv::MaxStop max_stop;
                tree::dfs::iterate_ptr(this, max_stop);
                all_leafs_are_planned = max_stop.all_leafs_are_planned;
                day = max_stop.stop;
            }

            TasksPerDeadline tasksPerDeadline()
            {
                TasksPerDeadline tpd;
                tree::dfs::iterate_ptr(this, priv::CollectTasksPerDeadline(tpd));
                return tpd;
            }

            bool isPlanned() const {return start.is_valid() && stop.is_valid();}

            void stream(std::ostream &os) const
            {
                tree::dfs::iterate_ptr(this, priv::Printer(os));
            }

            using NameParts = std::list<std::string>;
            NameParts name_parts() const
            {
                NameParts parts;
                auto n = shared_from_this();
                while (n)
                {
                    parts.push_front(n->name);
                    n = n->parent.lock();
                }
                return parts;
            }

            Name base_name(int nr, unsigned int skip = 0) const
            {
                const auto parts = name_parts();
                std::ostringstream oss;
                for (auto it = parts.begin(); it != parts.end() && nr != 0; ++it, --nr)
                {
                    if (skip > 0)
                    {
                        --skip;
                        continue;
                    }
                    oss << *it << "/";
                }
                return oss.str();
            }
            Name fullName() const { return base_name(-1); }

        private:
            Task(Name n):name(n), sweat(0), cumulSweat(0) { }

            bool invariants_() const
            {
                if (!childs.empty() && sweat != 0)
                    return false;
                return true;
            }
    };
    typedef std::vector<Task> Tasks;
    struct TaskRange
    {
        Tasks::iterator begin(){return b;}
        Tasks::iterator end(){return e;}
        Task &front(){return *b;}
        Task &back(){return *(e-1);}
        size_t size() const {return e - b;}

        Tasks::iterator b;
        Tasks::iterator e;
    };

    inline std::ostream &operator<<(std::ostream &os, const Task &task)
    {
        task.stream(os);
        return os;
    }

} }

#endif
