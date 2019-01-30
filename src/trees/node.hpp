#ifndef NODE_HPP
#define NODE_HPP

# include <cycles/aggregation.hpp>
# include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
# include <circuits/aggregator_circuit.hpp>
# include <libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp>

template<typename aggregation>
class node {

public:

    using fromT         = typename aggregation::fromT;
    using toT           = typename aggregation::toT;
    using field_fromT   = typename fromT::Fp_type;
    using field_toT     = typename toT::Fp_type;

public:

    node() : leaf(false) {}

    node(r1cs_ppzksnark_proof<toT>& _proof) :
        leaf(false)
    {
        proof.reset(_proof);
        verification_key.reset(
            aggregation::get_instance().key_pair->vk
        );
    }

    node(
        r1cs_ppzksnark_primary_input<toT>       &_primary_input,
        r1cs_ppzksnark_verification_key<toT>    &_verification_key,
        r1cs_ppzksnark_proof<toT>               &_proof
    ):
        leaf(true) 
    {
        primary_input       = std::make_shared<r1cs_ppzksnark_primary_input<toT>>(_primary_input);
        verification_key    = std::make_shared<r1cs_ppzksnark_verification_key<toT>>(_verification_key);
        proof               = std::make_shared<r1cs_ppzksnark_proof<toT>>(_proof);
    }

public:

    bool leaf;
    
    std::shared_ptr<r1cs_ppzksnark_primary_input<toT>>      primary_input;
    std::shared_ptr<r1cs_ppzksnark_verification_key<toT>>   verification_key;
    std::shared_ptr<r1cs_ppzksnark_proof<toT>>              proof;

    static node<aggregation> as_example(std::size_t num_constraints);

    static node<aggregation> from_leaf(
        r1cs_ppzksnark_primary_input<toT>       _primary_input,
        r1cs_ppzksnark_verification_key<toT>    _verification_key,
        r1cs_ppzksnark_proof<toT>               _proof
    ) {
        return node(
            _primary_input,
            _verification_key,
            _proof
        );
    };

    static node<aggregation> from_aggregation(
        r1cs_ppzksnark_proof<toT> _proof
    ) {
        return node(
            _proof
        );
    }

    node<aggregation>& set_inputs(
        r1cs_ppzksnark_primary_input<toT> new_primary_input
    ) {
        primary_input = make_shared<
            r1cs_ppzksnark_primary_input<toT>
        >(new_primary_input);
        return *this;
    }

    node<aggregation>& set_proof(
        r1cs_ppzksnark_proof<toT> new_proof
    ) {
        std::cout << "Getting in set proof" << std::endl;
        proof = make_shared<
            r1cs_ppzksnark_proof<toT>
        >(new_proof);
        return *this;
    }

    bool verify(); 

};

# include <trees/node.tcc>

#endif