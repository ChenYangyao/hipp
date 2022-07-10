Exception Handling 
===================

Exceptions handling is the most difficult part for any well-developed software. 
Because of the diversity of exceptions, HIPP is not designed to automatically solve them. 
Instead, HIPP provides an **Unified Exception Model** which reports exceptions 
to users, and leave users freedom to handling it.

HIPP's exception system is designed by the standard way in modern C++. HIPP provides 
layers of exceptions classes, which are all derived from the root exception class ``std::exception``.
One special thing is that
each exception class in HIPP has two super classes, one describes the application which 
leads to the exception and one describes the category, i.e., the class, of the it.
The detail description of the exception layers can be found in the 
:ref:`Exception Layers <api-cntl-exception-layers>` part
of the API documentation. In the following, we briefly describe how to throw an 
exception and how to catch it. All the example codes in the below sections can be downloaded at:  :download:`cntl/exception-usage.cpp </../example/tutorial/cntl/exception-usage.cpp>`.

In HIPP, almost any of the exception classes ``X`` has the following features:

-   It has a super class named like ``ErrAppY``, which describes the application 
    that leads to the exception.
-   It has a super class named like ``ErrClassZ``, which describes the category, i.e., the 
    class of the exception.
-   ``X``, ``ErrAppY`` and ``ErrClassZ`` are all subclass of ``std::exception``.
-   It has a constructor ``X()``, which accepts an error number typed ``X::errno_t``.
    The valid error numbers are enumerators inside the class domain defined by ``X``.
-   It has a method ``what()``, which returns a C-style string constant indicating the 
    detail of the exception. Is has a method ``whats()``, which returns a ``std::string`` that gives 
    more detail of the exception.
-   It has a method ``get_errno()``, which returns the error number, and a method ``set_errno()``, 
    which resets the error number.

Throwing and Catching Exceptions
------------------------------------

For example, the exception :class:`HIPP::ErrLogic` is thrown when a general logic error happens.
It has a super class :class:`HIPP::ErrAppDefault`, which means the logic error is general, not 
cause by any specific application. It has a super class :class:`HIPP::ErrClassLogic`, which 
means it belongs to the category of logic error.

To throw such a logic error, just use the the key-word ``throw``. To catch it, just 
use the key-word ``catch``. The following codes throw a logic error with an error number 
``eINVALIDARG`` indicating that the reason of the error is an invalid argument::

    try{
        throw HIPP::ErrLogic(HIPP::ErrLogic::eINVALIDARG);
    }catch(const HIPP::ErrLogic &e){
        HIPP::pout << "Error detail by what(): ", e.what(), '\n',
            "Error detail by whats(): ", e.whats(), '\n',
            "Error number: ", e.get_errno(), endl;
    }

After catching the error, the codes use ``what()`` and ``whats()`` to get the detail 
of the it. ``whats()`` is more verbose, but the overhead may be larger. The call of 
``get_errno()`` returns the error number ``eINVALIDARG``.

The output is 

.. code-block:: text 

    Error detail by what(): eINVALIDARG
    Error detail by whats(): Application: default | Class: logic error | Type: eINVALIDARG
    Error number: 3

If an exception is not caught, it propagates upward along the calling chain, 
unwinds the stack on the chain, and eventually terminates the whole process. 
In most cases, this is just what you want. However, any well-developed software 
must take care of these exceptions.

Catching Exceptions by their Application or Category 
-------------------------------------------------------

It is usual that many exceptions can be thrown by a piece of code. Sometimes, 
you just want to catch any of them, report the detail, and perform some protection 
procedures before terminating the program.

A simple way is to catch all the exceptions by ``std::exception``. However, HIPP 
provides a layered model of exception, so that you can catch exceptions in a 
more flexible way. For example, you may want to catch any logic error and 
runtime error, but leave other errors un-caught since they may not be recoverable.

You can achieve this by catching the exception :class:`HIPP::ErrClass`, which 
is a super class of both :class:`HIPP::ErrLogic` and :class:`HIPP::ErrRuntime`::

    try{
        throw HIPP::ErrLogic(HIPP::ErrLogic::eINVALIDARG);
        throw HIPP::ErrRuntime(HIPP::ErrRuntime::eRANGE);
    }catch(const HIPP::ErrClass &e){
        HIPP::pout << "Error detail by what(): ", e.what(), '\n',
            "Error detail by whats(): ", e.whats(), '\n',
            "Error number: ", e.get_errno(), endl;
        if( e.get_errno() == e.eLOGIC ){
            // Do something for a logic error, e.g.,
            HIPP::perr << "A logic error", endl;
        }else if( e.get_errno() == e.eRUNTIME ){
            // Do something for a runtime error, e.g.,
            HIPP::perr << "A runtime error", endl;
        }else{
            // Other errors, just re-throw.
            throw;
        }
    }

:class:`HIPP::ErrClass` also has ``what()`` and ``whats()`` methods which return 
the detail of the exception. The ``get_errno()`` of it, different from those 
of :class:`HIPP::ErrLogic` and :class:`HIPP::ErrRuntime`, returns the category 
of the exception. You may compare it with any specific category like :enumerator:`eLOGIC <HIPP::ErrClass::eLOGIC>`
or :enumerator:`eRUNTIME <HIPP::ErrClass::eRUNTIME>` to check whether it is a logic error 
or a runtime error or other categories, and perform necessary procedures for each different case.

The output of above codes is 

.. code-block::

    Error detail by what(): eINVALIDARG
    Error detail by whats(): Application: default | Class: logic error | Type: eINVALIDARG
    Error number: 4
    A logic error

  