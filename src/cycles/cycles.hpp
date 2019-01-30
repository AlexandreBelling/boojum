# ifndef CYCLE_HPP_
# define CYCLE_HPP_

# include<tuple>
# include<stdlib.h>

/**
 * @brief This function represents
 * 
 * @tparam ppTs 
 */
template<typename... ppTs>
class cycle {
    
public:

    cycle() = delete;

public:

    static constexpr std::size_t size = std::tuple_size<std::tuple<ppTs...>>::value;

    template<std::size_t N>
    struct get_curve 
    {
        using type = typename std::tuple_element<
                N % size, 
                std::tuple<ppTs...>
            >::type;
    };

};

# endif