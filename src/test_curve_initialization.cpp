# ifndef SNARK_COMPILE
# define SNARk_COMPILE

# include <libff/common/profiling.hpp>
# include <cycles/basic_aggregation_system.tcc>
# include <trees/tree.hpp>
//# include "gtest/gtest.h"

void initialize() 
{

    // Decreases libsnark verbosity
    libff::inhibit_profiling_info = true;
    libff::inhibit_profiling_counters = true;

    dual_basT::initialize_curves();
    std::cout << "Done all curves" << std::endl;

    dual_basT::build_circuits();
    std::cout << "Done all circuits" << std::endl;

    auto& forward = forwardT::get_instance();
    auto& backward = backwardT::get_instance();

    // Run generator or Load generated
    std::cout << "Loading forward" << std::endl;
    forward.with_name("forward").at_dir("../setup").with_crs();

    std::cout << "Loading backward" << std::endl;
    backward.with_name("backward").at_dir("../setup").with_crs();

}

void aggregate_serialize_deserialize_verify() 
{

    auto& forward = forwardT::get_instance();
    auto& backward = backwardT::get_instance();

    std::cout << "Example circuit" << std::endl;
    auto example_tree = tree<dual_basT, 0>::as_example(50);

    std::vector<tree<dual_basT, 0>> children{ example_tree, example_tree };

    std::cout << "Constructing the proof" << std::endl;
    auto aggregated_tree = tree<dual_basT, 1>::from_aggregation(children);

    std::cout << "Aggregating proofs" << std::endl;
    aggregated_tree.aggregate_inputs().aggregate_proofs();

    std::cout << "Tests the streaming : ";
    std::stringstream ss;
    ss << aggregated_tree;
    tree<dual_basT, 1> res;
    ss >> res;
    res.aggregate_inputs();
    std::cout << res.verify() << std::endl;

    std::cout << "Serializing the proofs" << std::endl;
    unsigned char * serialized = aggregated_tree.to_string();
    
    std::cout << "Deserializing the proofs" << std::endl;
    auto deserialized = tree<dual_basT, 1>::from_string(serialized);
    deserialized.aggregate_inputs();

    std::cout << "Valid proof : " << std::endl; 
    std::cout << deserialized.verify() << std::endl;

}

int main()
{
    initialize();
    aggregate_serialize_deserialize_verify();
    return 0;
}

# endif
