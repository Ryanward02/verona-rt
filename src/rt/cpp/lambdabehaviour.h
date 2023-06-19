// Copyright Microsoft and Project Verona Contributors.
// SPDX-License-Identifier: MIT
#pragma once

#include "../sched/behaviour.h"
#include "../sched/notification.h"
#include "vobject.h"
#include <pthread.h>

namespace verona::rt
{
  template<TransferOwnership transfer = NoTransfer, typename T>
  static void schedule_lambda(Cown* c, T&& f)
  {
    Behaviour::schedule<T, transfer>(c, std::forward<T>(f));
  }

  template<TransferOwnership transfer = NoTransfer, typename T>
  static void schedule_lambda(size_t count, Cown** cowns, T&& f)
  {
    Behaviour::schedule<T, transfer>(count, cowns, std::forward<T>(f));
  }

  template<TransferOwnership transfer = NoTransfer, typename T>
  static void schedule_lambda(size_t count, Request* requests, T&& f)
  {
    Behaviour::schedule<T, transfer>(count, requests, std::forward<T>(f));
  }

  template<typename T>
  static void schedule_lambda(T&& f)
  {
    auto w = Closure::make([f = std::forward<T>(f)](Work* w) mutable {
      f();
      return true;
    });
    Scheduler::schedule(w);
  }

  template<typename F1>
  static void* thread_func(void* arg)
  {
    auto* func = static_cast<F1*>(arg);
    (*func)();
    return nullptr;
  }

  template<TransferOwnership transfer = NoTransfer, typename T1, typename T2>
  static void schedule_lambda_atomic(size_t count1, Request* requests1, T1&& f1, size_t count2, Request* requests2, T2&& f2)
  {
    Behaviour::schedule2<T1, T2, transfer>(count1, requests1, std::move(f1), count2, requests2, std::move(f2));
  }

  template<typename T1, typename T2>
  static void schedule_lambda_atomic(T1&& f1, T2&& f2)
  {
    
    auto w1 = Closure::make([f1 = std::forward<T1>(f1)](Work* w1) mutable {
      f1();
      return true;
    });

    auto w2 = Closure::make([f2 = std::forward<T2>(f2)](Work* w2) mutable {
      f2();
      return true;
    });

    Scheduler::schedule(w1);
    Scheduler::schedule(w2);
  }

  // TODO super minimal version initially, just to get the tests working.
  // Should be expanded to cover multiple cowns.
  template<typename T>
  inline Notification* make_notification(Cown* cown, T&& f)
  {
    Request requests[] = {Request::write(cown)};
    return Notification::make<T>(1, requests, std::forward<T>(f));
  }

} // namespace verona::rt
