# include <libff/algebra/curves/mnt/mnt4/mnt4_pp.hpp>
# include <libff/algebra/curves/mnt/mnt6/mnt6_pp.hpp>

# include <cycles/cycles.hpp>
# include <cycles/aggregation_system.hpp>
# include <ostream>

using basicCycleT = cycle<libff::mnt4_pp, libff::mnt6_pp>;
using dual_basT = basic_aggregation_system<basicCycleT, 2>;

using forwardT = dual_basT::template get_aggregation<0>::type;
using backwardT = dual_basT::template get_aggregation<1>::type;

template<>
template<>
void basic_aggregation_system<basicCycleT, 2>::_init_curve<0>()
{
# if NDEBUG
    std::cout << "With curve " << 0 << std::endl;
# endif
    dual_basT::get_aggregation<0>::fromT::init_public_params();
}

template<>
template<>
void basic_aggregation_system<basicCycleT, 2>::_init_circuits<0>()
{
# if NDEBUG
    std::cout << "With circuit " << 0 << std::endl;
# endif
    get_aggregation<0>::type::get_instance().with_circuit();
}



