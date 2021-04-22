#include "simulation.h"

#include <cassert>
#include <vector>


simulation::simulation(//double targetA, double targetB,
                       int init_pop_size,
                       int seed,
                       int t_change_interval,
                       std::vector<int> net_arch):
    m_environment{/*targetA, targetB*/},
    m_population{net_arch, init_pop_size},
    m_rng{seed},
    m_t_change_env_distr{1.0/static_cast<double>(t_change_interval)}
{

}

void change_current_target_value(simulation& s, double new_target_value)
{
  get_current_env_value(s) = new_target_value;
}
void change_nth_ind_net(simulation& s, size_t ind_index, const network& n)
{
  get_nth_ind_net(s, ind_index) = n;
}

double get_current_env_value(const simulation&s)
{
  return s.get_env().get_current_target_value();
}

double& get_current_env_value(simulation&s)
{
  return s.get_env().get_current_target_value();
}

const std::vector<individual>& get_inds(const simulation&s)
{
  return s.get_pop().get_inds();
}

const individual& get_nth_ind(const simulation& s, size_t ind_index)
{
  return get_nth_ind(s.get_pop(), ind_index);
}

const network& get_nth_ind_net(const simulation& s, size_t ind_index)
{
  return get_nth_ind_net(s.get_pop(), ind_index);
}

network& get_nth_ind_net( simulation& s, size_t ind_index)
{
  return get_nth_ind_net(s.get_pop(), ind_index);
}

void test_simulation() noexcept//!OCLINT test may be many
{

    ///A simulation has a member of type population
    ///The population has a vector of individuals of size 1 by default
    {
        simulation s;
        assert(s.get_pop().get_inds().size() == 1u);
    }

#ifdef FIX_ISSUE_27
    ///A simulation can be initialized by
    /// target values for the environment
    /// and the number of individuals in
    /// the populaation
    {
        double target_valueA = 1.23456;
        double target_valueB = 6.12345;
        int init_pop_size = 123456;
        simulation s{target_valueA, target_valueB ,init_pop_size};
        assert(s.get_env().get_target_valueA() == target_valueA
               && s.get_env().get_target_valueB() == target_valueB
               && s.get_pop().get_ind_vec().size() == static_cast<unsigned int>(init_pop_size));
    }
#endif
    ////A simulation should have a random engine, intialized with a seed that is fed to simulation

    {
//      double targetA = 1.0;
//      double targetB = 0;
        int pop_size = 1;
        int seed = 123456789;
        simulation s{//targetA, targetB
          pop_size,seed};

        std::minstd_rand copy_rng(seed);
        assert ( s.get_rng()() == copy_rng());

    }

    ///A simulation should have a t_change_env_distr bernoulli distribution
    /// that determines the interval of environmental change
    /// Default initialization value is 10
    {
//      double targetA = 1.0;
//      double targetB = 0;
        int pop_size = 1;
        int seed = 123456789;
        int t_change_interval = 20;

        simulation s {// targetA, targetB,
              pop_size, seed, t_change_interval};
        std::bernoulli_distribution mockdistrotchange(1.0 / static_cast<double>(t_change_interval));
        assert (s.get_t_change_env_distr() == mockdistrotchange);

    }


#ifdef FIX_ISSUE_20
  //A simulation has an absolute counter and you can access it
  {
    simulation s;
    assert(s.get_time() > 0 | s.get_time() <= 0);
  }
#endif

#ifdef FIX_ISSUE_26
  //Every tick simulation timer increases by one
  {
    simulation s;
    auto init_timer_value = s.get_time();
    tick(s);
    assert(s.get_time == init_timer_value + 1);

    simulation s2;
    init_timer_value = s2.get_time();
    int repeats = 123;
    for(int i = 0; i != repeats; i++)
      {
        tick(s);
      }
    assert(s2.get_time() == init_timer_value + repeats);
  }
#endif

  ///Simulation can be initialized with network architecture for inds in pop
  {
    std::vector<int> net_arch{1,33,2,1};
    simulation s{1,0,0, net_arch};

    assert(get_nth_ind_net(s, 0) == network{net_arch});
  }

///individuals in a pop are selected based on how closely they match the current_env_target_value
  {
    int pop_size = 2;
    simulation s{pop_size};

    //change ind 0 net
    auto new_net = change_all_weights(network{{1,2,1}}, 1);
    change_nth_ind_net(s, 0, new_net);

    //change target value to match output of ind 0 net
    change_current_target_value(s, response(get_nth_ind(s, 0))[0]);

    assert(response(get_nth_ind(s, 0))[0] == get_current_env_value(s));
    assert(response(get_nth_ind(s, 1))[0] != get_current_env_value(s));

    select_inds(s);

    //all inds should now have the network that matches the target values
    for(const auto& ind :get_inds(s))
      {
        assert(ind.get_net() == new_net);
      }
  }
}
