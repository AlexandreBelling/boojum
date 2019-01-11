# ifndef SNARK_TEST_TCC_
# define SNARK_TEST_TCC_

# include <stdlib.h>
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

    // Get key pairs
    tool_AB.fetch_crs("./setup");
    tool_BA.fetch_crs("./setup");
}

void test_serializer_node_proofs(){

    // Generate a small example
    node_proof<curve_B_pp, curve_A_pp> node(20);

    // Witness test that the example is working
    assert(node.verify());

    // Going to aggregate the example with itself
    std::vector<node_proof<curve_B_pp, curve_A_pp>> children(2);
    children[0] = node;
    children[1] = node;

    // Run aggregation : Throws if the inputs node_proof are incorrect
    auto aggnode = node_proof<curve_A_pp, curve_B_pp>::from_aggregation(
        children
    );
    std::cout << "aggnode correct ? " << aggnode.verify() << endl;
    std::cout << "aggnode leaf ? " << aggnode.leaf << endl;

    // ==========================================

    // Unsigned char
    std::cout << string(50, '=') << endl << "UNSIGNED CHAR" << endl << string(50, '=') << endl;

    // Serialize the node_proof
    std::cout << string(50, '=') << endl << "Serialization" << endl << endl;
    unsigned char * aggnode_bytes;
    aggnode.to_string(aggnode_bytes);
    std::cout << "^^^ Was aggnode " << std::endl;
    
    // std::cout << string(50, '=') << endl;
    // int i = 0;
    // for(unsigned char* p=aggnode_bytes; *p; p++) {
    //     i++;
    // }
    // std::cout << aggnode_bytes << endl;
    // std::cout << string(50, '=') << endl;
    // std::cout << i << endl;

    // Deserialize the node proof
    std::cout << string(50, '=') << endl << "Deserialization" << endl << endl;
    node_proof<curve_A_pp, curve_B_pp> deserialized_bytes;
    deserialized_bytes.from_string(aggnode_bytes);

    // ===========================================

    // Strings
    std::cout << string(50, '=') << endl << "STRINGS" << endl << string(50, '=') << endl;

    // Serialize the node_proof
    std::cout << string(50, '=') << endl << "Serialization" << endl << endl;
    std::string aggnode_str;
    aggnode.to_string(aggnode_str);
    
    std::cout << string(50, '=') << endl << aggnode_str << endl;

    // Deserialize the node proof
    std::cout << string(50, '=') << endl << "Deserialization" << endl << endl;
    node_proof<curve_A_pp, curve_B_pp> deserialized_str;
    deserialized_str.from_string(aggnode_str);

    // ===========================================

    // Test that the proof is still correct with proof
    bool success = r1cs_ppzksnark_verifier_strong_IC<curve_A_pp>(
        aggnode.verification_key,
        aggnode.primary_input,
        deserialized_bytes.proof
    );
    std::cout << "Ok proof" << success << endl;

    // Test that the proof is still correct with input
    
    success = r1cs_ppzksnark_verifier_strong_IC<curve_A_pp>(
        aggnode.verification_key,
        deserialized_bytes.primary_input,
        aggnode.proof
    );
    std::cout << "Ok input" << success << endl;

    // Test that the proof is still correct with verification key
    success = r1cs_ppzksnark_verifier_strong_IC<curve_A_pp>(
        deserialized_bytes.verification_key,
        aggnode.primary_input,
        aggnode.proof
    );
    std::cout << "Ok verification key" << success << endl;

}

int main(){
    initialize();
    test_serializer_node_proofs();
    return 0;
}

# endif