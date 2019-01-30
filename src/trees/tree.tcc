# ifndef TREE_TCC_2
# define TREE_TCC_2

# include <stdlib.h>
# include <sstream>
# include <trees/tree.hpp>
# include <libff/common/serialization.hpp>
# include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>

using namespace std;

template<typename aggregation_system, size_t N>
std::ostream& operator<<(std::ostream& out, tree<aggregation_system, N> obj)
{
    using TreeT = tree<aggregation_system, N>;

    out << obj.node_values.leaf << OUTPUT_NEWLINE;
    out << *obj.node_values.proof << OUTPUT_NEWLINE;

    if (obj.node_values.leaf) 
    {
        out << *obj.node_values.primary_input << OUTPUT_NEWLINE;
        out << *obj.node_values.verification_key << OUTPUT_NEWLINE;

    } else {
        for(size_t i=0; i<TreeT::arity; i++)
        {
            out << obj.children[i] << OUTPUT_NEWLINE;
        }
    }

    return out;
}

template<typename aggregation_system, size_t N>
std::istream& operator>>(std::istream& in, tree<aggregation_system, N> obj)
{
    using TreeT = tree<aggregation_system, N>;
    using toT = typename TreeT::toT;

    in >> obj.node_values.leaf; libff::consume_OUTPUT_NEWLINE(in);

    libsnark::r1cs_ppzksnark_proof<toT> _proof;
    in >> _proof; libff::consume_OUTPUT_NEWLINE(in);
    obj.node_values.primary_input = make_shared(_proof);

    if (obj.node_values.leaf)
    {
        r1cs_ppzksnark_primary_input<toT> _input;
        in >> _input; libff::consume_OUTPUT_NEWLINE(in);
        obj.node_values.primary_input = make_shared(_input);

        r1cs_ppzksnark_verification_key<toT> _vk;
        in >> _vk; libff::consume_OUTPUT_NEWLINE(in);
        obj.node_values.verification_key = make_shared(_vk);

    } else {
        obj.children.resize(TreeT::arity);
        for(size_t i=0; i<TreeT::arity; i++)
        {
            in >> obj.children[i]; libff::consume_OUTPUT_NEWLINE(in);
        }
    }

    return in;

}

template<typename aggregation_system, size_t N>
tree<aggregation_system, N> tree<aggregation_system, N>::from_string(unsigned char * buff)
{

    uint32_t buff_size;
    size_t offset = sizeof(uint32_t);
    stringstream ss;
    tree<aggregation_system, N> res;

    /**
     * @brief Cast the buffer in a string, then a stringstream
     * 
     */
    memcpy(&buff_size, buff, offset);
    string buff_str;
    buff_str.assign(buff, &buff[buff_size-1]);
    ss << buff_str;

    ss.rdbuf()->pubseekpos(0, std::ios_base::in);

    /**
     * @brief Skip the headers
     * 
     */
    size_t garbage;
    ss >> garbage; libff::consume_OUTPUT_NEWLINE(ss);
    ss >> garbage; libff::consume_OUTPUT_NEWLINE(ss);
    ss >> garbage; libff::consume_OUTPUT_NEWLINE(ss);
    ss >> garbage; libff::consume_OUTPUT_NEWLINE(ss);

    /**
     * @brief Deserialize the object
     * 
     */
    ss >> res;

}

template<typename aggregation_system, size_t N>
unsigned char * tree<aggregation_system, N>::to_string()
{
    using TreeT = tree<aggregation_system, N>;
    const size_t offset = sizeof(uint32_t);
    stringstream ss;

    /**
     * @brief Various headers metadata
     *  
     * 1) The total length of the buffer is stored in the first bytes
     * 2) Dummy identifier for the aggregation system
     * 3) Identify the aggregation steps so that the deserializer can use the proper class
     * 4) Identify the arity of the aggregation
     */
    ss << string("0", offset) << OUTPUT_NEWLINE;
    ss << (uint64_t)0 << OUTPUT_NEWLINE;
    ss << (uint32_t)N << OUTPUT_NEWLINE;
    ss << (uint32_t)TreeT::aggregationT::arity << OUTPUT_NEWLINE;

    /**
     * @brief Stream the content of the tree
     * 
     */
    ss << *this;

    /** 
     * Writes the length of the buffer in a char[4]
     */
    uint32_t len = ss.tellp();
    unsigned char len_str[offset];
    memcpy(len_str, &len, offset);

    /**
     * @brief : Writes the length of the buffer in the stream
     * 
     */
    ss.seekp(0);
    ss << len_str;
    ss.seekp(len);

    /**
     * @brief Copy the stream contents in the output buffer
     * 
     */
    string buff_str = ss.str();
    auto buff = new unsigned char[buff_str.size() + 1];
    memcpy(buff, buff_str.c_str(), buff_str.size());

    return buff;
}

template<typename aggregation_system, size_t N>
tree<aggregation_system, N>& tree<aggregation_system, N>::aggregate_inputs()
{

#if NDEBUG
    std::cout << "tree<aggregation_system, N>& tree<aggregation_system, N>::aggregate_inputs" << std::endl;
# endif

    using TreeT = tree<aggregation_system, N>;
    using fromT = typename TreeT::fromT;
    using toT = typename TreeT::toT;

    auto aggregation = TreeT::aggregationT::get_instance();

#if NDEBUG
    if(node_values.leaf)
    {
        std::cout <<  "Called aggregate_input on a leaf node tree" << std::endl;
        return *this;
    }
#endif

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
        primary_inputs[i] = *children[i].node_values.primary_input;
        verification_keys[i] = *children[i].node_values.verification_key;
        proofs[i] = *children[i].node_values.proof;
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

#if NDEBUG
    std::cout << "tree<aggregation_system, N>& tree<aggregation_system, N>::aggregate_proofs" << std::endl;
# endif

    using TreeT = tree<aggregation_system, N>;
    using fromT = typename TreeT::fromT;
    using toT = typename TreeT::toT;; 

    vector<r1cs_ppzksnark_primary_input<fromT>> primary_inputs(arity);
    vector<r1cs_ppzksnark_verification_key<fromT>> verification_keys(arity);
    vector<r1cs_ppzksnark_proof<fromT>> proofs(arity);

    for (size_t i=0; i<arity; i++)
    {
        primary_inputs[i] = *children[i].node_values.primary_input.get();
        verification_keys[i] = *children[i].node_values.verification_key;
        proofs[i] = *children[i].node_values.proof;
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