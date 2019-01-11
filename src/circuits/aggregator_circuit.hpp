# ifndef AGGREGATOR_CIRCUIT_HPP
# define AGGREGATOR_CIRCUIT_HPP

#include "libsnark/gadgetlib1/gadget.hpp"
#include "libsnark/gadgetlib1/gadgets/verifiers/r1cs_ppzksnark_verifier_gadget.hpp"
#include "libsnark/gadgetlib1/gadgets/hashes/crh_gadget.hpp"
#include "libsnark/gadgetlib1/gadgets/basic_gadgets.hpp"
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>


using namespace libsnark;

template<typename ppT, typename other_curve>
class aggregator_circuit {
public:
    typedef libff::Fr<ppT> FieldT;
    typedef libff::Fr<other_curve> other_field;

    protoboard<FieldT> pb;

    size_t aggregation_arity;

    pb_variable_array<FieldT> aggregator_input_var;

    std::vector<pb_variable_array<FieldT>> verifier_inputs_bits;

    std::vector<pb_variable_array<FieldT>> verification_keys_bits;
    std::vector<r1cs_ppzksnark_verification_key_variable<ppT>> verification_keys;

    std::vector<r1cs_ppzksnark_proof_variable<ppT>> proofs;
    std::vector<pb_variable_array<FieldT>> proofs_contents;
    std::vector<multipacking_gadget<FieldT> > unpack_proofs;
    std::vector<pb_variable_array<FieldT>> proofs_bits;

    std::vector<pb_variable_array<FieldT>> block_inputs_vars;
    std::shared_ptr<block_variable<FieldT>> block_for_input_verification;
    std::shared_ptr<CRH_with_field_out_gadget<FieldT>> hash_incoming_proof;

    pb_variable<FieldT> verification_result;
    std::vector<r1cs_ppzksnark_verifier_gadget<ppT>> verifiers;

    aggregator_circuit(size_t aggregation_arity);
    void generate_r1cs_constraints();
    void generate_r1cs_witness(
        std::vector<r1cs_ppzksnark_verification_key<other_curve>> verification_key_value,
        std::vector<std::vector<other_field>> verifier_input_values,
        std::vector<r1cs_ppzksnark_proof<other_curve>> proof_value
    );
};

#include "aggregator_circuit.tcc"

# endif