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

#include "perfMonitor.h"

namespace OpenGL_Helper {
    
    PerfMonitor::PerfMonitor()
    : current_FPS_(0),
    tv_last_sec_(0),
    last_tick_(0.f),
    tickindex_(0),
    ticksum_(0) {
        for (int32_t i = 0; i < kNumSamples; ++i) ticklist_[i] = 0;
    }
    
    PerfMonitor::~PerfMonitor() {}
    
    double PerfMonitor::UpdateTick(double currentTick) {
        //更新当前样本时先将当前样本的已有值从sum中去掉，然后加上当前样本的当前值到sum中
        ticksum_ -= ticklist_[tickindex_];
        ticksum_ += currentTick;
        ticklist_[tickindex_] = currentTick;
        //更新完当前样本值后，将样本推进到下一个样本
        tickindex_ = (tickindex_ + 1) % kNumSamples;
        
        //只有所有样本位的值都填充进去一遍后，这个平均值才会较准确，如果只填充了30个样本，那么100样本平均值必然被拉低
        return ((double)ticksum_ / kNumSamples);
    }
    
    bool PerfMonitor::Update(float &fFPS) {
        struct timeval Time;
        gettimeofday(&Time, NULL);
        
        double time = Time.tv_sec + Time.tv_usec * 1.0 / 1000000.0;
        //double time = GetCurrentTime();
        //tick是两次时间调用的时间间隔，如其名时刻
        double tick = time - last_tick_;
        //更新本次调用的时间间隔，并加入到总样本中获得平均值，也即本方法所获得每次执行时间间隔都是平均值
        double d = UpdateTick(tick);
        last_tick_ = time;
        
        //超过一秒的间隔后开始更新计算的FPS，否则只更新当前已存在值
        if (Time.tv_sec - tv_last_sec_ >= 1) {
            current_FPS_ = 1.f / d;
            tv_last_sec_ = Time.tv_sec;
            fFPS = current_FPS_;
            return true;
        } else {
            fFPS = current_FPS_;
            return false;
        }
    }
    
}  // namespace OpenGL_Helper
