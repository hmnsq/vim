استورد platform

إذا platform.system() == 'Darwin':
    من ctypes استورد (
        CDLL,
        اشارو_2,
        Structure,
        byref,
        c_int,
        c_uint,
        c_uint32,
        c_void_p,
        sizeof
    )
    من ctypes.util استورد ڳوليو_library

    صنف Thread_وقت_Constraint_Policy(Structure):
        _fields_ = [
            ("period", c_uint32),
            ("computation", c_uint32),
            ("constraint", c_uint32),
            ("preemptible", c_uint)
        ]

    _libc = CDLL(ڳوليو_library('c'))

    THREAD_وقت_CONSTRAINT_POLICY = c_uint(2)

    THREAD_وقت_CONSTRAINT_POLICY_ڳڻپ = c_uint(
        صحيح(sizeof(Thread_وقت_Constraint_Policy) / sizeof(c_int)))

    _libc.pthread_self.restype = c_void_p

    _libc.pthread_mach_thread_np.restype = c_uint
    _libc.pthread_mach_thread_np.argtypes = [c_void_p]

    _libc.thread_policy_get.restype = c_int
    _libc.thread_policy_get.argtypes = [
        c_uint,
        c_uint,
        c_void_p,
        اشارو_2(c_uint),
        اشارو_2(c_uint)
    ]

    _libc.thread_policy_سيٽ.restype = c_int
    _libc.thread_policy_سيٽ.argtypes = [
        c_uint,
        c_uint,
        c_void_p,
        c_uint
    ]

    وصف _mach_thread_self():
        أرجع _libc.pthread_mach_thread_np(_libc.pthread_self())

    وصف get_وقت_constraint_policy(افتراضي=خطأ):
        thread = _mach_thread_self()
        policy_info = Thread_وقت_Constraint_Policy()
        policy_infoCnt = THREAD_وقت_CONSTRAINT_POLICY_ڳڻپ
        get_default = c_uint(افتراضي)

        kret = _libc.thread_policy_get(
            thread,
            THREAD_وقت_CONSTRAINT_POLICY,
            byref(policy_info),
            byref(policy_infoCnt),
            byref(get_default))
        إذا kret != 0:
            أرجع ڪجهه_نه
        أرجع policy_info

    وصف سيٽ_وقت_constraint_policy(policy_info):
        thread = _mach_thread_self()
        policy_infoCnt = THREAD_وقت_CONSTRAINT_POLICY_ڳڻپ

        kret = _libc.thread_policy_سيٽ(
            thread,
            THREAD_وقت_CONSTRAINT_POLICY,
            byref(policy_info),
            policy_infoCnt)
        إذا kret != 0:
            ارفع OSError(kret)

    وصف سيٽ_مٿاهون_ترجيح():
        policy_info = get_وقت_constraint_policy(افتراضي=سچ)
        إذا ليس policy_info:
            أرجع
        policy_info.preemptible = c_uint(خطأ)
        سيٽ_وقت_constraint_policy(policy_info)
