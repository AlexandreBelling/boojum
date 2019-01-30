# ifndef AGGREGATION_SYSTEM_HPP
# define AGGREGATION_SYSTEM_HPP

# include <stdlib.h>
# include <cycles/cycles.hpp>
# include <cycles/aggregation.hpp>

/**
 * @brief A basic aggregation system only supports aggregations
 * of the same arities
 * 
 * @tparam cycle 
 * @tparam arities 
 */
template<typename cycle, std::size_t Arity>
class basic_aggregation_system {

public:

    basic_aggregation_system() = delete;

public:

    template<std::size_t N>
    class get_aggregation {
    public:
        using type = aggregation<cycle, N % cycle::size, Arity>;
        using fromT = typename type::fromT;
        using toT = typename type::toT;
    };

    using cycleT = cycle;
    static constexpr std::size_t arity = Arity;

    static void initialize_curves()
    {
# if NDEBUG
    std::cout << "Initializing curves : size? " << cycle::size << std::endl;
# endif
        _init_curve<cycle::size - 1>();
    }

    static void build_circuits()
    {
        _init_circuits<cycle::size - 1>();
    }

    template<std::size_t I> 
    static void _init_curve()
    {

# ifdef NDEBUG
        std::cout << "With curve " << I << std::endl;
# endif

        get_aggregation<I>::fromT::init_public_params();
        _init_curve<I-1>();
    }

    template<std::size_t I> 
    static void _init_circuits()
    {

# ifdef NDEBUG
        std::cout << "With circuit " << I << std::endl;
# endif

        get_aggregation<I>::type::get_instance().with_circuit();
        _init_circuits<I-1>();
    }

};

// template<>
// template<typename cycle, std::size_t Arity>
// void basic_aggregation_system<cycle, Arity>::_init_curve<(std::size_t) 0>()
// {
//     get_aggregation<0>::fromT::init_public_params();
// }

// template<>
// template<typename cycle, std::size_t Arity>
// void basic_aggregation_system<cycle, Arity>::_init_circuits<0>()
// {
//     get_aggregation<I>::type::get_instance().with_circuit();
// }

# endif