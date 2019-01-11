#include "aggregator/aggregator_circuit.hpp"
#include "libff/algebra/curves/mnt/mnt4/mnt4_pp.hpp"
#include "libff/algebra/curves/mnt/mnt6/mnt6_pp.hpp"
#include "libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp"

using namespace libsnark;
using namespace std;

#define NDEBUG
#define DEBUG

using curve_A_pp = libff::mnt4_pp;
using curve_B_pp = libff::mnt6_pp;

int main() {

    curve_A_pp::init_public_params();
    curve_B_pp::init_public_params();

    using Field_A = libff::Fr<curve_A_pp>;
    using Field_B = libff::Fr<curve_B_pp>;

    /**
     * @brief Generate example circuit
     */
    const size_t num_constraints = 50;
    const size_t primary_input_size = 1;

    r1cs_example<Field_B> example = generate_r1cs_example_with_field_input<Field_B>(num_constraints, primary_input_size);
    const r1cs_ppzksnark_keypair<curve_B_pp> keypair = r1cs_ppzksnark_generator<curve_B_pp>(example.constraint_system);
    const r1cs_ppzksnark_proof<curve_B_pp> pi = r1cs_ppzksnark_prover<curve_B_pp>(
        keypair.pk, example.primary_input, 
        example.auxiliary_input
    );

    /**
     * @brief Build aggregator circuit
     * 
     */
    aggregator_circuit<curve_A_pp, curve_B_pp> circuit(2);
    circuit.generate_r1cs_constraints();

    vector<r1cs_ppzksnark_verification_key<curve_B_pp>> vks;
    vector<r1cs_primary_input<Field_B>> inputs;
    vector<r1cs_ppzksnark_proof<curve_B_pp>> proofs;

    for(size_t k=0; k<2; k++)
    {
        vks.emplace_back(keypair.vk);
        inputs.emplace_back(example.primary_input);
        proofs.emplace_back(pi);
    }

    circuit.generate_r1cs_witness(vks, inputs, proofs);

    /**
     * @brief Run generator
     * 
     */
    r1cs_constraint_system<Field_A> aggregator_constraint_system = circuit.pb.get_constraint_system();
    r1cs_ppzksnark_keypair<curve_A_pp> aggregator_key_pair = r1cs_ppzksnark_generator<curve_A_pp>(aggregator_constraint_system);


    /**
     * @brief Run prover and verifier on valid proof
     * 
     */
    r1cs_ppzksnark_proof<curve_A_pp> aggregation_proof = r1cs_ppzksnark_prover<curve_A_pp>(
        aggregator_key_pair.pk, 
        circuit.pb.primary_input(), 
        circuit.pb.auxiliary_input()
    );
    bool aggregation_verified = r1cs_ppzksnark_verifier_strong_IC<curve_A_pp>(
        aggregator_key_pair.vk, 
        circuit.pb.primary_input(), 
        aggregation_proof
    );
    assert(aggregation_verified);

    /**
     * @brief Do it with a fakeproof
     * 
     */
    circuit.pb.val(circuit.aggregator_input_var[0]) = 0;
    assert(!circuit.pb.is_satisfied());

    return 0;
}