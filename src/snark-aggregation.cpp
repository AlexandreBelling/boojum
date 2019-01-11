# ifndef SNARK_IMPL_TCC_
# define SNARK_IMPL_TCC_

# include <stdlib.h>
# include "snark-aggregation.h"
# include <iostream>
# include "libff/common/profiling.hpp"
# include "libff/algebra/curves/mnt/mnt4/mnt4_pp.hpp"
# include "libff/algebra/curves/mnt/mnt6/mnt6_pp.hpp"
# include "circuits/aggregator_circuit.hpp"
# include "libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp"
# include "serialize.hpp"
# include "tools.hpp"
# include "node-proof.hpp"

using curve_A_pp = libff::mnt4_pp;
using curve_B_pp = libff::mnt6_pp;

using Field_A = libff::Fr<curve_A_pp>;
using Field_B = libff::Fr<curve_B_pp>;

void initialize() {

    // Decreases libsnark verbosity
    libff::inhibit_profiling_info = true;
    libff::inhibit_profiling_counters = true;

    // Instantiate curve parameters
    curve_A_pp::init_public_params();
    curve_B_pp::init_public_params();

    auto& tool_AB = tools<curve_A_pp, curve_B_pp>::get_instance();
    auto& tool_BA = tools<curve_B_pp, curve_A_pp>::get_instance();

    // Instantiate the aggregation zksnark circuit cycle
    tool_AB.initialize("ab");
    tool_BA.initialize("ba");

}

// Initialize should be run as a prerequisite of generation no matter what
void run_generators(const char* path) {;

    const std::string path_str = std::string(path);

    auto& tool_AB = tools<curve_A_pp, curve_B_pp>::get_instance();
    auto& tool_BA = tools<curve_B_pp, curve_A_pp>::get_instance();

    // Fetch the CRS and save them if they were generated
    tool_AB.fetch_crs("setup", false);
    tool_BA.fetch_crs("setup", false);
}

void make_example_proof(
    void* node_proof_buff
) {
    unsigned char *node_proof_char = reinterpret_cast<unsigned char *>(node_proof_buff);
    
    const size_t num_constraints = 50;
    node_proof<curve_A_pp, curve_B_pp> example_AB(num_constraints);
    std::string node_proof_str;
    example_AB.to_string(node_proof_str);

    // Unsafe passing to the output buffer
    for(int i = 0; i < node_proof_str.size(); i++) {
        node_proof_char[i] = node_proof_str[i];
    }
}

// Takes 2 nodes proof from other curve to make a node proof on curve
template <typename curve, typename other_curve>
void prove_aggregation(
    void* output_node_buff,
    void* left_node_buff,
    void* right_node_buff
) {
    unsigned char *output_node_char = reinterpret_cast<unsigned char *>(output_node_buff);
    unsigned char *left_node_char = reinterpret_cast<unsigned char *>(left_node_buff);
    unsigned char *right_node_char = reinterpret_cast<unsigned char *>(right_node_buff);

    std::vector<node_proof<other_curve, curve>> children(2);
    children[0].from_string(left_node_char);
    children[1].from_string(right_node_char);

    auto output_proof = node_proof<curve, other_curve>::from_aggregation(children);
    output_proof.to_string(output_node_char);
}

// External non-templated functions
void prove_aggregation(
    bool is_ab,
    void* output_node_buff,
    void* left_node_buff,
    void* right_node_buff
) {
    if(is_ab) {
        prove_aggregation<curve_A_pp, curve_B_pp>(
            output_node_buff,
            left_node_buff,
            right_node_buff
        );
    } else {
        prove_aggregation<curve_A_pp, curve_B_pp>(
            output_node_buff,
            left_node_buff,
            right_node_buff
        );
    }
}

template<typename curve, typename other_curve>
bool verify(
    void* node_buff
) {
    unsigned char *node_char = reinterpret_cast<unsigned char *>(node_buff);
    node_proof<other_curve, curve> node;
    node.from_string(node_char);
    return node.verify();
}

bool verify(
    bool is_ab,
    void* node_buff
){
    if(is_ab){
        return verify<curve_A_pp, curve_B_pp>(node_buff);
    } else {
        return verify<curve_B_pp, curve_A_pp>(node_buff);
    }
}

#endif