// Copyright (c)		2013 Damond Howard
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt

#if !defined(EVENT_1_HPP)
#define EVENT_1_HPP

#include <hpx/include/components.hpp>
#include "stubs/event.hpp"

namespace hpx
{
    namespace cuda
    {
        class event
            : public hpx::components::client_base<
                event, stubs::event >
        {
            typedef hpx::components::client_base<
                event, stubs::event>
                base_type;

            public:
                event()
                {}

                event(hpx::future<hpx::naming::id_type> const& gid)
                : base_type(gid)
                {}

                static hpx::lcos::future<float> elapsed_time(hpx::cuda::event event1, hpx::cuda::event event2)
                {
                    float *milliseconds;
                    cuEventElapsedTime(milliseconds, event1.cuda_event(), event2.cuda_event());
                }

                hpx::lcos::future<void> await_async()
                {
                   HPX_ASSERT(this->get_gid());
                   return this->base_type::await_async(this->get_gid());
                }

                void await()
                {
                    HPX_ASSERT(this->get_gid());
                    this->base_type::await(this->get_gid());
                }

                hpx::lcos::future<bool> finished_async()
                {
                    HPX_ASSERT(this->get_gid());
                    return this->base_type::finished_async(this->get_gid());
                }

                bool finished()
                {
                    BOOST_ASSERT(this->get_gid());
                    return this->base_type::finished(this->get_gid());
                }

                hpx::lcos::future<void> trigger_async()
                {
                   BOOST_ASSERT(this->get_gid());
                   return this->base_type::trigger_async(this->get_gid());
                }

                void trigger()
                {
                   BOOST_ASSERT(this->get_gid());
                   this->base_type::trigger(this->get_gid());
                }

                CUevent cuda_event()
                {
                    BOOST_ASSERT(this->get_gid());
                    return this->base_type::cuda_event(this->get_gid());
                }
        };
    }
}
#endif //EVENT_1_HPP
