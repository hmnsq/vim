ଆମଦାନୀ platform

ଯଦି platform.system() == 'Darwin':
    ତଃ ctypes ଆମଦାନୀ (
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
    ତଃ ctypes.util ଆମଦାନୀ find_library

    ଶ୍ରେଣୀ ThreadTimeConstraintPolicy(Structure):
        _fields_ = [
            ("period", c_uint32),
            ("computation", c_uint32),
            ("constraint", c_uint32),
            ("preemptible", c_uint)
        ]

    _libc = CDLL(find_library('c'))

    THREAD_TIME_CONSTRAINT_POLICY = c_uint(2)

    THREAD_TIME_CONSTRAINT_POLICY_ଗଣନା = c_uint(
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା(sizeof(ThreadTimeConstraintPolicy) / sizeof(c_int)))

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
        ଫେରନ୍ତୁ _libc.pthread_mach_thread_np(_libc.pthread_self())

    def _get_time_constraint_policy(ପୂର୍ବନିର୍ଦ୍ଧାରିତ=ମିଥ୍ୟା):
        thread = _mach_thread_self()
        policy_info = ThreadTimeConstraintPolicy()
        policy_infoCnt = THREAD_TIME_CONSTRAINT_POLICY_ଗଣନା
        get_default = c_uint(ପୂର୍ବନିର୍ଦ୍ଧାରିତ)

        kret = _libc.thread_policy_get(
            thread,
            THREAD_TIME_CONSTRAINT_POLICY,
            byref(policy_info),
            byref(policy_infoCnt),
            byref(get_default))
        ଯଦି kret != 0:
            ଫେରନ୍ତୁ None
        ଫେରନ୍ତୁ policy_info

    def _set_time_constraint_policy(policy_info):
        thread = _mach_thread_self()
        policy_infoCnt = THREAD_TIME_CONSTRAINT_POLICY_ଗଣନା

        kret = _libc.thread_policy_set(
            thread,
            THREAD_TIME_CONSTRAINT_POLICY,
            byref(policy_info),
            policy_infoCnt)
        ଯଦି kret != 0:
            ଉଠାଅ OSError(kret)

    def set_ଉପର_priority():
        policy_info = _get_time_constraint_policy(ପୂର୍ବନିର୍ଦ୍ଧାରିତ=ସତ୍ୟ)
        ଯଦି ନୁହେଁ policy_info:
            ଫେରନ୍ତୁ
        policy_info.preemptible = c_uint(ମିଥ୍ୟା)
        _set_time_constraint_policy(policy_info)
