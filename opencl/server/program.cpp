// Copyright (c)    2013 Martin Stumpf
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "program.hpp"
#include "../tools.hpp"
#include "device.hpp"
#include "hpx_cl_interop.hpp"

#include <string>
#include <sstream>

#include <CL/cl.h>


using namespace hpx::opencl::server;

CL_FORBID_EMPTY_CONSTRUCTOR(program);


program::program(hpx::naming::id_type device_id, std::string code)
{
    this->program_id = NULL;
    this->parent_device_id = device_id;
    this->parent_device = hpx::get_ptr
                         <hpx::opencl::server::device>(parent_device_id).get();

    // create variables for clCreateProgram call
    size_t code_size = code.length();
    const char* code_ptr = code.c_str();

    // initialize the cl_program object
    cl_int err;
    program_id = clCreateProgramWithSource(parent_device->get_context(), 1,
                                            &code_ptr, &code_size, &err);
    cl_ensure(err, "clCreateProgramWithSource()");
                              
}

program::program(hpx::naming::id_type device_id,
                 hpx::util::serialize_buffer<char> binary)
{
    this->program_id = NULL;
    this->parent_device_id = device_id;
    this->parent_device = hpx::get_ptr
                         <hpx::opencl::server::device>(parent_device_id).get();


    // create variables for clCreateProgram call
    cl_device_id cl_device_id = parent_device->get_device_id();
    const unsigned char* binary_ptr = (unsigned char*)(binary.data());
    size_t binary_size = binary.size(); 

    // initialize the cl_program object
    cl_int err;
    cl_int binary_status;
    program_id = clCreateProgramWithBinary(parent_device->get_context(), 1,
                                           &cl_device_id, &binary_size,
                                           &binary_ptr, &binary_status,
                                           &err);
    cl_ensure(err, "clCreateProgramWithBinary()");
    cl_ensure(binary_status, "clCreateProgramWithBinary()");
                              
}


program::~program()
{
    cl_int err;

    // release the cl_program object
    if(program_id)
    {
        err = clReleaseProgram(program_id);
        cl_ensure_nothrow(err, "clReleaseProgram()");
        program_id = NULL;
    }

}

std::string
program::acquire_build_log()
{
    cl_int err;

    size_t build_log_size;

    // Query size
    err = clGetProgramBuildInfo(program_id, parent_device->get_device_id(),
                                CL_PROGRAM_BUILD_LOG, 0, NULL, &build_log_size);
   
    // Create buffer
    boost::scoped_array<char> buf(new char[build_log_size]);

    // Get log
    err = clGetProgramBuildInfo(program_id, parent_device->get_device_id(),
                                CL_PROGRAM_BUILD_LOG, build_log_size,
                                buf.get(), NULL);

    // make build log look nice in exception
    std::stringstream sstream;
    sstream << std::endl << std::endl;
    sstream << "//////////////////////////////////////" << std::endl;
    sstream << "/// OPENCL BUILD LOG" << std::endl;
    sstream << "///" << std::endl;
    sstream << std::endl << buf.get() << std::endl;
    sstream << "///" << std::endl;
    sstream << "/// OPENCL BUILD LOG END" << std::endl;
    sstream << "//////////////////////////////////////" << std::endl;
    sstream << std::endl;
    
    // return the nice looking error string.
    return sstream.str();

}

static void CL_CALLBACK build_callback(cl_program program, void* args_)
{

    // Read input args
    intptr_t* args = (intptr_t*) args_;
    hpx::runtime* rt = (hpx::runtime*) args[0];
    hpx::lcos::local::event* event = (hpx::lcos::local::event*) args[1];
    
    // trigger the event
    hpx::opencl::server::trigger_event_from_external(rt, event);

}
    
void
program::throw_on_build_errors(cl_device_id device_id, const char* function_name)
{

    cl_int err;
    cl_build_status build_status;

    // Read build status
    err = clGetProgramBuildInfo(program_id, device_id, CL_PROGRAM_BUILD_STATUS,
                                sizeof(build_status), &build_status, NULL);
    cl_ensure(err, "clGetProgramBuildInfo()");

    // Throw if build did not succeed
    if(build_status != CL_BUILD_SUCCESS)
    {
        // throw beautiful build log exception.
        HPX_THROW_EXCEPTION(hpx::no_success, function_name,
                            (std::string("A build error occured!")
                            + acquire_build_log()).c_str());
    }

}

void
program::build(std::string options)
{
    
    cl_int err;

    // fetch device id from parent device
    cl_device_id device_id = parent_device->get_device_id();

    // initialize event lock for cl synchronization
    hpx::lcos::local::event event_lock;
    
    // set up arguments list for callback
    intptr_t args[2];
    args[0] = (intptr_t)hpx::get_runtime_ptr();
    args[1] = (intptr_t)&(event_lock);

    // build the program
    err = clBuildProgram(program_id, 1, &device_id, options.c_str(),
                         &build_callback, (void*) args);

    // ignore CL_BUILD_PROGRAM_FAILURE.
    // we handle this case in throw_on_build_errors()
    if(err != CL_BUILD_PROGRAM_FAILURE)
        cl_ensure(err, "clBuildProgram()");

    // wait for build to finish
    event_lock.wait();
   
    // check for build errors
    throw_on_build_errors(device_id, "clBuildProgram()");
        
}

std::vector<char>
program::get_binary()
{

    cl_int err;

    // get number of devices
    cl_uint num_devices;
    err = clGetProgramInfo(program_id, CL_PROGRAM_NUM_DEVICES, sizeof(cl_uint),
                           &num_devices, NULL);
    cl_ensure(err, "clGetProgramInfo()");
    
    // ensure that only one device is associated
    if(num_devices != 1)
    {
        HPX_THROW_EXCEPTION(hpx::internal_server_error, "program::get_binary()",
                            "Internal Error: More than one device linked!");
    }

    // get binary size
    size_t binary_size;
    err = clGetProgramInfo(program_id, CL_PROGRAM_BINARY_SIZES, sizeof(size_t),
                           &binary_size, NULL);
    cl_ensure(err, "clGetProgramInfo()");

    // ensure that there actually is binary code
    if(binary_size == 0)
    {
        HPX_THROW_EXCEPTION(hpx::no_success, "program::get_binary()",
                            "Unable to fetch binary code!");
    }

    // get binary code
    std::vector<char> binary(binary_size);
    char* binary_ptr = binary.data();
    err = clGetProgramInfo(program_id, CL_PROGRAM_BINARIES,
                            sizeof(unsigned char*),
                            (unsigned char**) &binary_ptr,
                            NULL);
    cl_ensure(err, "clGetProgramInfo()");

    // return vector
    return binary;

}

cl_program
program::get_cl_program()
{
    return program_id;
}

hpx::naming::id_type
program::get_device_id()
{
    return parent_device_id;
}
