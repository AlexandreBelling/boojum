#ifndef AGGREGATOR_CIRCUIT_TCC
#define AGGREGATOR_CIRCUIT_TCC

#include "libsnark/gadgetlib1/gadget.hpp"
#include "libsnark/gadgetlib1/gadgets/verifiers/r1cs_ppzksnark_verifier_gadget.hpp"
#include "libsnark/gadgetlib1/gadgets/hashes/crh_gadget.hpp"
#include "libsnark/gadgetlib1/gadgets/basic_gadgets.hpp"
#include <libsnark/gadgetlib1/constraint_profiling.hpp>
#include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
#include <iostream>

using namespace std;
using namespace libsnark;

template<typename from, typename to>
aggregator_circuit<from, to>::aggregator_circuit(size_t aggregation_arity):
    aggregation_arity(aggregation_arity)
 {

    /**
     * @brief Manually sets the input, proof and verification key sizes
     *  for a single input circuit
     */
    const size_t field_logsize = to_field::size_in_bits();
    const size_t elt_size = from_field::size_in_bits();

    const size_t digest_size = CRH_with_field_out_gadget<to_field>::get_digest_len();
    const size_t verifier_input_size_in_bits = digest_size * elt_size;

    const size_t vk_size_in_bits = r1cs_ppzksnark_verification_key_variable<to>::size_in_bits(digest_size);
    const size_t proof_size_in_bits = r1cs_ppzksnark_proof_variable<to>::size() * field_logsize;
    
    const size_t block_size = aggregation_arity * (vk_size_in_bits + proof_size_in_bits + verifier_input_size_in_bits);

    aggregator_input_var.allocate(pb, digest_size, "Input of the aggregator circuit");

    /**
     * @brief Setup the wires for the verifier input bits decomposition
     *
     */
    verifier_inputs_bits.resize(aggregation_arity);
    for(uint i=0; i<aggregation_arity; i++)
    {
        verifier_inputs_bits[i].allocate(pb, verifier_input_size_in_bits, "Primary input of the verifier");
    }

    /**
     * @brief Setup the wires for the proof and its bit decomposition
     *
     */
    proofs_bits.resize(aggregation_arity);
    proofs_contents.resize(aggregation_arity);
    for(uint i=0; i<aggregation_arity; i++)
    {
        proofs.emplace_back(r1cs_ppzksnark_proof_variable<to>(pb, "proof"));
        proofs_bits[i].allocate(pb, proof_size_in_bits, "Proof in bits");
        proofs_contents[i].allocate(pb, 
            r1cs_ppzksnark_proof_variable<to>::size(), 
            "Intermediate variable storing the field representation of the proof"
        );
        unpack_proofs.emplace_back(multipacking_gadget<to_field>(pb,
            proofs_bits[i],
            proofs_contents[i],
            field_logsize,
            "Unpack the proof"
        ));
    } 

    /**
     * @brief Setup the wires for the verification key and its bits decomposition
     *
     */
    verification_keys_bits.resize(aggregation_arity);
    for(uint i=0; i<aggregation_arity; i++)
    {
        verification_keys_bits[i].allocate(pb, vk_size_in_bits, "Verification key in bits");
        verification_keys.emplace_back(r1cs_ppzksnark_verification_key_variable<to>(pb,
            verification_keys_bits[i],
            digest_size,
            "Verification key variable"
        ));
    }

    /**
     * @brief Concatenate all hash inputs in a single vector
     *
     */
    for(uint i=0; i<aggregation_arity; i++)
    {
        block_inputs_vars.emplace_back(verification_keys_bits[i]);
        block_inputs_vars.emplace_back(verifier_inputs_bits[i]);
        block_inputs_vars.emplace_back(proofs_bits[i]);
    }

    /**
     * @brief Set up the hash function input
     * 
     */
    block_for_input_verification.reset(new block_variable<to_field>(pb, 
        block_inputs_vars,
        "Block variable for verification key | proof | verifier input")
    );
    assert(block_for_input_verification->bits.size() == block_size);
    hash_incoming_proof.reset(new CRH_with_field_out_gadget<to_field>(pb,
        block_size,
        *block_for_input_verification,
        aggregator_input_var,
        "Hash verification key, proof, verifier input"
    ));

    /**
     * @brief Setup the wires for the verifier
     *
     */
    verification_result.allocate(pb, "Contains the result of the verification");
    for(uint i=0; i<aggregation_arity; i++)
    {
        verifiers.emplace_back(r1cs_ppzksnark_verifier_gadget<to>(pb,
            verification_keys[i],
            verifier_inputs_bits[i],
            elt_size,
            proofs[i],
            verification_result,
            "Verifier gadget"
        ));
    }

    /* Only aggregator_input is set as an input variable as it is the first instanciated variable */
    pb.set_input_sizes(digest_size);
}

template<typename from, typename to>
void aggregator_circuit<from, to>::generate_r1cs_constraints(){

    PROFILE_CONSTRAINTS(pb, "booleanity")
    {
        PROFILE_CONSTRAINTS(pb, "booleanity; unpack the proof")
        {
            for(uint i=0; i<aggregation_arity; i++)
            {
                unpack_proofs[i].generate_r1cs_constraints(true);
            }
        }

        PROFILE_CONSTRAINTS(pb, "booleanity: unpack the verification key")
        {
            for(uint i=0; i<aggregation_arity; i++)
            {
                verification_keys[i].generate_r1cs_constraints(true);
            }
        }
    }

    PROFILE_CONSTRAINTS(pb, "CRH: Hash")
    {
        hash_incoming_proof->generate_r1cs_constraints();
    }

    PROFILE_CONSTRAINTS(pb, "Check that proof lies on the curve")
    {        
        for(uint i=0; i<aggregation_arity; i++)
        {
            proofs[i].generate_r1cs_constraints();
        }
    }

    // TODO: Cache this in an upperlayer proof variable. 
    // Since this is a lot of code for a tiny part of the aggregator
    PROFILE_CONSTRAINTS(pb, "Map proof content"){
        size_t counter;
        for(uint k=0; k<aggregation_arity; k++){
            counter=0;
            for(size_t i=0; i<proofs[0].all_G1_vars.size(); i++)
            {
                for(size_t j=0; j<proofs[0].all_G1_vars[0]->all_vars.size(); j++)
                {
                    pb.add_r1cs_constraint(r1cs_constraint<to_field>(
                        1, 
                        proofs[k].all_G1_vars[i]->all_vars[j], 
                        proofs_contents[k][counter]
                    ));
                    counter++;  
                }
            }
            for(size_t i=0; i<proofs[0].all_G2_vars.size(); i++)
            {
                for(size_t j=0; j<proofs[0].all_G2_vars[0]->all_vars.size(); j++)
                {
                    pb.add_r1cs_constraint(r1cs_constraint<to_field>(
                        1, 
                        proofs[k].all_G2_vars[i]->all_vars[j], 
                        proofs_contents[k][counter]
                    ));
                    counter++;  
                }
            }
        }
        assert(counter == proofs_contents[0].size());
    }

    PROFILE_CONSTRAINTS(pb, "Check that proof is correct")
    {
        for(uint i=0; i<aggregation_arity; i++)
        {
            verifiers[i].generate_r1cs_constraints();
        }
        
    }

    PROFILE_CONSTRAINTS(pb, "Miscellaneous")
    {
        pb.add_r1cs_constraint(r1cs_constraint<to_field>(1, 1 - verification_result, 0), "Enforce valid proof");
    }

    PRINT_CONSTRAINT_PROFILING();
    libff::print_indent(); printf("* Number of constraints in aggregator circuit: %zu\n", pb.num_constraints());
}

template<typename from, typename to>
void aggregator_circuit<from, to>::generate_r1cs_witness(
    std::vector<r1cs_ppzksnark_verification_key<from>> verification_key_value,
    std::vector<r1cs_ppzksnark_primary_input<from>> verifier_input_values,
    std::vector<r1cs_ppzksnark_proof<from>> proof_value
){

    generate_primary_inputs(
        verification_key_value,
        verifier_input_values,
        proof_value
    );

    /**
     * @brief Generate witnesses for the CRH
     * 
     */
    hash_incoming_proof->generate_r1cs_witness();
    for(size_t k=0; k<aggregation_arity; k++)
    {
        verifiers[k].generate_r1cs_witness();
    }


    assert(pb.is_satisfied());
}


template<typename from, typename to>
void aggregator_circuit<from, to>::generate_primary_inputs(
    std::vector<r1cs_ppzksnark_verification_key<from>> verification_key_value,
    std::vector<r1cs_ppzksnark_primary_input<from>> verifier_input_values,
    std::vector<r1cs_ppzksnark_proof<from>> proof_value
){

    size_t elt_size = from_field::size_in_bits();

    /**
     * @brief Assign the verifier input bits
     * 
     */
    libff::bit_vector input_as_bits_values;
    libff::bit_vector v;
    for(size_t k=0; k<aggregation_arity; k++)
    {
        for(const from_field &val: verifier_input_values[k])
        {
            v = libff::convert_field_element_to_bit_vector<from_field>(val, elt_size);
            input_as_bits_values.insert(input_as_bits_values.end(), v.begin(), v.end());
        }
        verifier_inputs_bits[k].fill_with_bits(pb, input_as_bits_values);
        assert(verifier_inputs_bits[k].size() == input_as_bits_values.size());

        v.clear();
        input_as_bits_values.clear();
    }

    /**
     * @brief Assign the proof and verification keys
     * 
     */
    for(size_t k=0; k<aggregation_arity; k++)
    {
        proofs[k].generate_r1cs_witness(proof_value[k]);
        verification_keys[k].generate_r1cs_witness(verification_key_value[k]);
    }

    /**
     * @brief Maps proof content with proof, and proof_bits with proof content
     * 
     */
    uint counter;
    for(size_t k=0; k<aggregation_arity; k++)
    {
        counter = 0;
        for(size_t i=0; i<proofs[0].all_G1_vars.size(); i++){
            for(size_t j=0; j<proofs[0].all_G1_vars[0]->all_vars.size(); j++){
                pb.val(proofs_contents[k][counter]) = pb.lc_val(proofs[k].all_G1_vars[i]->all_vars[j]);
                counter++;  
            }
        }
        for(size_t i=0; i<proofs[0].all_G2_vars.size(); i++){
            for(size_t j=0; j<proofs[0].all_G2_vars[0]->all_vars.size(); j++){
                pb.val(proofs_contents[k][counter]) = pb.lc_val(proofs[k].all_G2_vars[i]->all_vars[j]);
                counter++;  
            }
        }
        unpack_proofs[k].generate_r1cs_witness_from_packed();
    }


    /**
     * @brief Generate witnesses for the CRH
     * 
     */
    hash_incoming_proof->generate_r1cs_witness();
    // for(size_t k=0; k<aggregation_arity; k++)
    // {
    //     verifiers[k].generate_r1cs_witness();
    // }


    assert(pb.is_satisfied());
}

#endif