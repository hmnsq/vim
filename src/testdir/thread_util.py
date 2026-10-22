استورد platform

إذا platform.system() == 'Darwin':
    من ctypes استورد (
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
    من ctypes.util استورد لبھو_library

    صنف Thread_ویلا_Constraint_Policy(Structure):
        _fields_ = [
            ("period", c_uint32),
            ("computation", c_uint32),
            ("constraint", c_uint32),
            ("preemptible", c_uint)
        ]

    _libc = CDLL(لبھو_library('c'))

    THREAD_ویلا_CONSTRAINT_POLICY = c_uint(2)

    THREAD_ویلا_CONSTRAINT_POLICY_گنتی = c_uint(
        صحيح(sizeof(Thread_ویلا_Constraint_Policy) / sizeof(c_int)))

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
        أرجع _libc.pthread_mach_thread_np(_libc.pthread_self())

    تعریف get_ویلا_constraint_policy(افتراضي=خطأ):
        thread = _mach_thread_self()
        policy_info = Thread_ویلا_Constraint_Policy()
        policy_infoCnt = THREAD_ویلا_CONSTRAINT_POLICY_گنتی
        get_default = c_uint(افتراضي)

        kret = _libc.thread_policy_get(
            thread,
            THREAD_ویلا_CONSTRAINT_POLICY,
            byref(policy_info),
            byref(policy_infoCnt),
            byref(get_default))
        إذا kret != 0:
            أرجع کجھ_نئیں
        أرجع policy_info

    تعریف سیٹ_ویلا_constraint_policy(policy_info):
        thread = _mach_thread_self()
        policy_infoCnt = THREAD_ویلا_CONSTRAINT_POLICY_گنتی

        kret = _libc.thread_policy_سیٹ(
            thread,
            THREAD_ویلا_CONSTRAINT_POLICY,
            byref(policy_info),
            policy_infoCnt)
        إذا kret != 0:
            ارفع OSError(kret)

    تعریف سیٹ_بلند_ترجیح():
        policy_info = get_ویلا_constraint_policy(افتراضي=سچ)
        إذا ليس policy_info:
            أرجع
        policy_info.preemptible = c_uint(خطأ)
        سیٹ_ویلا_constraint_policy(policy_info)
