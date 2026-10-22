దిగుమతి platform

ఒకవేళ platform.system() == 'Darwin':
    తః ctypes దిగుమతి (
        CDLL,
        సూచిక_2,
        Structure,
        byref,
        c_int,
        c_uint,
        c_uint32,
        c_void_p,
        sizeof
    )
    తః ctypes.util దిగుమతి కనుగొను_library

    తరగతి Thread_సమయం_Constraint_Policy(Structure):
        _fields_ = [
            ("period", c_uint32),
            ("computation", c_uint32),
            ("constraint", c_uint32),
            ("preemptible", c_uint)
        ]

    _libc = CDLL(కనుగొను_library('c'))

    THREAD_సమయం_CONSTRAINT_POLICY = c_uint(2)

    THREAD_సమయం_CONSTRAINT_POLICY_లెక్క = c_uint(
        పూర్ణాంకం(sizeof(Thread_సమయం_Constraint_Policy) / sizeof(c_int)))

    _libc.pthread_self.restype = c_void_p

    _libc.pthread_mach_thread_np.restype = c_uint
    _libc.pthread_mach_thread_np.argtypes = [c_void_p]

    _libc.thread_policy_get.restype = c_int
    _libc.thread_policy_get.argtypes = [
        c_uint,
        c_uint,
        c_void_p,
        సూచిక_2(c_uint),
        సూచిక_2(c_uint)
    ]

    _libc.thread_policy_సమితి.restype = c_int
    _libc.thread_policy_సమితి.argtypes = [
        c_uint,
        c_uint,
        c_void_p,
        c_uint
    ]

    నిర్వచనం _mach_thread_self():
        తిరిగి _libc.pthread_mach_thread_np(_libc.pthread_self())

    నిర్వచనం get_సమయం_constraint_policy(అప్రమేయ=అబద్ధం):
        thread = _mach_thread_self()
        policy_info = Thread_సమయం_Constraint_Policy()
        policy_infoCnt = THREAD_సమయం_CONSTRAINT_POLICY_లెక్క
        get_default = c_uint(అప్రమేయ)

        kret = _libc.thread_policy_get(
            thread,
            THREAD_సమయం_CONSTRAINT_POLICY,
            byref(policy_info),
            byref(policy_infoCnt),
            byref(get_default))
        ఒకవేళ kret != 0:
            తిరిగి ఏమీలేదు
        తిరిగి policy_info

    నిర్వచనం సమితి_సమయం_constraint_policy(policy_info):
        thread = _mach_thread_self()
        policy_infoCnt = THREAD_సమయం_CONSTRAINT_POLICY_లెక్క

        kret = _libc.thread_policy_సమితి(
            thread,
            THREAD_సమయం_CONSTRAINT_POLICY,
            byref(policy_info),
            policy_infoCnt)
        ఒకవేళ kret != 0:
            లేపు OSError(kret)

    నిర్వచనం సమితి_ఎగువ_ప్రాధాన్యత():
        policy_info = get_సమయం_constraint_policy(అప్రమేయ=నిజం)
        ఒకవేళ కాదు policy_info:
            తిరిగి
        policy_info.preemptible = c_uint(అబద్ధం)
        సమితి_సమయం_constraint_policy(policy_info)
