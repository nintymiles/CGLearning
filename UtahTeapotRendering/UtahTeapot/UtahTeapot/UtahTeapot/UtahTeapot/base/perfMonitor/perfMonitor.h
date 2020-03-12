/*
 * Copyright 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef PERFMONITOR_H_
#define PERFMONITOR_H_

#include <sys/errno.h>
#include <sys/time.h>

namespace OpenGL_Helper{
    
    const int32_t kNumSamples = 100;
    
    /******************************************************************
     * Helper class for a performance monitoring and get current tick time
     */
    class PerfMonitor {
    private:
        float current_FPS_;
        time_t tv_last_sec_;
        
        double last_tick_;
        int32_t tickindex_;
        double ticksum_;
        double ticklist_[kNumSamples];
        
        double UpdateTick(double current_tick);
        
    public:
        PerfMonitor();
        virtual ~PerfMonitor();
        
        bool Update(float &fFPS);
        
        //获得当前系统时间
        static double GetCurrentTime() {
            struct timeval time;
            //知识点：gettimeofday是类unix接口c语言所返回的较精确时间
            gettimeofday(&time, NULL);
            //知识点：timeval是由sec和microseconds组合而成的结构体，两个参数合成了具体的时间值
            double ret = time.tv_sec + time.tv_usec * 1.0 / 1000000.0;
            return ret;
        }
    };
    
}  // namespace OpenGL_Helper
#endif /* PERFMONITOR_H_ */
