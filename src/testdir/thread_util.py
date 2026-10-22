ਆਯਾਤ platform

ਜੇ platform.system() == 'Darwin':
    ਤਃ ctypes ਆਯਾਤ (
        CDLL,
        ਸੂਚਕ_2,
        Structure,
        byref,
        c_int,
        c_uint,
        c_uint32,
        c_void_p,
        sizeof
    )
    ਤਃ ctypes.util ਆਯਾਤ ਲੱਭੋ_library

    ਵਰਗ Thread_ਸਮਾਂ_Constraint_Policy(Structure):
        _fields_ = [
            ("period", c_uint32),
            ("computation", c_uint32),
            ("constraint", c_uint32),
            ("preemptible", c_uint)
        ]

    _libc = CDLL(ਲੱਭੋ_library('c'))

    THREAD_ਸਮਾਂ_CONSTRAINT_POLICY = c_uint(2)

    THREAD_ਸਮਾਂ_CONSTRAINT_POLICY_ਗਿਣਤੀ = c_uint(
        ਪੂਰਨ(sizeof(Thread_ਸਮਾਂ_Constraint_Policy) / sizeof(c_int)))

    _libc.pthread_self.restype = c_void_p

    _libc.pthread_mach_thread_np.restype = c_uint
    _libc.pthread_mach_thread_np.argtypes = [c_void_p]

    _libc.thread_policy_get.restype = c_int
    _libc.thread_policy_get.argtypes = [
        c_uint,
        c_uint,
        c_void_p,
        ਸੂਚਕ_2(c_uint),
        ਸੂਚਕ_2(c_uint)
    ]

    _libc.thread_policy_ਸੈੱਟ.restype = c_int
    _libc.thread_policy_ਸੈੱਟ.argtypes = [
        c_uint,
        c_uint,
        c_void_p,
        c_uint
    ]

    ਪਰਿਭਾਸ਼ਾ _mach_thread_self():
        ਵਾਪਸ _libc.pthread_mach_thread_np(_libc.pthread_self())

    ਪਰਿਭਾਸ਼ਾ get_ਸਮਾਂ_constraint_policy(ਮੂਲ=ਝੂਠ):
        thread = _mach_thread_self()
        policy_info = Thread_ਸਮਾਂ_Constraint_Policy()
        policy_infoCnt = THREAD_ਸਮਾਂ_CONSTRAINT_POLICY_ਗਿਣਤੀ
        get_default = c_uint(ਮੂਲ)

        kret = _libc.thread_policy_get(
            thread,
            THREAD_ਸਮਾਂ_CONSTRAINT_POLICY,
            byref(policy_info),
            byref(policy_infoCnt),
            byref(get_default))
        ਜੇ kret != 0:
            ਵਾਪਸ ਕੁਝ_ਨਹੀਂ
        ਵਾਪਸ policy_info

    ਪਰਿਭਾਸ਼ਾ ਸੈੱਟ_ਸਮਾਂ_constraint_policy(policy_info):
        thread = _mach_thread_self()
        policy_infoCnt = THREAD_ਸਮਾਂ_CONSTRAINT_POLICY_ਗਿਣਤੀ

        kret = _libc.thread_policy_ਸੈੱਟ(
            thread,
            THREAD_ਸਮਾਂ_CONSTRAINT_POLICY,
            byref(policy_info),
            policy_infoCnt)
        ਜੇ kret != 0:
            ਉਠਾਓ OSError(kret)

    ਪਰਿਭਾਸ਼ਾ ਸੈੱਟ_ਉੱਚਾ_ਤਰਜੀਹ():
        policy_info = get_ਸਮਾਂ_constraint_policy(ਮੂਲ=ਸੱਚ)
        ਜੇ ਨਹੀਂ policy_info:
            ਵਾਪਸ
        policy_info.preemptible = c_uint(ਝੂਠ)
        ਸੈੱਟ_ਸਮਾਂ_constraint_policy(policy_info)
