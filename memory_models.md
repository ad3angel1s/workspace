Table of Contents
=================

   * [Memory Models](#memory-models)
      * [C language](#c-language)
         * [Standard references](#standard-references)
            * [5.1.2.3p1](#5123p1)
            * [5.1.2.3.p4](#5123p4)
            * [5.1.2.3p5](#5123p5)
            * [5.1.2.3p6](#5123p6)
         * [Usage of volatile](#usage-of-volatile)
            * [Volatile: almost useless for multithreaded programming](#volatile-almost-useless-for-multithreaded-programming)
      * [Atomicity and memory ordering](#atomicity-and-memory-ordering)

# Memory Models <a name="memory-models"></a>

This is a summary page for my ongoing analysis of memory models in the context of the C/C++ programming languages and corresponding impact on the architecture specific memory models. This aspect is important in multithreaded programming in multicore systems, and also for efficient single core programming in a multithreaded environment.

## C language <a name="c-language"></a>

### Standard references <a name="standard-references"></a>
The C language in its first standardization (C89) didn't contemplate a memory model. The C language defines its behaviour in terms of an abstract machine, i.e. the execution environment is described [5.1.2.3](http://port70.net/~nsz/c/c11/n1570.html#5.1.2.3). More relevant items are below (confusingly enough, these references are taken from the C11 standard which has hints to the newly addition related to the C11/C++11 memory model, i.e. atomic references, should be ignored for the sake of this introduction):

#### 5.1.2.3p1 <a name="5123p1"></a>
The semantic descriptions in this International Standard describe the behavior of an abstract machine in which issues of optimization are irrelevant.

#### 5.1.2.3.p4 <a name="5123p4"></a>
In the abstract machine, all expressions are evaluated as specified by the semantics. An actual implementation need not evaluate part of an expression if it can deduce that its value is not used and that no needed side effects are produced (including any caused by calling a function or accessing a volatile object).

#### 5.1.2.3p5 <a name="5123p5"></a>
When the processing of the abstract machine is interrupted by receipt of a signal, the values of objects that are neither lock-free atomic objects nor of type `volatile sig_atomic_t` are unspecified, as is the state of the floating-point environment. The value of any object modified by the handler that is neither a lock-free atomic object nor of type `volatile sig_atomic_t` becomes indeterminate when the handler exits, as does the state of the floating-point environment if it is modified by the handler and not restored to its original state.

#### 5.1.2.3p6 <a name="5123p6"></a>
The least requirements on a conforming implementation are:

 * Accesses to volatile objects are evaluated strictly according to the rules of the abstract machine.
 * At program termination, all data written into files shall be identical to the result that execution of the program according to the abstract semantics would have produced.
 * The input and output dynamics of interactive devices shall take place as specified in [7.21.3](http://port70.net/~nsz/c/c11/n1570.html#7.21.3). The intent of these requirements is that unbuffered or line-buffered output appear as soon as possible, to ensure that prompting messages actually appear prior to a program waiting for input.

This is the observable behaviour of the program.

Essentially, the C abstract machine defines an execution environment, an infinite set of registers and an infinite, contiguous linearly addressed memory space in which execution happens. Objects are designed with a storage class (auto, extern, static, register) which describes the scope and visibility of the object. As the execution environment is essentially tied to a single thread of execution (with no specific mention to concurrent execution), the C language offers limited insight into memory model issues and delegates this to the programmer / platform / library. It's difficult to write portable multithreaded code in C (at least prior to C11).

The only thing that a C compiler needs to guarantee is that in a conforming implementation, the sequence of accesses (read or write) to volatile objects is respected (i.e. the compiler can't reorder accesses to volatile objects, but it can reorder accesses of volatile objects wrt normal objects), at program termination all data written to files must be the same as the one specified by the abstract machine semantics, and the behaviour with input/output interactive devices is consistent in the way all output is flushed before starting waiting for input. This is dubbed the *AS-IF* rule, which basically describes any conforming implementation as an implementation which follows these three rules (the observable behaviour of the abstract machine must be preserved), everything else can be optimized by the compiler, i.e. it is not guaranteed by the language, i.e. it's undefined behaviour. Basically all multithreading behaviour in C prior to C99 is built *on top of* undefined behaviour.

### Usage of volatile <a name="usage-of-volatile"></a>
A common deduction is that the usage of `volatile` qualified variables can somehow help in multithreaded programming. The only thing that volatile does is to *mark explicit read or write accesses*, i.e. the compiler can't optimize them out by following the first rule of 5.1.2.3p6. There are commonly three acceptable usages of volatile in C (referenced by the standard itself):

 1. A `volatile sig_atomic_t` is the only object which value is guaranteed to be consistent across an interruption of the execution context by the receipt of a signal (in addition to any lock-free atomic object, but this should be specific to C11 - i.e. atomics are not present in the C89 standard)
 2. Memory - mapped I/O, where a variable mapped to a particular memory location can change outside of the current scope where the variable is used, because it is effectively associated to an hardware process which can change its value in memory, or for which a write must be always performed to memory because it may have effects which are outside the visibility of the compiler
 3. In `setjmp`/`longjmp` context, the value of local (i.e. with `auto` storage class) objects which are not marked `volatile` is not consistent and is indeterminate if it is accessed between the `setjmp` and `longjmp`. In brief `setjmp()` stores the state of the execution environment and `longjmp()` restores the state to that saved value, i.e. they clobber the registers, so if an object is not marked `volatile` and gets accessed, it does not get read back from memory but from the register which has been clobbered. Example here:

    ```c
    #include <setjmp.h>
    #include <stdio.h>

    int main()
    {
      jmp_buf env;
      volatile int a = 69;
      int b = 42;

      if (!setjmp(env)) {
        a=0; b=0; longjmp(env,1);
      } else {
        printf("a is %d and b is %d\n", a, b);
      }
      return 0;
    }
    ```
    Between `setjmp` and `longjmp` local variables can change their values in a way unpredictable for the compiler, so volatile is needed. The code above behaves differently with `-O2` because the compiler will optimize the read of `b` in the else branch away assuming it will never be equal to 0, so it will print the wrong value (i.e. 42!)

Every other usage of `volatile`, especially in the context of helping multithreaded programming, is *not necessary*. It might be sufficient, but at the cost of performance penalty (i.e. declaring a `volatile` variable precludes any compiler optimization), or it might still not solve the problem for which is being used (i.e. like using the wrong tool for the job, it might be ineffective in solving the race condition or even introduce a bug). A famous opinion on this is from Arch Robinson of Intel (now at nVidia), which I report entirely as it seems that Intel blogs are disappearing with time (or maybe with ex-employees):

#### Volatile: almost useless for multithreaded programming <a name="volatile-almost-useless-for-multithreaded-programming"></a>

There is a widespread notion that the keyword `volatile` is good for multi-threaded programming. I've seen interfaces with `volatile` qualifiers justified as "it might be used for multi-threaded programming". I thought was useful until the last few weeks, when it finally dawned on me (or if you prefer, got through my thick head) that `volatile` is almost useless for multi-threaded programming. I'll explain here why you should scrub most of it from your multi-threaded code.

Hans Boehm points out that there are only [three portable uses for volatile](http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n2016.html). I'll summarize them here:

 * marking a local variable in the scope of a `setjmp` so that the variable does not roll back after a `longjmp`
 * memory that is modified by an external agent or appears to be because of a screwy memory mapping
 * signal handler mischief

None of these mention multi-threading. Indeed, Boehm's paper points to a 1997 [comp.programming.threads](http://groups.google.com/group/comp.programming.threads/browse_frm/thread/399797d84a5c37d5/eb60e71097dd5755) discussion where two experts said it bluntly:

> "Declaring your variables volatile will have no useful effect, and will simply cause your code to run a *lot* slower when you turn on optimisation in your compiler." - Bryan O' Sullivan

> "...the use of volatile accomplishes nothing but to prevent the compiler from making useful and desirable optimizations, providing no help whatsoever in making code "thread safe". " - David Butenhof

If you are multi-threading for the sake of speed, slowing down code is definitely not what you want. For multi-threaded programming, there two key issues that `volatile` is often mistakenly thought to address:

1. atomicity
2. memory consistency, i.e. the order of a thread's operations as seen by another thread.

Let's deal with (1) first. `volatile` does not guarantee atomic reads or writes. For example, a `volatile` read or write of a 129-bit structure is not going to be atomic on most modern hardware. A `volatile` read or write of a 32-bit int is atomic on most modern hardware, but `volatile` has nothing to do with it. It would likely be atomic without the `volatile`. The atomicity is at the whim of the compiler. There's nothing in the C or C++ standards that says it has to be atomic.
Now consider issue (2). Sometimes programmers think of `volatile` as turning off optimization of volatile accesses. That's largely true in practice. But that's only the volatile accesses, not the non-volatile ones. Consider this fragment:
```c
volatile int Ready;      
int Message[100];     

void foo( int i ) {     
    Message[i/10] = 42;     
    Ready = 1;     
}
```
It's trying to do something very reasonable in multi-threaded programming: write a message and then send it to another thread. The other thread will wait until Ready becomes non-zero and then read Message. Try compiling this with `gcc -O2 -S` using `gcc 4.0`, or `icc`. Both will do the store to `Ready` first, so it can be overlapped with the computation of `i/10`. The reordering is not a compiler bug. It's an aggressive optimizer doing its job.

You might think the solution is to mark *all* your memory references `volatile`. That's just plain silly. As the earlier quotes say, it will just slow down your code. Worst yet, it might not fix the problem. Even if the compiler does not reorder the references, the hardware might. In this example, x86 hardware will not reorder it. Neither will an Itanium™ processor, because Itanium compilers insert memory fences for volatile stores. That's a clever Itanium extension. But chips like Power™ will reorder. What you really need for ordering are memory fences, also called memory barriers. A memory fence prevents reordering of memory operations across the fence, or in some cases, prevents reordering in one direction. Paul McKenney's article [Memory Ordering in Modern Microprocessors](http://www.linuxjournal.com/article/8211) explains them. Sufficient for discussion here is that `volatile` has nothing to do with memory fences.
So what's the solution for multi-threaded programming? Use a library or language extension that implements the atomic and fence semantics. When used as intended, the operations in the library will insert the right fences. Some examples:

* POSIX threads
* Windows™ threads
* OpenMP
* TBB

For example, the parallel reduction template in TBB does all the right fences so you don't have to worry about them.
I spent part of this week scrubbing `volatile` from the TBB task scheduler. We were using `volatile` for memory fences because version 1.0 targeted only x86 and Itanium. For Itanium, `volatile` did imply memory fences. And for x86, we were just using one compiler, and catering to it. All atomic operations were in the binary that we compiled. But now with the open source version, we have to pay heed to other compilers and other chips. So I scrubbed out `volatile`, replacing them with explicit *load-with-acquire* and *store-with-release* operations, or in some cases plain loads and stores. Those operations themselves are implemented using `volatile`, but that's largely for Itanium's sake.  Only one `volatile` remained, ironically on an unshared local variable! See file `src/tbb/task.cpp` in the latest download if your curious about the oddball survivor.
\- Arch

## Atomicity and memory ordering <a name="atomicity-and-memory-ordering"></a>

The comment at the end of the previous section introduces what is really needed for consistent and correct multithreaded behaviour in the semantics of memory accesses: atomicity and memory ordering. It can be noted that the usage of `volatile` can indirectly help in some silly way as described above, but it's usually the wrong tool for the job. A detailed interesting discussion with demystified counter examples is presented [here](https://www.reddit.com/r/programming/comments/2z9iza/volatile_almost_useless_for_multithreaded/). Important thing note is that somehow `volatile` is mystified as useful in multithreaded programming (for example, in MSVC `volatile` is extended to have memory ordering semantics, i.e. a `volatile` read has *Acquire* semantics and and a `volatile` write as *Release* semantics, or in some cases the underlying platform architecture memory model enforces atomicity or memory ordering, i.e. the compiler adds fences on volatile accesses). This is the usual counter - example for `volatile` usage:
```c
void iterate(const volatile bool *cancel) {
    while (! *cancel) {
            stuff();
    }
}
```

but what we need to note is that in this case, although we use `volatile` to ensure the read is performed, it's atomicity of the read that clears the undefined behaviour for multi-threaded programming. Note that atomicity doesn't imply `volatile`, but there is debate whether atomic could be optimized at all by the compiler, read SO [here](https://stackoverflow.com/questions/41820539/can-and-does-the-compiler-optimize-out-two-atomic-loads) for links.

Important point is that we finally understand the need of a memory model, i.e. to describe uniquely the semantics of atomicity and memory ordering which are the semantics needed for correct multithreaded programming. The compiler will then map these semantics to the corresponding memory model of the underlying platform, which is architecture dependent, in a unique way, without resorting to language-level undefined behaviour. We can think of undefined behaviour as everything which we don't have to bridge the language in the abstract machine to the underlying platform machine.
If a language exposes memory ordering and atomic semantics, it exposes all the semantics we need to write meaningful multithreaded code. It does not mean that we won't need any locking mechanism, but it means that locking, if needed, could be implemented in terms of those semantics (without reverting to undefined behaviour/language specific intrinsics). It also implies that we could write lockless algorithms by appropriate usage of those semantics (lockless meaning an algorithm that does not use locks / does not put the thread to sleep on some wait). Sometimes you read the term lock-free, which is a theoretical definition (same context of wait-free, etc, see [here](https://stackoverflow.com/questions/20467247/whats-the-difference-between-lockless-and-lockfree)).
