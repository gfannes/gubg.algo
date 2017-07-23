#ifndef HEADER_gubg_pid_Controller_hpp_ALREADY_INCLUDED
#define HEADER_gubg_pid_Controller_hpp_ALREADY_INCLUDED

namespace gubg { namespace pid { 

    template <typename T>
    class Controller
    {
    public:
        T p() const {return p_;}
        T i() const {return i_;}
        T d() const {return d_;}

        void p(T v) {p_ = v;}
        void i(T v) {i_ = v;}
        void d(T v) {d_ = v;}

        T u() const {return u_;}

        //error: target - actual
        void update(T dt, T error)
        {
            //Update the proportional, integral and differential, to be used in the next iteration
            int_ += error*dt;
            diff_ = (error - prop_)/dt;
            prop_ = error;

            //Compute the control value: PID
            u_ = p_*prop_ + i_*int_ + d_*diff_;
        }

    private:
        T p_{};
        T i_{};
        T d_{};

        T prop_{};
        T int_{};
        T diff_{};

        T u_{};
    };

} } 

#endif
