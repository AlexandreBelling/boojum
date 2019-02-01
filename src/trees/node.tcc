# ifndef NODE_TCC
# define NODE_TCC

# include <trees/node.hpp>
# include <libff/common/serialization.hpp>
# include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>

using namespace libsnark;
using namespace std;

template<typename aggregation>
node<aggregation> node<aggregation>::as_example(std::size_t num_constraints)
{
    // Running an example prover of size num_constraint
    r1cs_example<field_toT> example = generate_r1cs_example_with_field_input<field_toT>(
        num_constraints, 
        1
    );

    auto _keypair = r1cs_ppzksnark_generator<toT>(example.constraint_system);
    auto _proof = r1cs_ppzksnark_prover<toT>(
        _keypair.pk,
        example.primary_input,
        example.auxiliary_input
    );

    return node(
        example.primary_input,
        _keypair.vk,
        _proof
    );

}

template<typename aggregation>
bool node<aggregation>::verify()
{

    return r1cs_ppzksnark_verifier_strong_IC<toT>(
        *verification_key,
        *primary_input,
        *proof
    );
}


# endif