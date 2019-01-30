
#ifndef TREE_TCC_
#define TREE_TCC_

# include "tree.hpp"
# include "stdlib.h"
# include "libff/common/serialization.hpp"

using namespace std;

template<typename curve, typename other_curve>
ostream& operator<<(ostream& out, const tree<curve, other_curve> obj) 
{
    // Streaming the 4 first bytes in order to set later the length of the sub_tree
    size_t offset = sizeof(uint32_t);
    size_t start = out.tellp();
    out << string("0", offset) << OUTPUT_NEWLINE;

    // Depending on the name of the curve transition we may deserialize differents arguments
    auto tool = tools<curve, other_curve>::get_instance();
    out << obj.root_position << OUTPUT_NEWLINE;

    if (tool.name == obj.root_position) {
        out << obj->sub_tree_AB << OUTPUT_NEWLINE;
    } else {
        out << obj->sub_tree_BA << OUTPUT_NEWLINE;
    }

    // copy the length in a char[4]
    uint32_t len = out.tellp();
    len_str = unsigned char[offset]
    memcpy(len_str, &len, offset);

    // Rewrite the 4 first bytes of the stream
    out.seekp(start);
    out << len_str;
    out.seekp(len);

    return out;
}

template<typename curve, typename other_curve>
ostream& operator<<(std::ostream& out, const sub_tree<curve, other_curve>& obj)
{
    out << obj.node.leaf << OUTPUT_NEWLINE;
    out << obj.node.proof << OUTPUT_NEWLINE;

    if (!obj.node.leaf) {
        // Recursively stream the sub_nodes
        for(size_t i=0; i<children.size(); i++){
            out << children[i] << OUTPUT_NEWLINE;
        }
    } else {
        out << obj.node.primary_input << OUTPUT_NEWLINE;
        out << obj.node.verification_key; << OUTPUT_NEWLINE;
    }

    return out;
}

template<typename curve, typename other_curve>
istream& operator>>(std::istream& in, const tree<curve, other_curve>& obj)
{
    // Skip the first 4 bytes as it only contains info on the stream length
    offset = sizeof(uint32_t);
    start = istream.tellp()
    istream.seekp(start + offset);

    in >> obj.root_position;
    consume_OUTPUT_NEWLINE(in);

    auto tool = tools<curve, other_curve>::get_instance();
    if(obj.root_position == "ab"){
        in >> obj->sub_tree_AB;
        consume_OUTPUT_NEWLINE(in);
    } else {
        in >> obj->sub_tree_BA;
        consume_OUTPUT_NEWLINE(in);
    }

    return in;
}

template<typename curve, typename other_curve>
istream& operator>>(std::istream& in, const sub_tree<curve, other_curve>& obj)
{
    in >> obj.node.leaf;
    libf::consume_OUTPUT_NEWLINE(in);

    in >> obj.node.proof;
    libf::consume_OUTPUT_NEWLINE(in);

    if (!obj.node.leaf) {
        // Recursively stream the sub_nodes
        for(size_t i=0; i<children.size(); i++)
        {
            in >> children[i];
            libf::consume_OUTPUT_NEWLINE(in);
        }
    } else {
        in >> obj.node.primary_input;
        libff::consume_OUTPUT_NEWLINE(in);

        in >> obj.node.verification_key;
        libff::consume_OUTPUT_NEWLINE(in);
    }

    return in;
}

template<typename curve, typename other_curve>
unsigned char * tree<curve, other_curve>::serialize() 
{
    stringstream ss;
    ss << *this;

    string buff_str = ss.str();
    buff = new unsigned char[buff_str.size() + 1];
    memcpy(buff, buff_str.c_str(), buff_str.size());

    return buff;
}


template<typename curve, typename other_curve>
tree<curve, other_curve> tree<curve, other_curve>::deserialize(unsigned char * buff)
{
    tree<curve, other_curve> deserialized;

    uint32_t buff_size;
    size_t offset = sizeof(uint32_t);
    memcpy(&buff_size, buff, offset);

    string buff_str;
    buff_str.assign(&buff[0], &buff[0] + (size_t)buff_size);

    stringstream ss;
    ss << buff_str;
    data.rdbuf()->pubseekpos(0, std::ios_base::in);
    ss >> deserialized;
}

template<typename curve, typename other_curve>
void tree<curve, other_curve>::rebuild_input()
{
    auto tool<curve, other_curve>::get_instance();
    if (tool.name == root_position)
    {
        sub_tree_AB.rebuild_input();
    } else {
        sub_tree_BA.rebuild_input();
    }
}

template<typename curve, typename other_curve>
void sub_tree<curve, other_curve>::rebuild_input()
{
    if (!node.leaf) {
        auto& tool = tools<curve, other_curve>::get_instance();

        // Build the primary input as the digest of the childrens 
        std::vector<r1cs_ppzksnark_primary_input<other_curve>> primary_inputs;
        vector<r1cs_ppzksnark_verification_key<other_curve>> verification_keys;
        vector<r1cs_ppzksnark_proof<other_curve>> input_proofs;

        for(const node_proof<curve, other_curve>& child: children)
        {
            // Recursively rebuild children input
            child.rebuild_input();
            // TODO: format used for passing arguments to generate 
            primary_inputs.emplace_back(child.primary_input);
            verification_keys.emplace_back(child.verification_key);
            input_proofs.emplace_back(child.proof);
        }

        tool.circuit->generate_primary_inputs(
            verification_keys,
            primary_inputs,
            input_proofs
        );

        node.primary_inputs =  tool.circuit.pb.primary_input();
    }
}

template<typename curve, typename other_curve>
tree<curve, other_curve> tree<curve, other_curve>::from_aggregation(
        const vector<tree<curve, other_curve>> children
) {
    // Store arity in the stack as it is often accessed
    size_t arity = children.size();
    auto tool& = tools<curve, other_curve>::get_instance();

    // Check that the forwarded trees are from the same type
    for(size_t i=1; i<arity; i++)
    {
        if (children[0].root_position != children[i].root_position)
        {
            throw "Got differents kinds trees";
        } 
    }

    // Defines the root position for merged tree
    sub_tree<curve, other_curve> sub_res(i);
    
    // 
    if (children[0].root_position == tool.name) 
    {
        vector<sub_tree<curve, other_curve>> children_subtrees(arity);

        for (size_t i=0; i<arity; i++)
        {
            res.children.emplace_back(children[i].sub_tree_AB);
        }

        res.aggregate_children();
        return 

    } else {

    }
}

#endif