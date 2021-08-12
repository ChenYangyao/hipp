Logging 
================

.. include:: /global.rst

Logging is a critical part of a mature software. Is is used to keep human-readable 
records of the execution of the software. If something happens - e.g., a regular event 
marking up the progress of s subroutine, a critical/erroneous event, or event
a corruption, library developers and users could read the logging records and
make diagnosis.

For scientific computation, the importance of logging system is usually 
underestimated, because most scientific executables are developed 
quick-and-dirty. Without enough logging, the maintainers, possibly 
the young students, easily get lost on the destructive errors.

HIPP provides a easy-to-use logging system to produce human-readable logging 
records. The utilities include 

- The logger class :class:`~HIPP::PLogStream`. It is very like
  :class:`~HIPP::PStream`, except that more methods are provided to 
  control the logging behavior.
- The global static variable, :var:`~HIPP::plog`. It is a instance 
  of :class:`~HIPP::PLogStream`, which allows direct logging into the standard 
  output.

The examples in this section assume the following inclusion and namespace
declaration::

    #include <hippcntl.h>
    using namespace HIPP;

This includes and exposes all the declarations in the CNTL module. For 
detailed compilation guide, see the API-ref :ref:`api-cntl-usage`. 
All the example codes in this section can be downloaded at :download:`cntl/logging.cpp </../example/tutorial/cntl/logging.cpp>`.

Basic Logging
----------------

To put log records into the standard output, use :var:`~HIPP::plog`. 
It can be used like the :class:`~HIPP::PStream` class introduced 
in the tutorial :ref:`tutor-cntl-pstream`::

    vector<int> v{1,2,3,4};
    plog << "Begin computing the sum of {", v, "}", endl;
    auto sum = std::accumulate(v.begin(), v.end(), 0);
    plog << "Done. Get ", sum, endl;

Elementary types, standard containers and utils, and anything with overloaded 
``operator<<`` can be streamed into :var:`~HIPP::plog`.

The output is:

.. code-block:: text 

    Begin computing the sum of {1,2,3,4}
    Done. Get 10

There is nothing special. But a real program would be more complex - 
functions on the calling chain are nested as a stack structure. For example, 
the above "summation of vector" can be written as a separate function and 
called by the main function. In this case, use :func:`~HIPP::PLogStream::push_g`
to indicate the entrance of a new stack level::

    int find_sum(const vector<double> &v) {
        auto _ = plog.push_g(emFF);
        plog << "Begin computing the sum of {", v, "}", endl;
        auto sum = std::accumulate(v.begin(), v.end(), 0);
        plog << "Done. Get ", sum, endl;
        return sum;
    }

    int main(int argc, char const *argv[])
    {
        auto sum1 = find_sum({1,2,3}),
            sum2 = find_sum({4,5,6});
        plog << "Get sums ", sum1, " and ", sum2, endl;
        return 0;
    }

Here, :func:`~HIPP::PLogStream::push_g` accepts a list of printable arguments,
prints them, and return a guard object. The macro :c:macro:`emFF` is a list 
of predefined strings indicating the current file and function.
We name the returned guard as "``_``" because
we do not direct use it. At the end of the function ``find_sum``, the guard 
is destructed and the stack level is restored to the caller.
The output indents more for higher stack level:

.. code-block:: text

    [file] logging.cpp, [func] find_sum 
      |- Begin computing the sum of {1,2,3}
      |- Done. Get 6
    [file] logging.cpp, [func] find_sum 
      |- Begin computing the sum of {4,5,6}
      |- Done. Get 15
    Get sums 6 and 15


Controlling the Priority Level
------------------------------

Each :class:`~HIPP::PLogStream` instance has a "current priority level" and 
a "used priority level".
Each message you print to the stream has its priority level equal to the 
"current priority level" of the stream. If it is lower than the 
"use priority level", the message does not show. With this feature, 
you can easily filter out undesired messages.
The default current and used priority levels are :enumerator:`~HIPP::PLogStream::LV_NOTICE`.

When entering a new scope, you may change the current priority level by calling 
:func:`~HIPP::PLogStream::push_at`. This method is similar to :func:`~HIPP::PLogStream::push_g`,
but with an additional first argument specifying the desired level. For example::

  void priority_level() {
      plog << "A NOTICE", endl;
      {
          auto _ = plog.push_at(plog.LV_INFO, "A new scope with INFO priority");
          plog << "An INFO", endl;
          plog << "Another INFO", endl;
      }
      plog << "Another NOTICE", endl;
  }

Here, in the inner scope (formed by the pair of braces, ``{}``), 
we set the level to :enumerator:`~HIPP::PLogStream::LV_INFO`, which is lower 
than the NOTICE level.

Then, a call of the function ``priority_level()``::

  priority_level();

outputs only

.. code-block:: text

  A NOTICE
  Another NOTICE

where the INFO level messages in the inner scope do not appear.

You can set the "used priority level" with :func:`~HIPP::PLogStream::set_level_used`.
For example, set it to a INFO level::

    plog.set_level_used(plog.LV_INFO);
    priority_level();

This time, the output includes the lower-level messages:

.. code-block:: text

    A NOTICE
    A new scope with INFO priority
      |- An INFO
      |- Another INFO
    Another NOTICE


