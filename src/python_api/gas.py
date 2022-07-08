import ctypes

lib = ctypes.cdll.LoadLibrary('bin/lib/libgas.so')

class GasModel:
    def __init__(self):
        self._gm = lib.GasModel_new()

class GasState:
    def __init__(self):
        self._gs = lib.GasState_new()

    @property
    def p(self):
        return lib.GasState_get_pressure(self._gs)

    @p.setter
    def p(self, p):
        lib.GasState_set_pressure(self._gs, p)

lib.GasState_new.restype = ctypes.c_void_p
lib.GasState_new.argtype = ctypes.c_void_p
lib.GasState_set_pressure.argtypes = (ctypes.c_void_p, ctypes.c_double)
lib.GasState_set_pressure.restype = ctypes.c_void_p
lib.GasState_get_pressure.argtype = ctypes.c_void_p
lib.GasState_get_pressure.restype = ctypes.c_double

gs = GasState()
gs.p = 101.0
gs.p
