#ifndef NODE_HPP
#define NODE_HPP

# include <cycles/aggregation.hpp>
# include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
# include <circuits/aggregator_circuit.hpp>
# include <libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp>

template<typename aggregationT>
class node {

public:

    using fromT         = typename aggregationT::fromT;
    using toT           = typename aggregationT::toT;
    using field_fromT   = typename fromT::Fp_type;
    using field_toT     = typename toT::Fp_type;

public:

    node() : leaf(false) {
        verification_key = aggregationT::get_instance().keypair->vk;
    }

    node(
        libsnark::r1cs_ppzksnark_proof<toT> &_proof
    ) : 
        leaf(false) 
    {
        proof               = _proof;
        verification_key    = aggregationT::get_instance().keypair->vk;
    }

    node(
        r1cs_ppzksnark_primary_input<toT>       &_primary_input,
        r1cs_ppzksnark_verification_key<toT>    &_verification_key,
        r1cs_ppzksnark_proof<toT>               &_proof
    ):
        leaf(true) 
    {
        primary_input       = _primary_input;
        verification_key    = _verification_key;
        proof               = _proof;
    }

public:

    bool leaf;
    
    r1cs_ppzksnark_primary_input<toT> primary_input;
    r1cs_ppzksnark_verification_key<toT> verification_key;
    r1cs_ppzksnark_proof<toT> proof;

    static node<aggregationT> as_example(std::size_t num_constraints);

    static node<aggregationT> from_leaf(
        r1cs_ppzksnark_primary_input<toT>       _primary_input,
        r1cs_ppzksnark_verification_key<toT>    _verification_key,
        r1cs_ppzksnark_proof<toT>               _proof
    ) {
        node<aggregationT> res(
            _primary_input,
            _verification_key,
            _proof
        );

        return res;
    };

    static node<aggregationT> from_aggregation(
        r1cs_ppzksnark_proof<toT> _proof
    ) {
        return node<aggregationT>(
            _proof
        );
    }

    node<aggregationT>& set_inputs(
        r1cs_ppzksnark_primary_input<toT> new_primary_input
    ) {
        primary_input = new_primary_input;
        return *this;
    }

    node<aggregationT>& set_proof(
        r1cs_ppzksnark_proof<toT> new_proof
    ) {
        proof = new_proof;
        return *this;
    }

    bool verify(); 

};

# include <trees/node.tcc>

#endif