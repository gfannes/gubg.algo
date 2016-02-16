#ifndef HEADER_gubg_planning_Types_hpp_ALREADY_INCLUDED
#define HEADER_gubg_planning_Types_hpp_ALREADY_INCLUDED

#include <string>
#include <vector>
#include <memory>
#include <ostream>

namespace gubg { namespace planning {

	typedef std::string Worker;
	typedef std::vector<Worker> Workers;
	typedef std::shared_ptr<Workers> WorkersPtr;
	typedef double Sweat;
	typedef double Efficiency;

	enum class Format {Text, Html};
    enum class Mode {Unset, Async, Sync};
    inline std::ostream &operator<<(std::ostream &os, Mode m)
    {
        switch (m)
        {
            case Mode::Unset: os << "Unset mode"; break;
            case Mode::Async: os << "Async mode"; break;
            case Mode::Sync: os << "Sync mode"; break;
            default: os << "Unknown mode"; break;
        }
        return os;
    }

} }

#endif
