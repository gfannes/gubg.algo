#ifndef HEADER_gubg_pid_Controller_hpp_ALREADY_INCLUDED
#define HEADER_gubg_pid_Controller_hpp_ALREADY_INCLUDED

namespace gubg { namespace pid { 

    template <typename Receiver, typename T>
    class Controller
    {
    public:
        T time() const {return time_;}

        void process(T dt)
        {
            //Ask the user for updated coefs, if needed
            receiver_().pid_coefs(p_, i_, d_, time_);

            //Compute the control value: PID
            const T u = p_*prop_ + i_*int_ + d_*diff_;

            //Ask the user to simulate the system and return the new error
            const T new_error = receiver_().pid_simulate(time_, dt, u);

            time_ += dt;

            //Update the proportional, integral and differential, to be used in the next iteration
            int_ += new_error*dt;
            diff_ = (new_error - prop_)/dt;
            prop_ = new_error;
        }
    private:
        Receiver &receiver_(){return static_cast<Receiver&>(*this);}

        T time_{};

        T prop_{};
        T int_{};
        T diff_{};

        T p_{};
        T i_{};
        T d_{};
    };

} } 

#endif
