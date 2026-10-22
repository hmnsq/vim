আমদানি platform

যদি platform.system() == 'Darwin':
    তঃ ctypes আমদানি (
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
    তঃ ctypes.util আমদানি find_library

    শ্রেণী ThreadTimeConstraintPolicy(Structure):
        _fields_ = [
            ("period", c_uint32),
            ("computation", c_uint32),
            ("constraint", c_uint32),
            ("preemptible", c_uint)
        ]

    _libc = CDLL(find_library('c'))

    THREAD_TIME_CONSTRAINT_POLICY = c_uint(2)

    THREAD_TIME_CONSTRAINT_POLICY_গুনতি = c_uint(
        পূর্ণসংখ্যা(sizeof(ThreadTimeConstraintPolicy) / sizeof(c_int)))

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
        ফেরত _libc.pthread_mach_thread_np(_libc.pthread_self())

    def _get_time_constraint_policy(পূর্বনির্ধারিত=মিথ্যা):
        thread = _mach_thread_self()
        policy_info = ThreadTimeConstraintPolicy()
        policy_infoCnt = THREAD_TIME_CONSTRAINT_POLICY_গুনতি
        get_default = c_uint(পূর্বনির্ধারিত)

        kret = _libc.thread_policy_get(
            thread,
            THREAD_TIME_CONSTRAINT_POLICY,
            byref(policy_info),
            byref(policy_infoCnt),
            byref(get_default))
        যদি kret != 0:
            ফেরত None
        ফেরত policy_info

    def _set_time_constraint_policy(policy_info):
        thread = _mach_thread_self()
        policy_infoCnt = THREAD_TIME_CONSTRAINT_POLICY_গুনতি

        kret = _libc.thread_policy_set(
            thread,
            THREAD_TIME_CONSTRAINT_POLICY,
            byref(policy_info),
            policy_infoCnt)
        যদি kret != 0:
            তোলো OSError(kret)

    def set_উপরের_priority():
        policy_info = _get_time_constraint_policy(পূর্বনির্ধারিত=সত্য)
        যদি নয় policy_info:
            ফেরত
        policy_info.preemptible = c_uint(মিথ্যা)
        _set_time_constraint_policy(policy_info)
