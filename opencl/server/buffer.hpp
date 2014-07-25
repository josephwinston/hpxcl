// Copyright (c)    2013 Martin Stumpf
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#pragma once
#ifndef HPX_OPENCL_SERVER_BUFFER_HPP
#define HPX_OPENCL_SERVER_BUFFER_HPP


#include <hpx/hpx.hpp>
#include <hpx/config.hpp>
#include <hpx/include/components.hpp>
#include <hpx/util/serialize_buffer.hpp>

#include <CL/cl.h>

#include <boost/shared_ptr.hpp>
#include <vector>

#include "../fwd_declarations.hpp"
#include "../event.hpp"

namespace hpx { namespace opencl{ namespace server{

    // /////////////////////////////////////////////////////
    //  This class represents an opencl buffer.

    class buffer
      : public hpx::components::managed_component_base<buffer>
    {
    public:

        // Constructor
        buffer();
        buffer(hpx::naming::id_type device_id, cl_mem_flags flags, size_t size);
        buffer(hpx::naming::id_type device_id, cl_mem_flags flags, size_t size,
               hpx::util::serialize_buffer<char> buffer);
        ~buffer();

        ///////////////////////////////////////////////////
        /// Local functions
        /// 
        cl_mem get_cl_mem();

        ///////////////////////////////////////////////////
        /// Exposed functionality of this component
        ///
        size_t size();
        hpx::opencl::event read(size_t offset, size_t size,
                                      std::vector<hpx::opencl::event> events);
        hpx::opencl::event write(size_t offset, 
                                 hpx::util::serialize_buffer<char> data,
                                 std::vector<hpx::opencl::event> events);
#ifdef CL_VERSION_1_2
        hpx::opencl::event fill(hpx::util::serialize_buffer<char> pattern,
                                size_t offset, size_t size,
                                std::vector<hpx::opencl::event> events);
#endif
        hpx::opencl::event copy(hpx::naming::id_type src_buffer, 
                                std::vector<size_t> dimensions,
                                std::vector<hpx::opencl::event> events);

    //[
    HPX_DEFINE_COMPONENT_ACTION(buffer, size);
    HPX_DEFINE_COMPONENT_ACTION(buffer, read);
    HPX_DEFINE_COMPONENT_ACTION(buffer, write);
    HPX_DEFINE_COMPONENT_ACTION(buffer, copy);
#ifdef CL_VERSION_1_2
    HPX_DEFINE_COMPONENT_ACTION(buffer, fill);
#endif
    //]
    private:
        //////////////////////////////////////////////////
        /// Private Member Functions
        ///

        // Bruteforce copy, needed for copy between different machines
        cl_event copy_bruteforce(hpx::naming::id_type & src_buffer,
                                 const size_t & src_offset,
                                 const size_t & dst_offset,
                                 const size_t & size,
                                 std::vector<hpx::opencl::event> & events);

        // Local copy, buffers are on the same machine but in different contexts
        cl_event copy_local(boost::shared_ptr<hpx::opencl::server::buffer>,
                            const size_t & src_offset,
                            const size_t & dst_offset,
                            const size_t & size,
                            std::vector<hpx::opencl::event> & events);

        // Direct copy, buffers are on the same context
        cl_event copy_direct(boost::shared_ptr<hpx::opencl::server::buffer>,
                             const size_t & src_offset,
                             const size_t & dst_offset,
                             const size_t & size,
                             std::vector<hpx::opencl::event> & events);



    private:
        //////////////////////////////////////////////////
        //  Private Member Variables
        boost::shared_ptr<device> parent_device;
        cl_mem device_mem;
        hpx::naming::id_type parent_device_id;

    };



}}}

//[
HPX_REGISTER_ACTION_DECLARATION(
        hpx::opencl::server::buffer::size_action,
        opencl_buffer_size_action);
HPX_REGISTER_ACTION_DECLARATION(
        hpx::opencl::server::buffer::read_action,
        opencl_buffer_read_action);
HPX_REGISTER_ACTION_DECLARATION(
        hpx::opencl::server::buffer::write_action,
        opencl_buffer_write_action);
HPX_REGISTER_ACTION_DECLARATION(
        hpx::opencl::server::buffer::copy_action,
        opencl_buffer_copy_action);
#ifdef CL_VERSION_1_2
HPX_REGISTER_ACTION_DECLARATION(
        hpx::opencl::server::buffer::fill_action,
        opencl_buffer_fill_action);
#endif
//]




























#endif//HPX_OPENCL_SERVER_BUFFER_HPP

