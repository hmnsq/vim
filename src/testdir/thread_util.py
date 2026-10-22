استورد platform

إذا platform.system() == 'Darwin':
    من ctypes استورد (
        CDLL,
        اشاره_گر_2,
        Structure,
        byref,
        c_int,
        c_uint,
        c_uint32,
        c_void_p,
        sizeof
    )
    من ctypes.util استورد پیدا_کن_library

    صنف Thread_زمان_Constraint_Policy(Structure):
        _fields_ = [
            ("period", c_uint32),
            ("computation", c_uint32),
            ("constraint", c_uint32),
            ("preemptible", c_uint)
        ]

    _libc = CDLL(پیدا_کن_library('c'))

    THREAD_زمان_CONSTRAINT_POLICY = c_uint(2)

    THREAD_زمان_CONSTRAINT_POLICY_شمار = c_uint(
        صحيح(sizeof(Thread_زمان_Constraint_Policy) / sizeof(c_int)))

    _libc.pthread_self.restype = c_void_p

    _libc.pthread_mach_thread_np.restype = c_uint
    _libc.pthread_mach_thread_np.argtypes = [c_void_p]

    _libc.thread_policy_get.restype = c_int
    _libc.thread_policy_get.argtypes = [
        c_uint,
        c_uint,
        c_void_p,
        اشاره_گر_2(c_uint),
        اشاره_گر_2(c_uint)
    ]

    _libc.thread_policy_مجموعه.restype = c_int
    _libc.thread_policy_مجموعه.argtypes = [
        c_uint,
        c_uint,
        c_void_p,
        c_uint
    ]

    تعریف _mach_thread_self():
        أرجع _libc.pthread_mach_thread_np(_libc.pthread_self())

    تعریف get_زمان_constraint_policy(افتراضي=خطأ):
        thread = _mach_thread_self()
        policy_info = Thread_زمان_Constraint_Policy()
        policy_infoCnt = THREAD_زمان_CONSTRAINT_POLICY_شمار
        get_default = c_uint(افتراضي)

        kret = _libc.thread_policy_get(
            thread,
            THREAD_زمان_CONSTRAINT_POLICY,
            byref(policy_info),
            byref(policy_infoCnt),
            byref(get_default))
        إذا kret != 0:
            أرجع هیچ
        أرجع policy_info

    تعریف مجموعه_زمان_constraint_policy(policy_info):
        thread = _mach_thread_self()
        policy_infoCnt = THREAD_زمان_CONSTRAINT_POLICY_شمار

        kret = _libc.thread_policy_مجموعه(
            thread,
            THREAD_زمان_CONSTRAINT_POLICY,
            byref(policy_info),
            policy_infoCnt)
        إذا kret != 0:
            ارفع OSError(kret)

    تعریف مجموعه_بالا_اولویت():
        policy_info = get_زمان_constraint_policy(افتراضي=درست)
        إذا ليس policy_info:
            أرجع
        policy_info.preemptible = c_uint(خطأ)
        مجموعه_زمان_constraint_policy(policy_info)
