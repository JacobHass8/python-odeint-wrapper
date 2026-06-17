from libcpp.vector cimport vector

cdef extern from "pyodeint.h":
    ctypedef vector[double] (*py_derv_func)(vector[double] vec, void *user_data)
    
    cdef cppclass ResultCpp:
        vector[vector[double]] m_p
        vector[vector[double]] m_q 
        vector[double] m_times 

        Result(vector[vector[double]], vector[vector[double]], vector[double]) except +

    ResultCpp* solve_ivp_symplectic(double dt, 
                                double t0, 
                                double tn, 
                                vector[double] q0, 
                                vector[double] p0, 
                                py_derv_func py_q2dp, 
                                void* user_data,
                                py_derv_func py_p2dq,
                                void* user_data2)

cdef class PyMyCppClass:
    cdef ResultCpp* thisptr  # Hold a C++ instance pointer
    cdef bint is_owner

    def __cinit__(self):
        self.thisptr = NULL
        self.is_owner = False

    def __dealloc__(self):
        if self.thisptr != NULL and self.is_owner:
            del self.thisptr

    @property 
    def p(self):
        return self.thisptr.m_p
    
    @property
    def q(self):
        return self.thisptr.m_q
    
    @property
    def times(self):
        return self.thisptr.m_times

cdef PyMyClass_from_ptr(ResultCpp* ptr, bint owner=True):
    if ptr == NULL:
        return None
    cdef PyMyCppClass wrapper = PyMyCppClass.__new__(PyMyCppClass)
    wrapper.thisptr = ptr
    wrapper.is_owner = owner
    return wrapper

def py_solve(dt, t0, tn, q0, p0, f, g):
    cdef ResultCpp* raw_ptr = solve_ivp_symplectic(dt, t0, tn, q0, p0, vector_callback, <void*>f, vector_callback, <void*>g)
    return PyMyClass_from_ptr(raw_ptr, owner=True)

cdef vector[double] vector_callback(vector[double] vec, void *f) noexcept:
    return (<object>f)(vec)