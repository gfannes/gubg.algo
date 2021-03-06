#ifndef HEADER_gubg_planning_Day_hpp_ALREADY_INCLUDED
#define HEADER_gubg_planning_Day_hpp_ALREADY_INCLUDED

#include <gubg/Strange.hpp>
#include <ostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <ctime>
#include <set>

namespace gubg { namespace planning {

    class Day
    {
        public:
            static Day today()
            {
                auto t = std::time(0);
                auto gt = std::gmtime(&t);
                return Day(gt->tm_year+1900, gt->tm_mon+1, gt->tm_mday);
            }

            Day(){}
            Day(int y, int m, int d):y_(y), m_(m), d_(d){}
            Day(const std::string &str)
            {
                Strange strange(str);
                Strange tmp;
                int y;
                if (strange.pop_count(tmp, 4) && tmp.pop_decimal(y))
                {
                    strange.pop_if('-');
                    int m;
                    if (strange.pop_count(tmp, 2) && tmp.pop_decimal(m))
                    {
                        strange.pop_if('-');
                        int d = 1;
                        strange.pop_count(tmp, 2) && tmp.pop_decimal(d);
                        y_ = y;
                        m_ = m;
                        d_ = d;
                    }
                }
            }

            bool is_valid() const {return y_ != 0 || m_ != 0 || d_ != 0;}

            int day() const {return d_;}
            int month() const {return m_;}
            int year() const {return y_;}

            Day &operator++()
            {
                ++d_;
                switch (m_)
                {
#ifdef L_CASE
#error L_CASE already defined
#endif
#define L_CASE(m, n)                            case m: if (d_ > n) { d_ = 1; ++m_; } break
                    L_CASE(1, 31);
                    case 2:
                    {
                        int nrDays = (y_%4 == 0 ? 29 : 28);
                        if (y_%100 == 0 && y_%400 != 0)
                            nrDays = 28;
                        if (d_ > nrDays)
                        {
                            d_ = 1;
                            ++m_;
                        }
                    }
                    break;
                    L_CASE(3, 31);
                    L_CASE(4, 30);
                    L_CASE(5, 31);
                    L_CASE(6, 30);
                    L_CASE(7, 31);
                    L_CASE(8, 31);
                    L_CASE(9, 30);
                    L_CASE(10, 31);
                    L_CASE(11, 30);
                    case 12:
                    if (d_ > 31)
                    {
                        d_ = 1;
                        m_ = 1;
                        ++y_;
                    }
                    break;
#undef L_CASE
                }
                return *this;
            }

            std::string str() const
            {
                std::ostringstream oss;
                oss << std::setw(4) << std::setfill('0') << y_ << "-" << std::setw(2) << std::setfill('0') << m_ << "-" << std::setw(2) << std::setfill('0') << d_;
                return oss.str();
            }
            void stream(std::ostream &os) const { os << str(); }

            bool operator<(const Day &rhs) const
            {
                if (y_ != rhs.y_)
                    return y_ < rhs.y_;
                if (m_ != rhs.m_)
                    return m_ < rhs.m_;
                if (d_ != rhs.d_)
                    return d_ < rhs.d_;
                //Days are equal
                return false;
            }
            bool operator<=(const Day &rhs) const
            {
                if (y_ != rhs.y_)
                    return y_ < rhs.y_;
                if (m_ != rhs.m_)
                    return m_ < rhs.m_;
                if (d_ != rhs.d_)
                    return d_ < rhs.d_;
                //Days are equal
                return true;
            }
            bool operator==(const Day &rhs) const
            {
                return y_ == rhs.y_ && m_ == rhs.m_ && d_ == rhs.d_;
            }
            bool operator!=(const Day &rhs) const {return !operator==(rhs);}
            bool operator>(const Day &rhs) const {return !operator<=(rhs);}
            bool operator>=(const Day &rhs) const {return !operator<(rhs);}

        private:
            int y_ = 0;
            int m_ = 0;
            int d_ = 0;
    };
    typedef std::vector<Day> Days;

    //Returns a vector of nr days starting from today, only including mon ... fri
    inline Days work_days(size_t nr)
    {
        Days days;
        Day d = Day::today();
        auto t = std::time(0);
        auto gt = std::gmtime(&t);
        int wday = (gt->tm_wday+6)%7;//wday == 0 => monday
        while (days.size() < nr)
        {
            if (wday < 5)
                //mon .. fri
                days.push_back(d);
            ++d;
            wday = (wday+1)%7;
        }
        return days;
    }

    //to is also included
    inline Days day_range(Day from, Day to)
    {
        Days days;
        while (from <= to)
        {
            days.push_back(from);
            ++from;
        }
        return days;
    }
    //The first quarter might be incomplete
    typedef std::set<Day> Quarter;
    inline std::vector<Quarter> quarters(Day from, const size_t nr)
    {
        std::vector<Quarter> qs;
        Quarter q;
        while (qs.size() != nr)
        {
            q.insert(from);
            ++from;
            if (from.day() == 1 && (from.month()%3) == 1)
            {
                qs.push_back(q);
                q.clear();
            }
        }
        return qs;
    }

    inline std::ostream &operator<<(std::ostream &os, const Day &day)
    {
        day.stream(os);
        return os;
    }

} }

#endif
