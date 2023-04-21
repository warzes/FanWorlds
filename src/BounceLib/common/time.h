/*
* Copyright (c) 2016-2019 Irlan Robson https://irlanrobson.github.io
*
* This software is provided 'as-is', without any express or implied
* warranty.  In no event will the authors be held liable for any damages
* arising from the use of this software.
* Permission is granted to anyone to use this software for any purpose,
* including commercial applications, and to alter it and redistribute it
* freely, subject to the following restrictions:
* 1. The origin of this software must not be misrepresented; you must not
* claim that you wrote the original software. If you use this software
* in a product, an acknowledgment in the product documentation would be
* appreciated but is not required.
* 2. Altered source versions must be plainly marked as such, and must not be
* misrepresented as being the original software.
* 3. This notice may not be removed or altered from any source distribution.
*/

#ifndef B3_TIME_H
#define B3_TIME_H

#include <common/settings.h>

#if defined(_WIN32)

#include <windows.h>

// A timer class that accumulates time.
// Usefull for measuring elapsed times between code sections.
class b3Time
{
public:
    b3Time()
    {
        LARGE_INTEGER c;
        QueryPerformanceCounter(&c);
        m_c0 = c.QuadPart;
        m_t0 = 0.0;
        m_t = 0.0;
    }

    // Get the accumulated time in miliseconds from this timer.
    float64 GetCurrentMilis() const
    {
        return m_t;
    }

    // Get the elapsed time since this timer was updated.
    float64 GetElapsedMilis() const
    {
        return m_t - m_t0;
    }

    // Add the elapsed time since this function was called to this timer.
    void Update()
    {
        static float64 inv_frequency = 0.0;
        if (inv_frequency == 0.0)
        {
            LARGE_INTEGER c;
            QueryPerformanceFrequency(&c);

            float64 cycles_per_s = float64(c.QuadPart);
            float64 s_per_cycle = 1.0 / cycles_per_s;
            float64 ms_per_cycle = 1000.0 * s_per_cycle;
            inv_frequency = ms_per_cycle;
        }

        LARGE_INTEGER c;
        QueryPerformanceCounter(&c);

        float64 dt = inv_frequency * float64(c.QuadPart - m_c0);
        m_c0 = c.QuadPart;
        Add(dt);
    }

    // Add time to this timer.
    void Add(float64 dt)
    {
        m_t0 = m_t;
        m_t += dt;
    }

private:
    u64 m_c0;
    float64 m_t0;
    float64 m_t;
};

#elif defined(__APPLE__)

#include <mach/mach_time.h>

// A timer class that accumulates time.
// Usefull for measuring elapsed times between code sections.
class b3Time
{
public:
    b3Time()
    {
        m_c0 = mach_absolute_time();
        m_t0 = 0.0;
        m_t = 0.0;
    }

    // Get the accumulated time in miliseconds from this timer.
    double GetCurrentMilis() const
    {
        return m_t;
    }

    // Get the elapsed time since this timer was updated.
    double GetElapsedMilis() const
    {
        return m_t - m_t0;
    }

    // Add the elapsed time since this function was called to this timer.
    void Update()
    {
        static double inv_frequency = 0.0;
        if (inv_frequency == 0.0)
        {
            mach_timebase_info_data_t info;
            mach_timebase_info(&info);
            inv_frequency = double(info.numer) / (double(info.denom) * 1.0e6);
        }

        uint64_t c = mach_absolute_time();
        double dt = inv_frequency * (double)(c - m_c0);
        m_c0 = c;
        Add(dt);
    }

    // Add time to this timer.
    void Add(double dt)
    {
        m_t0 = m_t;
        m_t += dt;
    }

private:
    uint64_t m_c0;
    double m_t0;
    double m_t;
};

#elif defined(_arch_dreamcast)

#include <kos.h>

class b3Time
{
public:
    b3Time()
    {
        m_c0 = timer_us_gettime64();
        m_t0 = 0.0;
        m_t = 0.0;
    }

    // Get the accumulated time in miliseconds from this timer.
    double GetCurrentMilis() const
    {
        return m_t;
    }

    // Get the elapsed time since this timer was updated.
    double GetElapsedMilis() const
    {
        return m_t - m_t0;
    }

    // Add the elapsed time since this function was called to this timer.
    void Update()
    {
        uint64_t c = timer_us_gettime64();
        double dt = double(c - m_c0) * 0.000001;
        m_c0 = c;
        Add(dt);
    }

    // Add time to this timer.
    void Add(double dt)
    {
        m_t0 = m_t;
        m_t += dt;
    }

private:
    uint64_t m_c0;
    double m_t0;
    double m_t;
};

#elif defined(__PSP__)

#include <pspthreadman.h>

class b3Time
{
public:
    b3Time()
    {
        m_c0 = sceKernelGetSystemTimeLow();
        m_t0 = 0.0;
        m_t = 0.0;
    }

    // Get the accumulated time in miliseconds from this timer.
    double GetCurrentMilis() const
    {
        return m_t;
    }

    // Get the elapsed time since this timer was updated.
    double GetElapsedMilis() const
    {
        return m_t - m_t0;
    }

    // Add the elapsed time since this function was called to this timer.
    void Update()
    {
        uint64_t c = sceKernelGetSystemTimeLow();
        double dt = double(c - m_c0) * 0.000001;
        m_c0 = c;
        Add(dt);
    }

    // Add time to this timer.
    void Add(double dt)
    {
        m_t0 = m_t;
        m_t += dt;
    }

private:
    uint64_t m_c0;
    double m_t0;
    double m_t;
};

#else

#include <time.h>

// A timer class that accumulates time.
// Usefull for measuring elapsed times between code sections.
class b3Time
{
public:
    b3Time()
    {
        clock_gettime(CLOCK_MONOTONIC, &m_c0);
        m_t0 = 0.0;
        m_t = 0.0;
    }

    // Get the accumulated time in miliseconds from this timer.
    double GetCurrentMilis() const
    {
        return m_t;
    }

    // Get the elapsed time since this timer was updated.
    double GetElapsedMilis() const
    {
        return m_t - m_t0;
    }

    // Add the elapsed time since this function was called to this timer.
    void Update()
    {
        struct timespec c;
        clock_gettime(CLOCK_MONOTONIC, &c);
        double dt = (double)(c.tv_nsec - m_c0.tv_nsec) * 1.0e-6;
        m_c0 = c;
        Add(dt);
    }

    // Add time to this timer.
    void Add(double dt)
    {
        m_t0 = m_t;
        m_t += dt;
    }

private:
    struct timespec m_c0;
    double m_t0;
    double m_t;
};

#endif // B3_PLATFORM

#endif
