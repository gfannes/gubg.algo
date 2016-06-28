#ifndef HEADER_gubg_planning_Planning_hpp_ALREADY_INCLUDED
#define HEADER_gubg_planning_Planning_hpp_ALREADY_INCLUDED

#include "gubg/planning/Codes.hpp"
#include "gubg/planning/Types.hpp"
#include "gubg/planning/Task.hpp"
#include "gubg/tree/dfs/Leafs.hpp"
#include "gubg/OnlyOnce.hpp"
#include "gubg/xml/Builder.hpp"
#include <vector>
#include <map>
#include <set>
#include <algorithm>
#include <iomanip>

namespace gubg { namespace planning {

	class Planning
	{
        private:
            static constexpr const char *logns = "Planning";

		public:
			struct TaskPart
			{
				Sweat sweat;
				Task::Ptr task;

				TaskPart():sweat(0){}
			};
			typedef std::list<TaskPart> TaskParts;

			void addWorker(Worker worker, Efficiency efficiency)
			{
				efficiencyPerWorker_[worker] = efficiency;
			}
			void addDay(Day day)
			{
				for (auto &p: efficiencyPerWorker_)
					dayPlanningsPerWorker_[p.first][day] = DayPlanning(day, p.second);
			}
			void absence(Worker worker, Day day)
			{
				dayPlanningsPerWorker_[worker][day] = DayPlanning(day, 0);
			}

			bool getLastDay(Day &day) const
			{
				Day d;
				for (auto &p: dayPlanningsPerWorker_)
				{
					const Day ld = p.second.rbegin()->first;
					if (!d.isValid() || d < ld)
						d = ld;
				}
				if (!d.isValid())
					return false;
				day = d;
				return true;
			}

			ReturnCode plan(Task &full_task_tree)
			{
				MSS_BEGIN(ReturnCode);L(STREAM(full_task_tree));

				full_task_tree.distributeDeadlines();
				full_task_tree.distributeStartdates();
				full_task_tree.distributeModes();
				full_task_tree.distributeWorkers();
				full_task_tree.aggregateSweat();

				auto tpd = full_task_tree.tasksPerDeadline();
				for (auto &p: tpd)
                {
                    auto &task_tree = *p.second;
					MSS(planTreeASAP(task_tree));
                }

				full_task_tree.aggregateStartStop();

				MSS_END();
			}

			ReturnCode planTreeASAP(Task &task)
			{
				MSS_BEGIN(ReturnCode, logns);L(STREAM(task.fullName()));
				auto leafTasks = tree::dfs::leafs(task);
				L("I found " << leafTasks.size() << " leaf tasks");

                Day_ptr eta;
                Mode mode = Mode::Unset;
				for (auto leaf: leafTasks)
                {
                    auto &task = *leaf;
                    if (task.mode == Mode::Async && mode != task.mode)
                        //When switching back to async planning mode, we reset the estimated time of arrival
                        eta.reset();
                    mode = task.mode;
					MSS(planLeafASAP(task, eta));
                }

                if (!!eta)
                    L(STREAM(*eta));
				MSS_END();
			}

			ReturnCode planLeafASAP(Task &task, Day_ptr &eta)
			{
				MSS_BEGIN(ReturnCode, logns);L(STREAM(task.fullName(), task.mode));

				MSS(task.childs.empty());

				MSS((bool)task.workers);
				const auto workers = *task.workers;

				Sweat sweat = task.cumulSweat;
				while (sweat > eps_())
				{
					Worker worker;
                    const Day *startdate = nullptr;
                    if (task.mode == Mode::Sync)
                        startdate = eta.get();
                    if (!startdate)
                        startdate = task.startdate.get();
                    else if (!!task.startdate && *startdate < *task.startdate)
                        startdate = task.startdate.get();

                    Day max_dep_startday;
                    for (const auto &dep: task.dependencies)
                    {
                        bool all_leafs_are_planned;
                        Day max_stop;
                        dep->get_max_stop(all_leafs_are_planned, max_stop);
                        L("This task depends on " << dep->fullName() << STREAM(all_leafs_are_planned, max_stop));
                        MSS(all_leafs_are_planned, L("Please make sure the dependecies are tree-like: " << task.fullName() << " depends on " << dep->fullName() << " (" << dep.get() << ") " << " but the latter has no stop time yet."));
                        if (max_dep_startday < max_stop)
                            max_dep_startday = max_stop;
                    }
                    if (!startdate || *startdate < max_dep_startday)
                        startdate = &max_dep_startday;

					auto dayPlanning = getFirstAvailableDayPlanning_(worker, workers, startdate);
					MSS(mss::on_fail(dayPlanning != 0, ReturnCode::NotEnoughSweatAvailable));
					auto taskPart = dayPlanning->addTask(task, sweat);
					MSS(!!taskPart);
					if (!task.start.isValid())
						task.start = dayPlanning->day;
					task.stop = dayPlanning->day;
                    if (!eta)
                        eta.reset(new Day(task.stop));
                    if (*eta < task.stop)
                        *eta = task.stop;
					/* L("Assigned " << taskPart->sweat << " to " << worker << " on " << dayPlanning->day << STREAM(*eta)); */
				}
                L("Task " << task.fullName() << " (" << &task << ")" << " will end on " << task.stop);

				MSS_END();
			}

			typedef std::map<Day, TaskParts> TaskPartsPerDay;
			TaskPartsPerDay taskPartsPerDay() const
			{
				S(0);
				TaskPartsPerDay res;
				for (auto &dpsw: dayPlanningsPerWorker_)
				{
					for (auto &dp: dpsw.second)
					{
						for (auto &tp: dp.second.taskParts)
						{
							L(STREAM(*tp.task));
							res[dp.first].push_back(tp);
							L(res.size());
						}
					}
				}
				return res;
			}

			void stream(std::ostream &os, Format format = Format::Text) const
			{
				switch (format)
				{
					case Format::Text:
						streamText_(os);
						break;
					case Format::Html:
						streamHtml_(os);
						break;
				}
			}
			void overview(std::ostream &os, const std::set<Day> &days)
			{
				if (days.empty())
				{
					os << "Empty period specified" << std::endl;
					return;
				}
				std::map<std::string, double> sweatPerCat;
				for (auto it: taskPartsPerDay())
				{
					auto day = it.first;
					if (days.count(day) > 0)
					{
						for (auto &tp: it.second)
						{
							auto task = tp.task;
							auto sweat = tp.sweat;
							if (task)
								sweatPerCat[task->category] += sweat;
						}
					}
				}
				auto b = days.begin();
				auto e = days.end();
				--e;
				for (auto &c_s: sweatPerCat)
					os << c_s.first << "\t" << c_s.second << std::endl;
			}

			size_t depth_(Task::Ptr task)
			{
				Task::Ptr p = task->parent.lock();
				if (!p)
					return 0;
				return depth_(p)+1;
			}
			Task::Ptr taskAtLevel_(Task::Ptr task, size_t level)
			{
				if (level == 0)
					//End of recursion
					return task;
				Task::Ptr p = task->parent.lock();
				if (!p)
					//No more parents
					return task;
				return taskAtLevel_(p, level-1);
			}
			void overviewForWorker(std::ostream &os, const Worker &worker, const std::set<Day> &days, const size_t wantedLevel)
			{
				if (days.empty())
				{
					os << "Empty period specified" << std::endl;
					return;
				}
				std::map<Task::Ptr, double> sweatPerTask;
				for (auto dp: dayPlanningsPerWorker_[worker])
				{
					if (!days.count(dp.first))
						continue;
					for (auto tp: dp.second.taskParts)
					{
						auto task = tp.task;
						auto level = depth_(task);
						if (level >= wantedLevel)
							level -= wantedLevel;
						else
							level = 0;
						sweatPerTask[taskAtLevel_(task, level)] += tp.sweat;
					}
				}
				auto b = days.begin();
				auto e = days.end();
				--e;
				size_t maxTaskNameSize = 0;
				for (auto &t_s: sweatPerTask)
				{
					auto name = t_s.first->fullName();
					if (name.size() > maxTaskNameSize)
						maxTaskNameSize = name.size();
				}
				for (auto &t_s: sweatPerTask)
				{
					auto name = t_s.first->fullName();
					os << " * " << name << std::string(maxTaskNameSize-name.size(), ' ') << " [" << t_s.second << "days, " << t_s.first->category << "]" << std::endl;
				}
			}

		private:
			typedef std::map<Worker, Efficiency> EfficiencyPerWorker;
			EfficiencyPerWorker efficiencyPerWorker_;

			struct DayPlanning
			{
				Day day;
				Sweat sweat;
				TaskParts taskParts;

				DayPlanning():sweat(0){}
				DayPlanning(Day d, Sweat s):day(d), sweat(s){}

				Sweat availableSweat() const
				{
					Sweat sw = sweat;
					for (const auto &tp: taskParts)
						sw -= tp.sweat;
					return sw;
				}
				bool hasSweatAvailable() const
				{
					return availableSweat() > eps_();
				}
				TaskPart *addTask(Task &task, Sweat &sweatLeft)
				{
					TaskPart tp;
					tp.sweat = std::min(availableSweat(), sweatLeft);
					if (tp.sweat <= 0)
						return 0;
					tp.task = task.shared_from_this();
					taskParts.push_back(tp);
					sweatLeft -= tp.sweat;
					return &taskParts.back();
				}
			};
			typedef std::map<Day, DayPlanning> DayPlannings;
			typedef std::map<Worker, DayPlannings> DayPlanningsPerWorker;
			DayPlanningsPerWorker dayPlanningsPerWorker_;

			DayPlanning *getFirstAvailableDayPlanning_(Worker &w, const Workers &workers, const Day *startdate)
			{
				DayPlanning *res = 0;
				for (const auto &worker: workers)
				{
					auto it = dayPlanningsPerWorker_.find(worker);
					if (it == dayPlanningsPerWorker_.end())
						continue;
					auto &dayPlannings = it->second;
					for (auto &p: dayPlannings)
					{
                        const Day &day = p.first;
                        DayPlanning &day_planning = p.second;

                        if (!!startdate && day < *startdate)
                            //This is too soon for this task
                            continue;

						if (day_planning.hasSweatAvailable())
						{
							if (!res)
							{
								//We have nothing yet, we take this one as a starting point
								res = &day_planning;
								w = worker;
							}
							else
							{
								//We already have a potential DayPlanning, check if this is better
								if (day_planning.day < res->day)
								{
									res = &day_planning;
									w = worker;
								}
							}
							//No need to go further for this worker
							break;
						}
					}
				}
				return res;
			}
			void streamText_(std::ostream &os) const
			{
				os << "# Planning on " << today() << std::endl << std::endl;
				for (const auto &p: dayPlanningsPerWorker_)
				{
					os << p.first << std::endl;
					for (const auto &p2: p.second)
					{
						if (p2.second.taskParts.empty())
							//We do not break here, maybe some things are planned in the future
							continue;
						os << "\t" << p2.first;
						gubg::OnlyOnce putBehindDate;
						for (const auto &tp: p2.second.taskParts)
						{
							if (!putBehindDate())
								os << "\t          ";
							os << " (" << std::setprecision(2) << std::fixed << tp.sweat << "d) " << tp.task->fullName() << " (work: " << tp.task->sweat << "d)" << std::endl;
						}
					}
				}
			}

			struct CompareStart
			{
				//We first compare the start days. If these are the same, we compare the stop days.
				//If we still have a tie, just use the pointer value itself to make sure different tasks with the same start and stop won't get lost
				bool operator()(const Task::Ptr &lhs, const Task::Ptr &rhs) const {return std::make_tuple(lhs->start, lhs->stop, lhs) < std::make_tuple(rhs->start, rhs->stop, rhs);}
			};
			void streamHtml_(std::ostream &os) const
			{
				using namespace gubg::xml::builder;
				Tag html(os, "html", NoShortClose);
				auto body = html.tag("body");
				{
					auto h1 = body.tag("h1");
					h1 << "Planning on " << today();
				}
				auto table = body.tag("table");
				table.attr("border", 0).attr("cellpadding", 0).attr("cellspacing", 0);

				Day globalFirst, globalLast;
				for (const auto &p: dayPlanningsPerWorker_)
				{
					const auto &dayPlannings = p.second;
					for (const auto &p2: dayPlannings)
					{
						if (!globalFirst.isValid() || ((p2.second.sweat - p2.second.availableSweat()) > eps_() && p2.second.day < globalFirst))
							globalFirst = p2.second.day;
						if (!globalLast.isValid() || ((p2.second.sweat - p2.second.availableSweat()) > eps_() && p2.second.day > globalLast))
							globalLast = p2.second.day;
					}
				}

				for (const auto &p: dayPlanningsPerWorker_)
				{
					const auto worker = p.first;
					const auto &dayPlannings = p.second;

					table.tag("tr").tag("th") << worker;

					Day start = globalFirst;
					Day stop = globalLast;

					//The header with the days
					{
						auto tr = table.tag("tr");
						tr.tag("th") << "date";
						Day firstOfMonth, prev;
						int nr;
						bool toggle = true;
						for (auto d = start; d <= stop; ++d)
						{
							if (!firstOfMonth.isValid())
							{
								firstOfMonth = start;
								nr = 1;
							}
							else
							{
								if (d.day() == 1)
								{
									tr.tag("th").attr("bgcolor", (toggle ? "yellow" : "orange")).attr("colspan", nr) << firstOfMonth << " " << prev;
									firstOfMonth = d;
									nr = 0;
									toggle = !toggle;
								}
								++nr;
							}
							prev = d;
						}
					}

					//Determine the set of all tasks this workers has on its schedule, and sort them based on the start date
					typedef std::set<Task::Ptr, CompareStart> Tasks;
					Tasks tasks;
					for (const auto &p2: dayPlannings)
						for (const auto &tp: p2.second.taskParts)
							tasks.insert(tp.task);

					//Add the load for this day
					{
						auto tr = table.tag("tr");
						tr.tag("td").attr("bgcolor", "grey") << "LOAD";
						for (auto d = start; d <= stop; ++d)
						{
							auto it = dayPlannings.find(d);
							auto td = tr.tag("td");
							if (it == dayPlannings.end())
								td.attr("bgcolor", "grey");
							else
							{
								const auto sweat = it->second.sweat - it->second.availableSweat();
								if (sweat <= 0.2)
									td.attr("bgcolor", "blue");
								else if (sweat <= 0.5)
									td.attr("bgcolor", "orange");
								else
									td.attr("bgcolor", "black");
							}
							td << "&nbsp;";
						}
					}

                    std::string task_basename;
                    size_t stripe_ix = 1;
                    const char *greens[2] = {"green", "darkgreen"};
                    const char *reds[2] = {"red", "darkred"};
                    const char *whites[2] = {"white", "lightgrey"};

					for (auto task: tasks)
					{
                        const unsigned int nr_levels = 4;
                        const auto basename = task->base_name(nr_levels);
                        const auto restname = task->base_name(-1, nr_levels);

                        //Handle striping
                        if (basename != task_basename)
                        {
                            task_basename = basename;
                            stripe_ix = (stripe_ix+1)%2;
                        }

						auto tr = table.tag("tr");
                        {
                            auto cell = tr.tag("td");
                            cell.attr("nowrap", "").attr("bgcolor", whites[stripe_ix]);
                            { cell.tag("b") << basename; }
                            { cell << restname; }
                        }
						for (auto d = start; d <= stop; ++d)
						{
							auto it = dayPlannings.find(d);
							auto td = tr.tag("td");
							if (it == dayPlannings.end())
								td.attr("bgcolor", "grey");
							else
							{
								auto &dayPlanning = it->second;
								auto tp = std::find_if(dayPlanning.taskParts.begin(), dayPlanning.taskParts.end(), [&task](const TaskPart &tp){return tp.task == task;});
								if (tp == dayPlanning.taskParts.end())
								{
									if (dayPlanning.sweat > eps_())
										td.attr("bgcolor", greens[stripe_ix]);
									else
										td.attr("bgcolor", "pink");
								}
								else
									td.attr("bgcolor", reds[stripe_ix]);
							}
							td << "&nbsp;";
						}
					}
				}
			}

			static Sweat eps_() {return 0.0001;}
	};

	inline std::ostream &operator<<(std::ostream &os, const Planning &pl)
	{
		pl.stream(os);
		return os;
	}

} }

#endif
