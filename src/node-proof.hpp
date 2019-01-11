# ifndef NODE_PROOF_HPP
# define NODE_PROOF_HPP

# include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
# include "serialize.hpp"
# include "circuits/aggregator_circuit.hpp"
# include "tools.hpp"
# include "libsnark/relations/constraint_satisfaction_problems/r1cs/examples/r1cs_examples.hpp"

using namespace libsnark;

template<typename curve, typename other_curve>
class node_proof;

template<typename curve, typename other_curve>
std::ostream& operator<<(std::ostream &out, const node_proof<curve, other_curve> &pk);

template<typename curve, typename other_curve>
std::istream& operator>>(std::istream &in, node_proof<curve, other_curve> &pk);

template<typename curve, typename other_curve>
class node_proof {
public:
 
    using FieldT = libff::Fr<curve>;

    bool leaf;
    r1cs_ppzksnark_primary_input<curve> primary_input;
    r1cs_ppzksnark_verification_key<curve> verification_key;
    r1cs_ppzksnark_proof<curve> proof;

    node_proof() = default;

    node_proof(size_t num_constraints);

    // Constructor for leaf nodes
    node_proof(
        r1cs_ppzksnark_primary_input<curve> primary_input,
        r1cs_ppzksnark_verification_key<curve> verification_key,
        r1cs_ppzksnark_proof<curve> proof
    );

    // Constructor for aggregation node
    node_proof(
        r1cs_ppzksnark_primary_input<curve> primary_input,
        r1cs_ppzksnark_proof<curve> proof        
    );

    // Verifier
    bool verify();

    // Aggregator constructor
    static node_proof<curve, other_curve> from_aggregation(
        const std::vector<node_proof<other_curve, curve>> childrens
    );

    // String serializer
    void to_string(std::string &buff);
    void to_string(unsigned char* &buff);
    void from_string(const std::string &buff);
    void from_string(unsigned char* &buff);

};

# include "node-proof.tcc"

# endif