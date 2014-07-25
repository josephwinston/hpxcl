// Copyright (c)		2013 Damond Howard
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

#include <hpx/hpx.hpp>
#include <hpx/runtime/components/component_factory.hpp>
#include <hpx/util/portable_binary_iarchive.hpp>
#include <hpx/util/portable_binary_oarchive.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/export.hpp>

#include "server/buffer.hpp"

HPX_REGISTER_COMPONENT_MODULE();

typedef hpx::components::managed_component<
    hpx::cuda::server::buffer>
    cuda_buffer_type;

HPX_REGISTER_MINIMAL_COMPONENT_FACTORY(cuda_buffer_type,buffer);

HPX_REGISTER_ACTION(
    cuda_buffer_type::wrapped_type::size_action,
	cuda_buffer_size_action);
HPX_REGISTER_ACTION(
	cuda_buffer_type::wrapped_type::push_back_action,
	cuda_buffer_push_back_action);
HPX_REGISTER_ACTION(
	cuda_buffer_type::wrapped_type::load_args_action,
	cuda_buffer_load_args_action);