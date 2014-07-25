// Copyright (c)       2014 Martin Stumpf
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef BENCHMARK_HPXCL_SINGLE_HPP_
#define BENCHMARK_HPXCL_SINGLE_HPP_

#include "../../../opencl.hpp"
#include "timer.hpp"

using namespace hpx::opencl;
using hpx::lcos::shared_future;

static device   hpxcl_single_device;
static buffer   hpxcl_single_buffer_a;
static buffer   hpxcl_single_buffer_b;
static buffer   hpxcl_single_buffer_c;
static buffer   hpxcl_single_buffer_m;
static buffer   hpxcl_single_buffer_n;
static buffer   hpxcl_single_buffer_o;
static buffer   hpxcl_single_buffer_p;
static buffer   hpxcl_single_buffer_z;
static program  hpxcl_single_program;
static kernel   hpxcl_single_log_kernel;
static kernel   hpxcl_single_exp_kernel;
static kernel   hpxcl_single_mul_kernel;
static kernel   hpxcl_single_add_kernel;
static kernel   hpxcl_single_dbl_kernel;


static void hpxcl_single_initialize( hpx::naming::id_type node_id, 
                                     size_t vector_size)
{

    // Query all devices on local node
    std::vector<device> devices = get_devices( node_id, 
                                               CL_DEVICE_TYPE_GPU,
                                               "OpenCL 1.1" ).get();

/*
    // print devices
    hpx::cout << "Devices:" << hpx::endl;
    for(cl_uint i = 0; i < devices.size(); i++)
    {
        
        device cldevice = devices[i];

        // Query name
        std::string device_name = device::device_info_to_string(
                                    cldevice.get_device_info(CL_DEVICE_NAME));
        std::string device_vendor = device::device_info_to_string(
                                    cldevice.get_device_info(CL_DEVICE_VENDOR));

        hpx::cout << i << ": " << device_name << " (" << device_vendor << ")"
                  << hpx::endl;

    }

    // Lets you choose a device
    size_t device_num;
    hpx::cout << "Choose device: " << hpx::endl;
    std::cin >> device_num;
    if(device_num < 0 || device_num >= devices.size())
        exit(0);

    // Select a device
    hpxcl_single_device = devices[device_num];
*/

    size_t device_id = 0;
    // print device
    hpx::cout << "Device:" << hpx::endl;
    {
        
        device cldevice = devices[device_id];

        // Query name
        std::string device_name = device::device_info_to_string(
                                    cldevice.get_device_info(CL_DEVICE_NAME));
        std::string device_vendor = device::device_info_to_string(
                                    cldevice.get_device_info(CL_DEVICE_VENDOR));

        hpx::cout << "    " << device_name << " (" << device_vendor << ")"
                  << hpx::endl;

    }

    // Select a device
    hpxcl_single_device = devices[device_id];

    // Create program
    hpxcl_single_program = hpxcl_single_device.create_program_with_source(
                                                                    gpu_code);

    // Build program
    hpxcl_single_program.build();

    // Create kernels
    hpxcl_single_log_kernel = hpxcl_single_program.create_kernel("logn");
    hpxcl_single_exp_kernel = hpxcl_single_program.create_kernel("expn");
    hpxcl_single_mul_kernel = hpxcl_single_program.create_kernel("mul");
    hpxcl_single_add_kernel = hpxcl_single_program.create_kernel("add");
    hpxcl_single_dbl_kernel = hpxcl_single_program.create_kernel("dbl");
 
    // Generate buffers
    hpxcl_single_buffer_a = hpxcl_single_device.create_buffer(
                                    CL_MEM_READ_ONLY,
                                    vector_size * sizeof(float));
    hpxcl_single_buffer_b = hpxcl_single_device.create_buffer(
                                    CL_MEM_READ_ONLY,
                                    vector_size * sizeof(float));
    hpxcl_single_buffer_c = hpxcl_single_device.create_buffer(
                                    CL_MEM_READ_ONLY,
                                    vector_size * sizeof(float));
    hpxcl_single_buffer_m = hpxcl_single_device.create_buffer(
                                    CL_MEM_READ_WRITE,
                                    vector_size * sizeof(float));
    hpxcl_single_buffer_n = hpxcl_single_device.create_buffer(
                                    CL_MEM_READ_WRITE,
                                    vector_size * sizeof(float));
    hpxcl_single_buffer_o = hpxcl_single_device.create_buffer(
                                    CL_MEM_READ_WRITE,
                                    vector_size * sizeof(float));
    hpxcl_single_buffer_p = hpxcl_single_device.create_buffer(
                                    CL_MEM_READ_WRITE,
                                    vector_size * sizeof(float));
    hpxcl_single_buffer_z = hpxcl_single_device.create_buffer(
                                    CL_MEM_WRITE_ONLY,
                                    vector_size * sizeof(float));

    // Initialize a list of future events for asynchronous set_arg calls
    std::vector<shared_future<void>> set_arg_futures;

    // set kernel args for exp
    set_arg_futures.push_back(
        hpxcl_single_exp_kernel.set_arg_async(0, hpxcl_single_buffer_m));
    set_arg_futures.push_back(
        hpxcl_single_exp_kernel.set_arg_async(1, hpxcl_single_buffer_b));
    
    // set kernel args for add
    set_arg_futures.push_back(
        hpxcl_single_add_kernel.set_arg_async(0, hpxcl_single_buffer_n));
    set_arg_futures.push_back(
        hpxcl_single_add_kernel.set_arg_async(1, hpxcl_single_buffer_a));
    set_arg_futures.push_back(
        hpxcl_single_add_kernel.set_arg_async(2, hpxcl_single_buffer_m));

    // set kernel args for dbl
    set_arg_futures.push_back(
        hpxcl_single_dbl_kernel.set_arg_async(0, hpxcl_single_buffer_o));
    set_arg_futures.push_back(
        hpxcl_single_dbl_kernel.set_arg_async(1, hpxcl_single_buffer_c));
    
    // set kernel args for mul
    set_arg_futures.push_back(
        hpxcl_single_mul_kernel.set_arg_async(0, hpxcl_single_buffer_p));
    set_arg_futures.push_back(
        hpxcl_single_mul_kernel.set_arg_async(1, hpxcl_single_buffer_n));
    set_arg_futures.push_back(
        hpxcl_single_mul_kernel.set_arg_async(2, hpxcl_single_buffer_o));
    
    // set kernel args for log
    set_arg_futures.push_back(
        hpxcl_single_log_kernel.set_arg_async(0, hpxcl_single_buffer_z));
    set_arg_futures.push_back(
        hpxcl_single_log_kernel.set_arg_async(1, hpxcl_single_buffer_p));
    
    // wait for function calls to trigger
    BOOST_FOREACH(shared_future<void> & future, set_arg_futures)
    {
        future.wait();
    }
    

}

static boost::shared_ptr<std::vector<char>>
hpxcl_single_calculate(std::vector<float> &a,
                       std::vector<float> &b,
                       std::vector<float> &c,
                       double* t_nonblock,
                       double* t_sync,
                       double* t_finish)
{
    // do nothing if matrices are wrong
    if(a.size() != b.size() || b.size() != c.size())
    {
        return boost::shared_ptr<std::vector<char>>();
    }

    size_t size = a.size();

    // copy data to gpu
    shared_future<event> write_a_event = 
           hpxcl_single_buffer_a.enqueue_write(0, size*sizeof(float), a.data());
    shared_future<event> write_b_event =
           hpxcl_single_buffer_b.enqueue_write(0, size*sizeof(float), b.data());
    shared_future<event> write_c_event =
           hpxcl_single_buffer_c.enqueue_write(0, size*sizeof(float), c.data());

    // wait for write to finish
    write_a_event.get().await();
    write_b_event.get().await();
    write_c_event.get().await();

    // start time measurement
    timer_start();

    // set work dimensions
    work_size<1> dim;
    dim[0].offset = 0;
    dim[0].size = size;

    // run exp kernel
    shared_future<event> kernel_exp_event =
                             hpxcl_single_exp_kernel.enqueue(dim, write_b_event);

    // run add kernel
    std::vector<shared_future<event>> add_dependencies;
    add_dependencies.push_back(kernel_exp_event);
    add_dependencies.push_back(write_a_event);
    shared_future<event> kernel_add_event = 
                          hpxcl_single_add_kernel.enqueue(dim, add_dependencies);

    // run dbl kernel
    shared_future<event> kernel_dbl_event =
                             hpxcl_single_dbl_kernel.enqueue(dim, write_c_event);

    // run mul kernel
    std::vector<shared_future<event>> mul_dependencies;
    mul_dependencies.push_back(kernel_add_event);
    mul_dependencies.push_back(kernel_dbl_event);
    shared_future<event> kernel_mul_event = 
                          hpxcl_single_mul_kernel.enqueue(dim, mul_dependencies);

    // run log kernel
    shared_future<event> kernel_log_event_future = 
                          hpxcl_single_log_kernel.enqueue(dim, kernel_mul_event);

    ////////// UNTIL HERE ALL CALLS WERE NON-BLOCKING /////////////////////////

    // get time of non-blocking calls
    *t_nonblock = timer_stop();

    // wait for all nonblocking calls to finish
    event kernel_log_event = kernel_log_event_future.get();

    // get time of synchronization
    *t_sync = timer_stop();

    // wait for the end of the execution
    kernel_log_event.await();

    // get total time of execution
    *t_finish = timer_stop();
 
    // enqueue result read
    shared_future<event> read_event_future = 
                       hpxcl_single_buffer_z.enqueue_read(0, size*sizeof(float),
                                                          kernel_log_event);

    // wait for enqueue_read to return the event
    event read_event = read_event_future.get();

   // wait for calculation to complete and return data
    boost::shared_ptr<std::vector<char>> data_ptr = read_event.get_data().get();
   
    // return the computed data
    return data_ptr;


}

static void hpxcl_single_shutdown()
{

    // release buffers
    hpxcl_single_buffer_a = buffer();
    hpxcl_single_buffer_b = buffer();
    hpxcl_single_buffer_c = buffer();
    hpxcl_single_buffer_m = buffer();
    hpxcl_single_buffer_n = buffer();
    hpxcl_single_buffer_o = buffer();
    hpxcl_single_buffer_p = buffer();
    hpxcl_single_buffer_z = buffer();

    // release kernels
    hpxcl_single_dbl_kernel = kernel();
    hpxcl_single_add_kernel = kernel();
    hpxcl_single_mul_kernel = kernel();
    hpxcl_single_exp_kernel = kernel();
    hpxcl_single_log_kernel = kernel();

    // release program
    hpxcl_single_program = program();
    
    // delete device
    hpxcl_single_device = device();

}





















#endif //BENCHMARK_HPXCL_SINGLE_HPP_

