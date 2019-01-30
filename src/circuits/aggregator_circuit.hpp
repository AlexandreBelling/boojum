# ifndef AGGREGATOR_CIRCUIT_HPP
# define AGGREGATOR_CIRCUIT_HPP

#include "libsnark/gadgetlib1/gadget.hpp"
#include "libsnark/gadgetlib1/gadgets/verifiers/r1cs_ppzksnark_verifier_gadget.hpp"
#include "libsnark/gadgetlib1/gadgets/hashes/crh_gadget.hpp"
#include "libsnark/gadgetlib1/gadgets/basic_gadgets.hpp"
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>


using namespace libsnark;

template<typename from, typename to>
class aggregator_circuit {
public:

    using to_field = libff::Fr<to>;
    using from_field = libff::Fr<from>;

    protoboard<to_field> pb;

    size_t aggregation_arity;

    pb_variable_array<to_field> aggregator_input_var;

    std::vector<pb_variable_array<to_field>> verifier_inputs_bits;

    std::vector<pb_variable_array<to_field>> verification_keys_bits;
    std::vector<r1cs_ppzksnark_verification_key_variable<to>> verification_keys;

    std::vector<r1cs_ppzksnark_proof_variable<to>> proofs;
    std::vector<pb_variable_array<to_field>> proofs_contents;
    std::vector<multipacking_gadget<to_field> > unpack_proofs;
    std::vector<pb_variable_array<to_field>> proofs_bits;

    std::vector<pb_variable_array<to_field>> block_inputs_vars;
    std::shared_ptr<block_variable<to_field>> block_for_input_verification;
    std::shared_ptr<CRH_with_field_out_gadget<to_field>> hash_incoming_proof;

    pb_variable<to_field> verification_result;
    std::vector<r1cs_ppzksnark_verifier_gadget<to>> verifiers;

    aggregator_circuit(size_t aggregation_arity);
    void generate_r1cs_constraints();
    void generate_r1cs_witness(
        std::vector<r1cs_ppzksnark_verification_key<from>> verification_key_value,
        std::vector<r1cs_ppzksnark_primary_input<from>> verifier_input_values,
        std::vector<r1cs_ppzksnark_proof<from>> proof_value
    );
    void generate_primary_inputs(
        std::vector<r1cs_ppzksnark_verification_key<from>> verification_key_value,
        std::vector<r1cs_ppzksnark_primary_input<from>> verifier_input_values,
        std::vector<r1cs_ppzksnark_proof<from>> proof_value
    );
};

#include "aggregator_circuit.tcc"

# endif