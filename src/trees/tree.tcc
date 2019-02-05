# ifndef TREE_TCC_2
# define TREE_TCC_2

# include <stdlib.h>
# include <sstream>
# include <trees/tree.hpp>
# include <libff/common/serialization.hpp>
# include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>

using namespace std;

template<typename aggregation_system, size_t N>
std::ostream& operator<<(std::ostream& out, const tree<aggregation_system, N> &obj)
{
    using TreeT = tree<aggregation_system, N>;

    out << obj.node_values.leaf << OUTPUT_NEWLINE;
    out << obj.node_values.proof << OUTPUT_NEWLINE;

    if (obj.node_values.leaf) 
    {
        out << obj.node_values.primary_input[0] << OUTPUT_NEWLINE;
        out << obj.node_values.verification_key << OUTPUT_NEWLINE;

    } else {
        for(size_t i=0; i<TreeT::arity; i++)
        {
            out << obj.children[i] << OUTPUT_NEWLINE;
        }
    }

    return out;
}

template<typename aggregation_system, size_t N>
std::istream& operator>>(std::istream& in, tree<aggregation_system, N> &obj)
{
    using TreeT = tree<aggregation_system, N>;
    using toT = typename TreeT::toT;
    using aggregationT = typename TreeT::aggregationT;

    bool leaf;
    in >> leaf; libff::consume_OUTPUT_NEWLINE(in);

    r1cs_ppzksnark_proof<toT> _proof;
    in >> _proof; libff::consume_OUTPUT_NEWLINE(in);

    if (leaf)
    {
        r1cs_ppzksnark_primary_input<toT> _input(1);
        in >> _input[0]; libff::consume_OUTPUT_NEWLINE(in);

        r1cs_ppzksnark_verification_key<toT> _vk;
        in >> _vk; libff::consume_OUTPUT_NEWLINE(in);

        auto _node = node<aggregationT>::from_leaf(_input, _vk, _proof);
        obj.node_values = _node;

    } else {
        obj.children.resize(TreeT::arity);
        for(size_t k=0; k<TreeT::arity; k++)
        {
            in >> obj.children[k]; libff::consume_OUTPUT_NEWLINE(in);
        }

        auto _node = node<aggregationT>::from_aggregation(_proof);
        obj.node_values = _node;
    }

    return in;

}

template<typename aggregation_system, size_t N>
tree<aggregation_system, N> tree<aggregation_system, N>::from_string(unsigned char * buff)
{

    uint32_t buff_size;
    string buff_str;
    size_t offset = sizeof(uint32_t);
    stringstream ss;
    tree<aggregation_system, N> res;

    /**
     * @brief Cast the buffer in a string, then a stringstream
     * 
     */
    memcpy(&buff_size, buff, offset);
    buff_str.assign(buff, buff + buff_size - 1);

    ss << buff_str;

    cout << "-------" << endl << buff_str << endl << "-------" << endl;

    ss.rdbuf()->pubseekpos(0, std::ios_base::in);
    ss.seekg(4*offset);

    /**
     * @brief Deserialize the object
     * 
     */
    ss >> res;

    return res;
}

template<typename aggregation_system, size_t N>
unsigned char * tree<aggregation_system, N>::to_string()
{
    using TreeT = tree<aggregation_system, N>;
    const size_t offset = sizeof(uint32_t);
    const size_t arity = TreeT::aggregationT::arity;
    const int zero = 0;
    stringstream ss;

    /**
     * @brief Various headers metadata
     *  
     * 1) The total length of the buffer is stored in the first bytes
     * 2) Dummy identifier for the aggregation system
     * 3) Identify the aggregation steps so that the deserializer can use the proper class
     * 4) Identify the arity of the aggregation
     */
    unsigned char header[offset*4];
    memset(header, '\0', offset*4);
    memset(&header[offset], (uint32_t)0, 1);
    memset(&header[offset*2], (uint32_t)N, 1);
    memset(&header[offset*3], (uint32_t)arity, 1);

    /**
     * @brief Stream the content of the tree
     * 
     */
    ss << header << *this;

    /** 
     * Writes the length of the buffer in a char[4]
     */
    uint32_t len = ss.tellp();

    /**
     * @brief Copy the stream contents in the output buffer
     * 
     */
    auto buff = new unsigned char[len+1];
    std::string buff_str = ss.str();
    memset(buff, '\0', len+1);
    memcpy(buff, buff_str.c_str(), len);
    memcpy(buff, &len, offset);

    return buff;
}

template<typename aggregation_system, size_t N>
tree<aggregation_system, N>& tree<aggregation_system, N>::aggregate_inputs()
{

    using TreeT = tree<aggregation_system, N>;
    using fromT = typename TreeT::fromT;
    using toT = typename TreeT::toT;

    auto aggregation = TreeT::aggregationT::get_instance();

    for (size_t i=0; i<arity; i++)
    {
        if (!children[i].node_values.leaf)
        {
            children[i].aggregate_inputs();
        }
    }

    vector<r1cs_ppzksnark_primary_input<fromT>> primary_inputs(arity);
    vector<r1cs_ppzksnark_verification_key<fromT>> verification_keys(arity);
    vector<r1cs_ppzksnark_proof<fromT>> proofs(arity);

    for (size_t i=0; i<arity; i++)
    {
        primary_inputs[i] = children[i].node_values.primary_input;
        verification_keys[i] = children[i].node_values.verification_key;
        proofs[i] = children[i].node_values.proof;
    }

    node_values.set_inputs(
        aggregation.aggregate_inputs(
            primary_inputs,
            verification_keys,
            proofs
        )
    );

    return *this;
}

template<typename aggregation_system, size_t N>
tree<aggregation_system, N>& tree<aggregation_system, N>::aggregate_proofs()
{

    using TreeT = tree<aggregation_system, N>;
    using fromT = typename TreeT::fromT;
    using toT = typename TreeT::toT;; 

    vector<r1cs_ppzksnark_primary_input<fromT>> primary_inputs(arity);
    vector<r1cs_ppzksnark_verification_key<fromT>> verification_keys(arity);
    vector<r1cs_ppzksnark_proof<fromT>> proofs(arity);

    for (size_t i=0; i<arity; i++)
    {
        primary_inputs[i] = children[i].node_values.primary_input;
        verification_keys[i] = children[i].node_values.verification_key;
        proofs[i] = children[i].node_values.proof;
    }

    auto aggregation = TreeT::aggregationT::get_instance();

    node_values.set_proof(
        aggregation.aggregate_proofs(
            primary_inputs,
            verification_keys,
            proofs
        )
    );

    return *this;
}

# endif