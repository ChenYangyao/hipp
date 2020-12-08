Error Handling
================================

The following classes are all defined within namespace ``HIPP::IO``.

.. namespace:: HIPP::IO 

Class ErrH5: the Exception Handler 
-------------------------------------

.. class::  ErrH5: public HIPP::ErrAppH5, public HIPP::ErrClassDefault

    .. type::   herr_t errno_t
                int flag_t
                ErrAppH5 err_app_t
                ErrClassDefault err_class_t

        ``errno_t`` is the type of the error number hosted by :class:`ErrH5`.
        ``flat_t`` is the type of the static flag attribute which controls the 
        behavior of :class:`ErrH5`.
        ``err_app_t``, ``err_class_t`` are the parent classes which define the 
        structure of the exception model.

    .. function::   ErrH5( errno_t new_errno = -1 ) noexcept

        The constructor. 
        
        By default, HDF5 oringinal library does not code the error by a single 
        number, so the ``new_errno`` is of no meaning.


    .. function::  virtual const char *what() const noexcept override
                    virtual string whats() const
        
        These two functions report the detail of the exception instance.
        ``what()`` only roughly indicates the error at application level,
        but ``whats()`` returns more detailed information, including the info 
        associated with thrown error number.

    .. function::   errno_t get_errno() const noexcept
                    void set_errno( errno_t new_errno ) noexcept

        ``get_errno()`` returns the error number in the current instance.
        
        ``set_errno`` set the error number.

    
    
    .. function::   template<typename ReturnType, typename ...Args>\
                        static void check( ReturnType r, Args &&... args )
                    template<typename ReturnType, typename ...Args>\
                        static void throw_( ReturnType r, Args &&... args )

        Convinient function for detect, report errors and throw exceptions.

        ``check(r, ...args)`` tests if ``r < 0``. If so, call ``throw_(r, args)``. 
        Otherwise nothing happens.
        
        ``throw_(r, ...args)`` printing some information ``args`` and always 
        throws an ``ErrH5()``. 
        The printing can be turned on/off by setting the controlling flag to 1/0.
   
    .. function::   static flag_t err_cntl_flag() noexcept
                    static void err_cntl_flag( flag_t flag ) noexcept

        The error-reporing controlling flag of the HIPP HDF5 API.
        If set to 0, no error is printed on an exception. Otherwise some
        information is printed to ``std::cerr`` before an thrown exception.
        
        ``err_cntl_flag()`` retrive the current flag, ``err_cntl_flag(flag)`` 
        set the flag.