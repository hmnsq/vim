இறக்குமதி platform

எனில் platform.system() == 'Darwin':
    தஃ ctypes இறக்குமதி (
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
    தஃ ctypes.util இறக்குமதி find_library

    வகுப்பு ThreadTimeConstraintPolicy(Structure):
        _fields_ = [
            ("period", c_uint32),
            ("computation", c_uint32),
            ("constraint", c_uint32),
            ("preemptible", c_uint)
        ]

    _libc = CDLL(find_library('c'))

    THREAD_TIME_CONSTRAINT_POLICY = c_uint(2)

    THREAD_TIME_CONSTRAINT_POLICY_மொத்தம் = c_uint(
        முழு(sizeof(ThreadTimeConstraintPolicy) / sizeof(c_int)))

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
        திரும்பு _libc.pthread_mach_thread_np(_libc.pthread_self())

    def _get_time_constraint_policy(இயல்பு=பொய்):
        thread = _mach_thread_self()
        policy_info = ThreadTimeConstraintPolicy()
        policy_infoCnt = THREAD_TIME_CONSTRAINT_POLICY_மொத்தம்
        get_default = c_uint(இயல்பு)

        kret = _libc.thread_policy_get(
            thread,
            THREAD_TIME_CONSTRAINT_POLICY,
            byref(policy_info),
            byref(policy_infoCnt),
            byref(get_default))
        எனில் kret != 0:
            திரும்பு None
        திரும்பு policy_info

    def _set_time_constraint_policy(policy_info):
        thread = _mach_thread_self()
        policy_infoCnt = THREAD_TIME_CONSTRAINT_POLICY_மொத்தம்

        kret = _libc.thread_policy_set(
            thread,
            THREAD_TIME_CONSTRAINT_POLICY,
            byref(policy_info),
            policy_infoCnt)
        எனில் kret != 0:
            எழுப்பு OSError(kret)

    def set_உயர்வு_priority():
        policy_info = _get_time_constraint_policy(இயல்பு=உண்மை)
        எனில் இல்லை policy_info:
            திரும்பு
        policy_info.preemptible = c_uint(பொய்)
        _set_time_constraint_policy(policy_info)
