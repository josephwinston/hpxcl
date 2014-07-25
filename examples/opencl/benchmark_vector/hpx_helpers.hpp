// Copyright (c)       2014 Martin Stumpf
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BENCHMARK_HPX_HELPERS_HPP_
#define BENCHMARK_HPX_HELPERS_HPP_

#include "../../../opencl.hpp"

static hpx::naming::id_type hpx_get_remote_node()
{

    // Get all HPX localities
    std::vector<hpx::naming::id_type> localities =
                    hpx::find_all_localities();

    // Get local HPX id
    hpx::naming::id_type local_id = hpx::find_here();

    // Find the node that is NOT the local id
    BOOST_FOREACH(hpx::naming::id_type const& node_id, localities)
    {
        if(node_id != local_id)
            return node_id;
    }

    hpx::cout << "ERROR: No remote node found!" << hpx::endl;

    return hpx::naming::id_type();
}










#endif //BENCHMARK_HPX_HELPERS_HPP_

