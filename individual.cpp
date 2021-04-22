#include "individual.h"

#include <algorithm>
#include <cassert>

individual::individual(std::vector<int> net_arch, int age) :
  m_age{age},
  m_input_values(net_arch[0], 1.0),
  m_network{net_arch}
{

}

std::vector<double> response(const individual& ind)
{
  return response(ind.get_net(),ind.get_input_values());
}

void test_individual()
{
  //An individual has a member variable called m_age
  //By default initialized to 0
  {
    individual i;
    assert(i.get_age() == 0);

    int age = 5;
    individual i2{{1},age};
    assert(i2.get_age() == age);
  }

  ///An indiivdual is initialized with a network architecture,
  /// by default 1,2,1
  {
    std::vector<int> net_arch{1,2,1};
    individual i{net_arch};
    assert(i.get_net() == network{net_arch});
  }

  ///Individuals have a vector of fixed input values, always equal to 1, for their network
  {
    int n_input = 456;
    std::vector<int> net_arch{n_input};
    individual i{net_arch};
    assert(i.get_input_values().size() == static_cast<size_t>(n_input));
    for(const auto& value : i.get_input_values())
      {
        assert(are_equal_with_tolerance(value, 1.0));
      }
  }

  ///When an individual responds to environment it uses its input values as input
  {
    individual i;
    assert( response(i) == response(i.get_net(),i.get_input_values()));
  }
}
