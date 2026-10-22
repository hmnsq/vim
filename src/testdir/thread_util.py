ಆಮದು platform

ಆದರೆ platform.system() == 'Darwin':
    ತಃ ctypes ಆಮದು (
        CDLL,
        ಸೂಚಕ_2,
        Structure,
        byref,
        c_int,
        c_uint,
        c_uint32,
        c_void_p,
        sizeof
    )
    ತಃ ctypes.util ಆಮದು ಕಂಡುಹಿಡಿ_library

    ವರ್ಗ Thread_ಸಮಯ_Constraint_Policy(Structure):
        _fields_ = [
            ("period", c_uint32),
            ("computation", c_uint32),
            ("constraint", c_uint32),
            ("preemptible", c_uint)
        ]

    _libc = CDLL(ಕಂಡುಹಿಡಿ_library('c'))

    THREAD_ಸಮಯ_CONSTRAINT_POLICY = c_uint(2)

    THREAD_ಸಮಯ_CONSTRAINT_POLICY_ಎಣಿಕೆ = c_uint(
        ಪೂರ್ಣಾಂಕ(sizeof(Thread_ಸಮಯ_Constraint_Policy) / sizeof(c_int)))

    _libc.pthread_self.restype = c_void_p

    _libc.pthread_mach_thread_np.restype = c_uint
    _libc.pthread_mach_thread_np.argtypes = [c_void_p]

    _libc.thread_policy_get.restype = c_int
    _libc.thread_policy_get.argtypes = [
        c_uint,
        c_uint,
        c_void_p,
        ಸೂಚಕ_2(c_uint),
        ಸೂಚಕ_2(c_uint)
    ]

    _libc.thread_policy_ಗಣ.restype = c_int
    _libc.thread_policy_ಗಣ.argtypes = [
        c_uint,
        c_uint,
        c_void_p,
        c_uint
    ]

    ವ್ಯಾಖ್ಯಾನ _mach_thread_self():
        ಹಿಂತಿರುಗಿ _libc.pthread_mach_thread_np(_libc.pthread_self())

    ವ್ಯಾಖ್ಯಾನ get_ಸಮಯ_constraint_policy(ಪೂರ್ವನಿಯೋಜಿತ=ಸುಳ್ಳು):
        thread = _mach_thread_self()
        policy_info = Thread_ಸಮಯ_Constraint_Policy()
        policy_infoCnt = THREAD_ಸಮಯ_CONSTRAINT_POLICY_ಎಣಿಕೆ
        get_default = c_uint(ಪೂರ್ವನಿಯೋಜಿತ)

        kret = _libc.thread_policy_get(
            thread,
            THREAD_ಸಮಯ_CONSTRAINT_POLICY,
            byref(policy_info),
            byref(policy_infoCnt),
            byref(get_default))
        ಆದರೆ kret != 0:
            ಹಿಂತಿರುಗಿ ಏನೂಇಲ್ಲ
        ಹಿಂತಿರುಗಿ policy_info

    ವ್ಯಾಖ್ಯಾನ ಗಣ_ಸಮಯ_constraint_policy(policy_info):
        thread = _mach_thread_self()
        policy_infoCnt = THREAD_ಸಮಯ_CONSTRAINT_POLICY_ಎಣಿಕೆ

        kret = _libc.thread_policy_ಗಣ(
            thread,
            THREAD_ಸಮಯ_CONSTRAINT_POLICY,
            byref(policy_info),
            policy_infoCnt)
        ಆದರೆ kret != 0:
            ಎತ್ತು OSError(kret)

    ವ್ಯಾಖ್ಯಾನ ಗಣ_ಮೇಲಿನ_ಆದ್ಯತೆ():
        policy_info = get_ಸಮಯ_constraint_policy(ಪೂರ್ವನಿಯೋಜಿತ=ನಿಜ)
        ಆದರೆ ಅಲ್ಲ policy_info:
            ಹಿಂತಿರುಗಿ
        policy_info.preemptible = c_uint(ಸುಳ್ಳು)
        ಗಣ_ಸಮಯ_constraint_policy(policy_info)
