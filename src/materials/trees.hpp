# ifndef TREE_MATERIAL_HPP
# define TREE_MATERIAL_HPP

# include <stdlib.h>
# include <cstdint>
# include <cstring>
# include <vector>

void memfreetree(unsigned char * buff)
{
    delete buff;
}

std::vector<std::uint32_t> parse_header(unsigned char * buff)
{
    std::size_t offset = sizeof(std::uint32_t);
    std::vector<uint32_t> res {0, 0, 0, 0};

    std::memcpy(&res[0], &buff[0], offset);
    std::memcpy(&res[1], &buff[offset], offset);
    std::memcpy(&res[2], &buff[2*offset], offset);
    std::memcpy(&res[3], &buff[3*offset], offset);

    return res;
}

# endif