# ifndef TREE_HPP
# define TREE_HPP

# include<stdlib.h>
# include <libsnark/zk_proof_systems/ppzksnark/r1cs_ppzksnark/r1cs_ppzksnark.hpp>
# include <trees/node.hpp>

template<typename aggregation_system, std::size_t N>
class tree;

template<typename aggregation_system, size_t N>
std::ostream& operator<<(std::ostream& out, const tree<aggregation_system, N> &obj);

template<typename aggregation_system, size_t N>
std::istream& operator>>(std::istream& in, tree<aggregation_system, N> &obj);

template<typename aggregation_system, std::size_t N>
class tree {

public:

    using cycleT = typename aggregation_system::cycleT;

    using aggregationT      = typename aggregation_system::template get_aggregation<N>::type;
    using prev_aggregationT = typename aggregation_system::template get_aggregation<(N-1)%(cycleT::size)>::type;
    using sub_tree          = tree<aggregation_system, (N-1)%(cycleT::size)>;
    using toT               = typename aggregationT::toT;
    using fromT             = typename aggregationT::fromT;

    static constexpr std::size_t arity = aggregationT::arity;
    static constexpr std::size_t fromN = N;

    std::vector<tree<aggregation_system, (N-1)%(cycleT::size)>> children;
    node<aggregationT> node_values;

public:

    tree(){};
    tree(std::vector<sub_tree> &_children) : children(_children) {}
    tree(node<aggregationT> &_node_values) : node_values(_node_values) {}

public:

    unsigned char * to_string();
    static tree from_string(unsigned char *);

    static tree<aggregation_system, N> from_aggregation(std::vector<sub_tree> children)
    {
        tree<aggregation_system, N> res(children);
        return res;
    }

    static tree<aggregation_system, N> from_leaf(
        libsnark::r1cs_ppzksnark_primary_input<toT>& _primary_input,
        libsnark::r1cs_ppzksnark_verification_key<toT>& _verification_key,
        libsnark::r1cs_ppzksnark_proof<toT>& _proof
    ) {
        auto _node = node<aggregationT>::from_leaf(
            _primary_input,
            _verification_key,
            _proof
        );

        tree<aggregation_system, N> res(_node);
        return res;
    }

    static tree<aggregation_system, N> as_example(std::size_t num_constraints)
    {
        auto _node = node<aggregationT>::as_example(num_constraints);
        tree<aggregation_system, N> res(_node);
        return res;
    }
        
    tree& aggregate_inputs();
    tree& aggregate_proofs();

    bool verify()
    {
        return node_values.verify();
    }

};

# include <trees/tree.tcc>

# endif