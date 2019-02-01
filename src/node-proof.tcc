# ifndef NODE_PROOF_TCC
# define NODE_PROOF_TCC

# include "sstream"
# include "node-proof.hpp"
# include "stdlib.h"
# include "libff/common/serialization.hpp"

using namespace libsnark;

template<typename curve, typename other_curve>
std::ostream& operator<<(std::ostream &out, const node_proof<curve, other_curve> &obj)
{
    out << obj.leaf << OUTPUT_NEWLINE;
    out << obj.primary_input[0] << OUTPUT_NEWLINE;
    out << obj.proof << OUTPUT_NEWLINE;
    
    if (obj.leaf) {
        out << obj.verification_key << OUTPUT_NEWLINE;
        
    }

    return out;
}

template<typename curve, typename other_curve>
std::istream& operator>>(std::istream &in, node_proof<curve, other_curve> &obj){
    
    obj.primary_input.resize(1);
    in >> obj.leaf;
    libff::consume_OUTPUT_NEWLINE(in);
    
    in >> obj.primary_input[0];
    libff::consume_OUTPUT_NEWLINE(in);
    
    in >> obj.proof;
    libff::consume_OUTPUT_NEWLINE(in);
    
    if (obj.leaf) { 
        in >> obj.verification_key;
        libff::consume_OUTPUT_NEWLINE(in);
    }

    return in;
}

template<typename curve, typename other_curve>
node_proof<curve,other_curve>::node_proof(size_t num_constraints) :
    leaf(true)
{
    // Running an example prover of size num_constraint
    r1cs_example<FieldT> example = generate_r1cs_example_with_field_input<FieldT>(
        num_constraints, 
        1
    );
    primary_input = example.primary_input;
    r1cs_ppzksnark_keypair<curve> keypair = r1cs_ppzksnark_generator<curve>(example.constraint_system);
    verification_key = keypair.vk;
    proof = r1cs_ppzksnark_prover<curve>(
        keypair.pk, example.primary_input, 
        example.auxiliary_input
    );
}

template<typename curve, typename other_curve>
node_proof<curve,other_curve>::node_proof(
    r1cs_ppzksnark_primary_input<curve> primary_input,
    r1cs_ppzksnark_verification_key<curve> verification_key,
    r1cs_ppzksnark_proof<curve> proof
) :
    primary_input(primary_input), 
    verification_key(verification_key), 
    proof(proof),
    leaf(true)
{}

template<typename curve, typename other_curve>
node_proof<curve,other_curve>::node_proof(
    r1cs_ppzksnark_primary_input<curve> primary_input,
    r1cs_ppzksnark_proof<curve> proof        
) :
    primary_input(primary_input),
    proof(proof),
    leaf(false)
{
    verification_key = tools<curve, other_curve>::get_instance().key_pair->vk;
}

template<typename curve, typename other_curve>
bool node_proof<curve,other_curve>::verify() {
    return r1cs_ppzksnark_verifier_strong_IC<curve>(
        verification_key,
        primary_input,
        proof
    );
}

template<typename curve, typename other_curve>
node_proof<curve, other_curve> node_proof<curve,other_curve>::from_aggregation(
    const std::vector<node_proof<other_curve, curve>> childrens
) {
    // Throws if the inputs node_proof are incorrect
    for(auto child : childrens){
        if (!child.verify()) {
            std::cout << "Failed verification" << std::endl;
            throw "Left proof is fake"; 
        }
    }

    vector<r1cs_ppzksnark_primary_input<other_curve>> primary_inputs;
    vector<r1cs_ppzksnark_verification_key<other_curve>> verification_keys;
    vector<r1cs_ppzksnark_proof<other_curve>> input_proofs;

    for(const node_proof<other_curve, curve>& child : childrens){
        primary_inputs.emplace_back(child.primary_input);
        verification_keys.emplace_back(child.verification_key);
        input_proofs.emplace_back(child.proof);
    }

    tools<curve, other_curve>& tool = tools<curve, other_curve>::get_instance();
    
    tool.circuit->generate_r1cs_witness(
        verification_keys,
        primary_inputs,
        input_proofs
    );

    libsnark::r1cs_ppzksnark_proof<curve> output_proof = r1cs_ppzksnark_prover<curve>(
        tool.key_pair->pk,
        tool.circuit->pb.primary_input(),
        tool.circuit->pb.auxiliary_input()
    );
    
    return node_proof(
        tool.circuit->pb.primary_input(),
        output_proof
    );
}

template<typename curve, typename other_curve>
void node_proof<curve, other_curve>::to_string(std::string &buff){
    std::stringstream data;
    data << *this;
    buff = data.str();

    # ifdef DEBUG
    std::cout << "Serialized node proof of size : " << buff.size() << endl;
    # endif
}

template<typename curve, typename other_curve>
void node_proof<curve, other_curve>::to_string(unsigned char* &buff)
{
    std::string buff_str;
    to_string(buff_str);

    uint32_t buff_size = buff_str.size();
    size_t offset = sizeof(uint32_t);

    buff = new unsigned char[buff_size + offset + 1];
    memcpy(buff, &buff_size, offset);
    memcpy(buff + offset, buff_str.c_str(), buff_size);
}

template<typename curve, typename other_curve>
void node_proof<curve,other_curve>::from_string(
    const std::string &buff
){
    std::stringstream data;
    data << buff;
    data.rdbuf()->pubseekpos(0, std::ios_base::in);
    data >> *this;

    if(!leaf) {
        auto& tool = tools<curve, other_curve>::get_instance();
        verification_key = tool.key_pair->vk;
    }
}

template<typename curve, typename other_curve>
void node_proof<curve,other_curve>::from_string(
    unsigned char* &buff
){
    uint32_t buff_size;
    size_t offset = sizeof(uint32_t);
    memcpy(&buff_size, buff, offset);

    cout << buff[offset + (size_t)buff_size] << endl;
    cout << "buffsize : " << buff_size << endl;

    string buff_str; 
    buff_str.assign(&buff[offset], &buff[offset]+(size_t)buff_size);

    from_string(buff_str);
}

# endif