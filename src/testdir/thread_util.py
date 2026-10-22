આયાત platform

જો platform.system() == 'Darwin':
    તઃ ctypes આયાત (
        CDLL,
        POINTER,
        Structure,
        byref,
        c_int,
        c_uint,
        c_uint32,
        c_void_p,
        sizeof
    )
    તઃ ctypes.util આયાત find_library

    વર્ગ ThreadTimeConstraintPolicy(Structure):
        _fields_ = [
            ("period", c_uint32),
            ("computation", c_uint32),
            ("constraint", c_uint32),
            ("preemptible", c_uint)
        ]

    _libc = CDLL(find_library('c'))

    THREAD_TIME_CONSTRAINT_POLICY = c_uint(2)

    THREAD_TIME_CONSTRAINT_POLICY_ગણતરી = c_uint(
        પૂર્ણાંક(sizeof(ThreadTimeConstraintPolicy) / sizeof(c_int)))

    _libc.pthread_self.restype = c_void_p

    _libc.pthread_mach_thread_np.restype = c_uint
    _libc.pthread_mach_thread_np.argtypes = [c_void_p]

    _libc.thread_policy_get.restype = c_int
    _libc.thread_policy_get.argtypes = [
        c_uint,
        c_uint,
        c_void_p,
        POINTER(c_uint),
        POINTER(c_uint)
    ]

    _libc.thread_policy_set.restype = c_int
    _libc.thread_policy_set.argtypes = [
        c_uint,
        c_uint,
        c_void_p,
        c_uint
    ]

    def _mach_thread_self():
        પાછા _libc.pthread_mach_thread_np(_libc.pthread_self())

    def _get_time_constraint_policy(મૂળભૂત=ખોટું):
        thread = _mach_thread_self()
        policy_info = ThreadTimeConstraintPolicy()
        policy_infoCnt = THREAD_TIME_CONSTRAINT_POLICY_ગણતરી
        get_default = c_uint(મૂળભૂત)

        kret = _libc.thread_policy_get(
            thread,
            THREAD_TIME_CONSTRAINT_POLICY,
            byref(policy_info),
            byref(policy_infoCnt),
            byref(get_default))
        જો kret != 0:
            પાછા None
        પાછા policy_info

    def _set_time_constraint_policy(policy_info):
        thread = _mach_thread_self()
        policy_infoCnt = THREAD_TIME_CONSTRAINT_POLICY_ગણતરી

        kret = _libc.thread_policy_set(
            thread,
            THREAD_TIME_CONSTRAINT_POLICY,
            byref(policy_info),
            policy_infoCnt)
        જો kret != 0:
            ઉઠાવો OSError(kret)

    def set_ઉપલું_priority():
        policy_info = _get_time_constraint_policy(મૂળભૂત=સાચું)
        જો નહીં policy_info:
            પાછા
        policy_info.preemptible = c_uint(ખોટું)
        _set_time_constraint_policy(policy_info)
