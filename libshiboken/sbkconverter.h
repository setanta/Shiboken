/*
 * This file is part of the Shiboken Python Bindings Generator project.
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: PySide team <contact@pyside.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef SBK_CONVERTER_H
#define SBK_CONVERTER_H

#include <limits>
#include <Python.h>
#include "shibokenmacros.h"
#include "basewrapper.h"

extern "C"
{

/**
 *  SbkConverter is used to perform type conversions from C++
 *  to Python and vice-versa;.and it is also used for type checking.
 *  SbkConverter is a private structure that must be accessed
 *  using the functions provided by the converter API.
 */
struct SbkConverter;

/**
 *  Given a void pointer to a C++ object, this function must return
 *  the proper Python object. It may be either an existing wrapper
 *  for the C++ object, or a newly create one. Or even the Python
 *  equivalent of the C++ value passed in the argument.
 *
 *  C++ -> Python
 */
typedef PyObject* (*CppToPythonFunc)(const void*);

/**
 *  This function converts a Python object to a C++ value, it may be
 *  a pointer, value, class, container or primitive type, passed via
 *  a void pointer, that will be cast properly inside the function.
 *  This function is usually returned by an IsConvertibleToCppFunc
 *  function, or obtained knowing the type of the Python object input,
 *  thus it will not check the Python object type, and will expect
 *  the void pointer to be pointing to a proper variable.
 *
 *  Python object -> C++ value ?
 */
typedef void (*PythonToCppFunc)(PyObject*,void*);

/**
 *  Checks if the Python object passed in the argument is convertible to a
 *  C++ type defined inside the function, it returns the converter function
 *  that will transform a Python argument into a C++ value.
 *  It returns NULL if the Python object is not convertible to the C++ type
 *  that the function represents.
 *
 *  Python object -> C++ value ?
 */
typedef PythonToCppFunc (*IsConvertibleToCppFunc)(PyObject*);

} // extern "C"


namespace Shiboken {
namespace Conversions {

/**
 *  Creates a converter for a wrapper type.
 *  \param type                  A Shiboken.ObjectType that will receive the new converter.
 *  \param toCppPointerConvFunc  Function to retrieve the C++ pointer held by a Python wrapper.
 *  \param toCppPointerCheckFunc Check and return the retriever function of the C++ pointer held by a Python wrapper.
 *  \param pointerToPythonFunc   Function to convert a C++ object to a Python \p type wrapper, keeping their identity.
 *  \param copyToPythonFunc      Function to convert a C++ object to a Python \p type, copying the object.
 *  \returns                     The new converter referred by the wrapper \p type.
 */
LIBSHIBOKEN_API SbkConverter* createConverter(SbkObjectType* type,
                                              PythonToCppFunc toCppPointerConvFunc,
                                              IsConvertibleToCppFunc toCppPointerCheckFunc,
                                              CppToPythonFunc pointerToPythonFunc,
                                              CppToPythonFunc copyToPythonFunc = 0);

LIBSHIBOKEN_API void deleteConverter(SbkConverter* converter);

// TODO:WRITEDOCSTRING
LIBSHIBOKEN_API void addPythonToCppValueConversion(SbkConverter* converter,
                                                   PythonToCppFunc pythonToCppFunc,
                                                   IsConvertibleToCppFunc isConvertibleToCppFunc);
LIBSHIBOKEN_API void addPythonToCppValueConversion(SbkObjectType* type,
                                                   PythonToCppFunc pythonToCppFunc,
                                                   IsConvertibleToCppFunc isConvertibleToCppFunc);

// C++ -> Python ---------------------------------------------------------------------------

// TODO:WRITEDOCSTRING - used only for Object Types and Value Type pointers
LIBSHIBOKEN_API PyObject* pointerToPython(SbkObjectType* type, const void* cppIn);

// TODO:WRITEDOCSTRING - used only for Value Types (and Primitives and Containers) - cppIn must point to a value
LIBSHIBOKEN_API PyObject* copyToPython(SbkObjectType* type, const void* cppIn);

// TODO:WRITEDOCSTRING - used only for Value Types (and Primitives and Containers) - cppIn must point to a value
LIBSHIBOKEN_API PyObject* referenceToPython(SbkObjectType* type, const void* cppIn);

// Python -> C++ ---------------------------------------------------------------------------

// TODO:WRITEDOCSTRING
LIBSHIBOKEN_API PythonToCppFunc isPythonToCppPointerConvertible(SbkObjectType* type, PyObject* pyIn);

// TODO:WRITEDOCSTRING- Returns a Python to C++ conversion function if true, or NULL if false.
LIBSHIBOKEN_API PythonToCppFunc isPythonToCppValueConvertible(SbkObjectType* type, PyObject* pyIn);

// TODO:WRITEDOCSTRING- Returns a Python to C++ conversion function if true, or NULL if false.
LIBSHIBOKEN_API PythonToCppFunc isPythonToCppReferenceConvertible(SbkObjectType* type, PyObject* pyIn);

// TODO:WRITEDOCSTRING - function used by the generated [TYPE]_PythonToCpp_[TYPE]_PTR
LIBSHIBOKEN_API void pythonToCppPointer(SbkObjectType* type, PyObject* pyIn, void* cppOut);

// TODO:WRITEDOCSTRING - function used by the generated isConvertible when the PyObject is None,
// making a C++ NULL pointer the result of the toCpp function call.
// DRAFT: When the Python object is a Py_None, it's C++ conversion is always a NULL pointer.
LIBSHIBOKEN_API void nonePythonToCppNullPtr(PyObject*, void* cppOut);

// TODO:WRITEDOCSTRING - tells if \p toCpp is an implicit conversion.
LIBSHIBOKEN_API bool isImplicitConversion(SbkObjectType* type, PythonToCppFunc toCpp);

// Standard primitive conversions ----------------------------------------------------------

// TODO: consider adding a convenience function for converters without wrapper types.
//LIBSHIBOKEN_API void pythonToCppPointer(SbkConverter* type, PyObject* pyIn, void* cppOut);

// Basic primitive type converter.
template <typename T> struct Primitive {};

// Helper template for checking if a value overflows when cast to type T.
template<typename T, bool isSigned = std::numeric_limits<T>::is_signed >
struct OverFlowChecker;

template<typename T>
struct OverFlowChecker<T, true> {
    static bool check(const PY_LONG_LONG& value) {
        return value < std::numeric_limits<T>::min() || value > std::numeric_limits<T>::max();
    }
};
template<typename T>
struct OverFlowChecker<T, false> {
    static bool check(const PY_LONG_LONG& value) {
        return value < 0 || static_cast<unsigned long long>(value) > std::numeric_limits<T>::max();
    }
};
template<>
struct OverFlowChecker<PY_LONG_LONG, true> {
    static bool check(const PY_LONG_LONG& value) { return false; }
};
template<>
struct OverFlowChecker<double, true> {
    static bool check(const double& value) { return false; }
};
template<>
struct OverFlowChecker<float, true> {
    static bool check(const double& value) {
        return value < std::numeric_limits<float>::min() || value > std::numeric_limits<float>::max();
    }
};

// Integers --------------------------------------------------------------------------------

// Note: if there wasn't for the old-style classes, a simple PyNumber_Check would suffice.
#define SbkNumber_Check(X) (PyNumber_Check(X) \
        && (!PyInstance_Check(X) || PyObject_HasAttrString(X, "__trunc__")))

template <typename INT>
struct IntPrimitive
{
    static inline INT toCpp(PyObject* pyIn)
    {
        if (PyFloat_Check(pyIn)) {
            double result = PyFloat_AS_DOUBLE(pyIn);
            // If cast to long directly it could overflow silently.
            if (OverFlowChecker<INT>::check(result))
                PyErr_SetObject(PyExc_OverflowError, 0);
            return static_cast<INT>(result);
        }
        PY_LONG_LONG result = PyLong_AsLongLong(pyIn);
        if (OverFlowChecker<INT>::check(result))
            PyErr_SetObject(PyExc_OverflowError, 0);
        return static_cast<INT>(result);
    }
    static inline PyObject* toPython(const void* cppIn)
    {
        return PyInt_FromLong((long)*((INT*)cppIn));
    }
    static inline PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        return (SbkNumber_Check(pyIn)) ? convert : 0;
    }
protected:
    static void convert(PyObject* pyIn, void* cppOut) { *((INT*)cppOut) = toCpp(pyIn); }
};
template <> struct Primitive<int> : IntPrimitive<int> {};
template <> struct Primitive<long> : IntPrimitive<long> {};
template <> struct Primitive<short> : IntPrimitive<short> {};
template <> struct Primitive<unsigned short> : IntPrimitive<unsigned short> {};

// Unsigned Long Integers ------------------------------------------------------------------

template <typename LONG>
struct UnsignedLongPrimitive : IntPrimitive<LONG>
{
    static inline PyObject* toPython(const void* cppIn)
    {
        return PyLong_FromUnsignedLong(*((LONG*)cppIn));
    }
};
template <> struct Primitive<unsigned int> : UnsignedLongPrimitive<unsigned int> {};
template <> struct Primitive<unsigned long> : UnsignedLongPrimitive<unsigned long> {};

// Big integers ----------------------------------------------------------------------------

template <>
struct Primitive<PY_LONG_LONG>
{
    static inline PY_LONG_LONG toCpp(PyObject* pyIn)
    {
        return (PY_LONG_LONG) PyLong_AsLongLong(pyIn);
    }
    static inline PyObject* toPython(const void* cppIn)
    {
        return PyLong_FromLongLong(*((PY_LONG_LONG*)cppIn));
    }
    static inline PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        return (SbkNumber_Check(pyIn)) ? convert : 0;
    }
protected:
    static void convert(PyObject* pyIn, void* cppOut) { *((PY_LONG_LONG*)cppOut) = toCpp(pyIn); }
};

template <>
struct Primitive<unsigned PY_LONG_LONG>
{
    static inline unsigned PY_LONG_LONG toCpp(PyObject* pyIn)
    {
        return (unsigned PY_LONG_LONG) PyLong_AsUnsignedLongLong(pyIn);
    }
    static inline PyObject* toPython(const void* cppIn)
    {
        return PyLong_FromUnsignedLongLong(*((unsigned PY_LONG_LONG*)cppIn));
    }
    static inline PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        return (SbkNumber_Check(pyIn)) ? convert : 0;
    }
protected:
    static void convert(PyObject* pyIn, void* cppOut) { *((unsigned PY_LONG_LONG*)cppOut) = toCpp(pyIn); }
};

// Floating point --------------------------------------------------------------------------

template <typename FLOAT>
struct FloatPrimitive
{
    static inline FLOAT toCpp(PyObject* pyIn)
    {
        return (PyInt_Check(pyIn) || PyLong_Check(pyIn))
                ? (FLOAT) PyLong_AsLong(pyIn)
                : (FLOAT) PyFloat_AsDouble(pyIn);
    }
    static inline PyObject* toPython(const void* cppIn)
    {
        return PyFloat_FromDouble((double)*((FLOAT*)cppIn));
    }
    static inline PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        return (SbkNumber_Check(pyIn)) ? convert : 0;
    }
protected:
    static void convert(PyObject* pyIn, void* cppOut) { *((FLOAT*)cppOut) = toCpp(pyIn); }
};
template <> struct Primitive<float> : FloatPrimitive<float> {};
template <> struct Primitive<double> : FloatPrimitive<double> {};

// Boolean ---------------------------------------------------------------------------------

template <>
struct Primitive<bool>
{
    static inline bool toCpp(PyObject* pyIn)
    {
        // TODO-CONVERTER: this should check for universal True/False (e.g. empty list == False).
        return PyInt_AS_LONG(pyIn);
    }
    static inline PyObject* toPython(const void* cppIn)
    {
        return PyBool_FromLong(*((bool*)cppIn));
    }
    static inline PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        return (SbkNumber_Check(pyIn)) ? convert : 0;
    }
protected:
    static void convert(PyObject* pyIn, void* cppOut) { *((bool*)cppOut) = toCpp(pyIn); }
};

// Characters ------------------------------------------------------------------------------

#define SbkChar_Check(pyobj) (SbkNumber_Check(pyobj) || (PyString_Check(pyobj) && PyString_Size(pyobj) == 1))

template <typename CHAR>
struct CharPrimitive : IntPrimitive<CHAR>
{
    static inline CHAR toCpp(PyObject* pyIn)
    {
        if (PyString_Check(pyIn)) {
            assert(PyString_Size(pyIn) == 1); // TODO: REVIEW THIS COMMENT -> This check is made on SbkChar_Check
            return PyString_AS_STRING(pyIn)[0];
        }
        PY_LONG_LONG result = PyLong_AsLongLong(pyIn);
        if (OverFlowChecker<CHAR>::check(result))
            PyErr_SetObject(PyExc_OverflowError, 0);
        return result;
    }
    static inline PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        return (SbkChar_Check(pyIn)) ? convert : 0;
    }
protected:
    static void convert(PyObject* pyIn, void* cppOut) { *((CHAR*)cppOut) = toCpp(pyIn); }
};
template <> struct Primitive<char> : CharPrimitive<char> {};
template <> struct Primitive<signed char> : CharPrimitive<signed char> {};
template <> struct Primitive<unsigned char> : CharPrimitive<unsigned char> {};

// Strings ---------------------------------------------------------------------------------

template <>
struct Primitive<const char*>
{
    static inline const char* toCpp(PyObject* pyIn)
    {
        return (pyIn == Py_None) ? 0 : PyString_AsString(pyIn);
    }
    static inline PyObject* toPython(const void* cppIn)
    {
        if (!cppIn)
            Py_RETURN_NONE;
        return PyString_FromString((const char*)cppIn);
    }
    static inline PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        return (pyIn == Py_None || PyString_Check(pyIn)) ? convert : 0;
    }
protected:
    static void convert(PyObject* pyIn, void* cppOut) { *((const char**)cppOut) = toCpp(pyIn); }
};

template <>
struct Primitive<std::string>
{
    static inline std::string toCpp(PyObject* pyIn)
    {
        return (pyIn == Py_None) ? 0 : std::string(PyString_AsString(pyIn));
    }
    static inline PyObject* toPython(const void* cppIn)
    {
        return PyString_FromString(((std::string*)cppIn)->c_str());
    }
    static inline PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        return (pyIn == Py_None || PyString_Check(pyIn)) ? convert : 0;
    }
protected:
    static void convert(PyObject* pyIn, void* cppOut) { *((std::string*)cppOut) = toCpp(pyIn); }
};

// Void pointer ----------------------------------------------------------------------------

template <>
struct Primitive<void*>
{
    static inline void* toCpp(PyObject* pyIn)
    {
        return pyIn;
    }
    static inline PyObject* toPython(const void* cppIn)
    {
        if (!cppIn)
            Py_RETURN_NONE;
        PyObject* result = (PyObject*) cppIn;
        Py_INCREF(result);
        return result;
    }
    static inline PythonToCppFunc isConvertible(PyObject* pyIn)
    {
        return convert;
    }
protected:
    static void convert(PyObject* pyIn, void* cppOut) { *((void**)cppOut) = toCpp(pyIn); }
};

} } // namespace Shiboken::Conversions

#endif // SBK_CONVERTER_H
