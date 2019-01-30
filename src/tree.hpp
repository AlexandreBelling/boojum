# ifndef OLD_TREE_HPP
# define OLD_TREE_HPP

# include "node-proof.hpp"
# include "stdlib.h"

template<typename curve, typename other_curve>
class sub_tree;

template<typename curve, typename other_curve>
class sub_tree {
public:

    using field = libff::Fr<curve>;
    using other_field = libff::Fr<other_curve>;

    vector<sub_tree<other_curve, curve>> children;
    node_proof<curve, other_curve> node;
    string serialized_header;

    sub_tree(i);
    void rebuild_input();

    void to_string(std::string& str);
    void to_string(char *& str);
    
    void from_string(std::string& str);
    void from_string(const char*& str);

    void rebuild_inputs();
    void from_aggregation(
        vector<tree<curve, other_curve>> childrens
    );
    void from_aggregation(
        vector<tree<other_curve, curve>> childrens
    );

    tree<curve, other_curve> grow_tree();

    sub_tree(vector)

    friend &ostream operator<<(std::ostream& out, const sub_tree<curve, other_curve>& obj);
    friend &istream operator>>(std::istream& in, sub_tree<curve, other_curve>& obj);

};

/*
    In practice tree should be define as tree<A, B> whatsoever the actual root type
    may be because there are no way to know which kind of tree is going to be deserialized
*/
template<typename curve, typename other_curve>
class tree;

template<typename curve, typename other_curve>
class tree {
public:

    using field = libff::Fr<curve>;
    using other_field = libff::Fr<other_curve>;

    string root_position;
    std::shared_ptr<tree<curve, other_curve>> sub_tree_AB;
    std::shared_ptr<tree<other_curve, curve>> sub_tree_BA;
    
    unsigned char * serialize();
    static tree<curve, other_curve> deserialize(unsigned char *);
    static tree<curve, other_curve> from_sub_tree(sub_tree<curve, other_curve>);
    statuc tree<curve, other_curve> from_sub_tree(sub_tree<curve, other_curve>);

    void rebuild_input();
    bool valid_tree();

    static tree<curve, other_curve> from_aggregation(
        const vector<tree<curve, other_curve>> children
    );

    friend &ostream operator<<(std::ostream& out, const tree<curve, other_curve>& obj);
    friend &istream operator>>(std::istream& in, tree<curve, other_curve>& obj);

};

# include "tree.tcc"

# endif