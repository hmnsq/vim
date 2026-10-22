درآمد platform

اگر platform.system() == 'Darwin':
    سے ctypes درآمد (
        CDLL,
        اشارہ_گر_2,
        Structure,
        byref,
        c_int,
        c_uint,
        c_uint32,
        c_void_p,
        sizeof
    )
    سے ctypes.util درآمد ڈھونڈو_library

    جماعت Thread_وقت_Constraint_Policy(Structure):
        _fields_ = [
            ("period", c_uint32),
            ("computation", c_uint32),
            ("constraint", c_uint32),
            ("preemptible", c_uint)
        ]

    _libc = CDLL(ڈھونڈو_library('c'))

    THREAD_وقت_CONSTRAINT_POLICY = c_uint(2)

    THREAD_وقت_CONSTRAINT_POLICY_گنتی = c_uint(
        صحیح(sizeof(Thread_وقت_Constraint_Policy) / sizeof(c_int)))

    _libc.pthread_self.restype = c_void_p

    _libc.pthread_mach_thread_np.restype = c_uint
    _libc.pthread_mach_thread_np.argtypes = [c_void_p]

    _libc.thread_policy_get.restype = c_int
    _libc.thread_policy_get.argtypes = [
        c_uint,
        c_uint,
        c_void_p,
        اشارہ_گر_2(c_uint),
        اشارہ_گر_2(c_uint)
    ]

    _libc.thread_policy_سیٹ.restype = c_int
    _libc.thread_policy_سیٹ.argtypes = [
        c_uint,
        c_uint,
        c_void_p,
        c_uint
    ]

    تعریف _mach_thread_self():
        واپس _libc.pthread_mach_thread_np(_libc.pthread_self())

    تعریف get_وقت_constraint_policy(طےشدہ=جھوٹ):
        thread = _mach_thread_self()
        policy_info = Thread_وقت_Constraint_Policy()
        policy_infoCnt = THREAD_وقت_CONSTRAINT_POLICY_گنتی
        get_default = c_uint(طےشدہ)

        kret = _libc.thread_policy_get(
            thread,
            THREAD_وقت_CONSTRAINT_POLICY,
            byref(policy_info),
            byref(policy_infoCnt),
            byref(get_default))
        اگر kret != 0:
            واپس کچھ_نہیں
        واپس policy_info

    تعریف سیٹ_وقت_constraint_policy(policy_info):
        thread = _mach_thread_self()
        policy_infoCnt = THREAD_وقت_CONSTRAINT_POLICY_گنتی

        kret = _libc.thread_policy_سیٹ(
            thread,
            THREAD_وقت_CONSTRAINT_POLICY,
            byref(policy_info),
            policy_infoCnt)
        اگر kret != 0:
            اٹھاؤ OSError(kret)

    تعریف سیٹ_بلند_ترجیح():
        policy_info = get_وقت_constraint_policy(طےشدہ=سچ)
        اگر نہیں policy_info:
            واپس
        policy_info.preemptible = c_uint(جھوٹ)
        سیٹ_وقت_constraint_policy(policy_info)
