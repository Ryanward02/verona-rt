// Copyright Microsoft and Project Verona Contributors.
// SPDX-License-Identifier: MIT
#pragma once

#include "cown.h"

#include <functional>
#include <tuple>
#include <utility>
#include <verona.h>

namespace verona::cpp
{
  using namespace verona::rt;

  /**
   * Used to track the type of access request by embedding const into
   * the type T, or not having const.
   */
  template<typename T>
  class Access
  {
    ActualCown<std::remove_const_t<T>>* t;

  public:
    Access(const cown_ptr<T>& c) : t(c.allocated_cown) {}

    template<typename... Args>
    friend class When;

    template<typename T1, typename T2, typename F1, typename F2>
    friend class When2;
  };
  

  template<typename T1, typename T2, typename F1, typename F2>
  class When2
  {

    template<class T>
    struct is_read_only : std::false_type
    {};
    template<class T>
    struct is_read_only<Access<const T>> : std::true_type
    {};


    // Friend necessary, same as when's friend - Args2 == Args.
    template<typename Ty1, typename Ty2, typename Fn1, typename Fn2>
    friend auto when2(cown_ptr<Ty1>& cowns1, cown_ptr<Ty2>& cowns2, Fn1&& f1, Fn2&& f2);

    std::tuple<Access<T1>> cown_tuple1;
    std::tuple<Access<T2>> cown_tuple2;

    /**
     * This uses template programming to turn the std::tuple into a C style
     * stack allocated array.
     * The index template parameter is used to perform each the assignment for
     * each index.
     */
    template<size_t index = 0, typename T>
    void array_assign(Request* requests, std::tuple<Access<T>> cown_tuple)
    {
      if constexpr (index >= sizeof(T))
      {
        return;
      }
      else
      {
        auto p = std::get<index>(cown_tuple);
        if constexpr (is_read_only<decltype(p)>())
          requests[index] = Request::read(p.t);
        else
          requests[index] = Request::write(p.t);
        assert(requests[index].cown() != nullptr);
        array_assign<index + 1>(requests, cown_tuple);
      }
    }

    /**
     * Applies the closure to schedule the behaviour on the set of cowns.
     * We need to figure out how to make this schedule both function atomically.
     * Probably new implementations of schedule_lambda.
     */
    void lambdaClosure(F1&& f1, F2&& f2, std::tuple<Access<T1>> cown_tuple1, std::tuple<Access<T2>> cown_tuple2)
    {
      if constexpr (sizeof(T1) == 0 && sizeof(T2) == 0)
      {
        verona::rt::schedule_lambda_atomic(std::forward<F1>(f1), std::forward<F2>(f2));
      }
      else
      {
        verona::rt::Request requests1[std::tuple_size<decltype(cown_tuple1)>::value];
        verona::rt::Request requests2[std::tuple_size<decltype(cown_tuple2)>::value];
        array_assign(requests1, cown_tuple1);
        array_assign(requests2, cown_tuple2);

        verona::rt::schedule_lambda_atomic(
          sizeof(T1),
          requests1,
          [=, f = std::forward<F1>(f1), cown_tuple = cown_tuple1]() mutable {
            /// Effectively converts ActualCown<T>... to
            /// acquired_cown... .
            auto lift_f = [f =
                             std::forward<F1>(f)](Access<T1> args) mutable {
              f(access_to_acquired<T1>(args));
            };

            std::apply(lift_f, cown_tuple);
          }, 
          sizeof(T2), 
          requests2,
          [=, f = std::forward<F2>(f2), cown_tuple = cown_tuple2]() mutable {
            /// Effectively converts ActualCown<T>... to
            /// acquired_cown... .
            auto lift_f = [f =
                             std::forward<F2>(f)](Access<T2> args) mutable {
              f(access_to_acquired<T2>(args));
            };

            std::apply(lift_f, cown_tuple);
          });
      }
    }

    /**
     * Converts a single `cown_ptr` into a `acquired_cown`.
     *
     * Needs to be a separate function for the template parameter to work.
     */
    template<typename C>
    static acquired_cown<C> access_to_acquired(Access<C> c)
    {
      assert(c.t != nullptr);
      return acquired_cown<C>(*c.t);
    }

  public:

    When2(Access<T1> *cowns1, Access<T2> *cowns2, F1&& f1, F2&& f2) : 
      cown_tuple1(*cowns1),
      cown_tuple2(*cowns2) {
      lambdaClosure(std::move(f1), std::move(f2), cown_tuple1, cown_tuple2);
    }

  };
  /**
   * Class for staging the when creation.
   *
   * Do not call directly use `when`
   *
   * This provides an operator << to apply the closure.  This allows the
   * argument order to be more sensible, as variadic arguments have to be last.
   *
   *   when (cown1, ..., cownn) << closure;
   *
   * Allows the variadic number of cowns to occur before the closure.
   */
  template<typename... Args>
  class When
  {
    template<class T>
    struct is_read_only : std::false_type
    {};
    template<class T>
    struct is_read_only<Access<const T>> : std::true_type
    {};

    // Note only requires friend when Args2 == Args
    // but C++ doesn't like this.
    template<typename... Args2>
    friend auto when(Args2&&... args);

    /**
     * Internally uses AcquiredCown.  The cown is only acquired after the
     * behaviour is scheduled.
     */
    std::tuple<Access<Args>...> cown_tuple;

    /**
     * This uses template programming to turn the std::tuple into a C style
     * stack allocated array.
     * The index template parameter is used to perform each the assignment for
     * each index.
     */
    template<size_t index = 0>
    void array_assign(Request* requests)
    {
      if constexpr (index >= sizeof...(Args))
      {
        return;
      }
      else
      {
        auto p = std::get<index>(cown_tuple);
        if constexpr (is_read_only<decltype(p)>())
          requests[index] = Request::read(p.t);
        else
          requests[index] = Request::write(p.t);
        assert(requests[index].cown() != nullptr);
        array_assign<index + 1>(requests);
      }
    }

    When(Access<Args>... args) : cown_tuple(args...) {}

    /**
     * Converts a single `cown_ptr` into a `acquired_cown`.
     *
     * Needs to be a separate function for the template parameter to work.
     */
    template<typename C>
    static acquired_cown<C> access_to_acquired(Access<C> c)
    {
      assert(c.t != nullptr);
      return acquired_cown<C>(*c.t);
    }

  public:
    /**
     * Applies the closure to schedule the behaviour on the set of cowns.
     */
    template<typename F>
    void operator<<(F&& f)
    {
      if constexpr (sizeof...(Args) == 0)
      {
        verona::rt::schedule_lambda(std::forward<F>(f));
      }
      else
      {
        verona::rt::Request requests[sizeof...(Args)];
        array_assign(requests);

        verona::rt::schedule_lambda(
          sizeof...(Args),
          requests,
          [f = std::forward<F>(f), cown_tuple = cown_tuple]() mutable {
            /// Effectively converts ActualCown<T>... to
            /// acquired_cown... .
            auto lift_f = [f =
                             std::forward<F>(f)](Access<Args>... args) mutable {
              f(access_to_acquired<Args>(args)...);
            };

            std::apply(lift_f, cown_tuple);
          });
      }
    }
  };

  /**
   * Template deduction guide for when.
   */
  template<typename... Args>
  When(Access<Args>...)->When<Args...>;

  /**
   * Template deduction guide for Access.
   */

  // This means if Access is provided with cown_ptr<T>&, the type for the template is T.
  // Must be provided cown_ptr<T>&. How to create cown_ptr<T>&.
  template<typename T>
  Access(const cown_ptr<T>&)->Access<T>;  

  /**
   * Implements a Verona-like `when` statement.
   *
   * Uses `<<` to apply the closure.
   *
   * This should really take a type of
   *   ((cown_ptr<A1>& | cown_ptr<A1>&&)...
   * To get the universal reference type to work, we can't
   * place this constraint on it directly, as it needs to be
   * on a type argument.
   */
  template<typename... Args>
  auto when(Args&&... args)
  {
    return When(Access(args)...);
  }


  /**
   * Allows us to directly type cown_ptr<T>& cowns into std::tuple<Access<cown_ptr<T>&>>. 
   * Needed because parameter packs wont work when we want to provide specific lists of cowns, for more than one lambda.
   * Cannot implement Atomic-Schedule-Parallel-Execution without this.
  */

  template<typename T>
  std::vector<Access<T>> create_cown_Access_vector(std::vector<cown_ptr<T>> cowns) {
    std::vector<Access<T>> vector_ret = std::vector<Access<T>>();
    for (int i = 0; i < cowns.size(); i++) {
      cown_ptr<T>& cown = cowns.at(i);
      vector_ret.emplace_back(Access(cown));
    }
    return vector_ret;
  }

  /**
   * Implements a Verona-like `when` statement, but accepts two sets of cowns, and two lambda functions. This can be passed for atomic scheduling and parallel execution.
   * To do this, we have implmented a "schedule_lambda_atomic" function, that takes all of the arguments as usual, doubled, 
   * and creates a new function that creates 2 p_threads, and executes the lambdas in parallel. It then joins the threads to ensure that they return after parallel exec.
   *
   * Uses `<<` to apply the closure.
   *
   * This should really take a type of
   *   ((cown_ptr<A1>& | cown_ptr<A1>&&)...
   * To get the universal reference type to work, we can't
   * place this constraint on it directly, as it needs to be
   * on a type argument.
   */

  template<typename T, typename... Args>
  auto cowns(Args&&... args) {
    std::vector<T> vector_of_cowns { {args...}};
    return vector_of_cowns;
  }

  template<typename F1, typename F2, typename T1, typename T2>
  auto when2(std::vector<cown_ptr<T1>> args1, std::vector<cown_ptr<T2>> args2, F1&& f1, F2&& f2)
  {
    std::vector<Access<T1>> access_cowns1 = create_cown_Access_vector(args1);
    std::vector<Access<T2>> access_cowns2 = create_cown_Access_vector(args2);

    When2 when = When2(access_cowns1.data(), access_cowns2.data(), std::move(f1), std::move(f2));

    return when;
  }
} // namespace verona::cpp