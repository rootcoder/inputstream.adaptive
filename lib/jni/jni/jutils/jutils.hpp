/*
 *  Copyright (C) 2011-2012 Dmitry Moskalchuk <dm@crystax.net>
 *  Copyright (C) 2013 Team Kodi
 *  This file is part of Kodi - https://kodi.tv
 *
 *  SPDX-License-Identifier: BSD-2-Clause-Views WITH GPL-2.0-or-later
 *  See LICENSES/README.md for more information.
 */

#pragma once

#define HAVE_INTTYPES_H 1

#include <jni.h>
#include <string>
#include <android/log.h>
#include <vector>

JNIEnv *xbmc_jnienv();

namespace jni
{

template <typename T>
class jholder
{
/* A native jni types wrapper class.

  This templated class is used as a container for native jni types: jobject,
  jclass, etc. It maintains scope and references so that parent objects don't
  have to bother.

  Here, a jobject will be used as an example.

  Background: JNI uses reference-counted objects to facilitate
  garbage-collection. A jobject is really just a pointer to some shared memory.
  When a jobject is given to native code, it has a local
  reference. When this local reference is removed, the JVM is free to
  garbage-collect its contents. Local references are destroyed when the jobject
  is destroyed (or loses scope) or when Java execution resumes, or they can be
  destroyed manually. Objects which hold local references also cannot be shared
  between threads. To get around these limitations, the local reference can be
  upgraded to a global one.

  This class handles this logic for the user. A jobject can be moved into a
  jhobject via the jhobject constructor. After doing so, the original jobject's
  state is undefined because the jhobject will unref it automatically.

  Scope of copies is handled as well, so no extra care needs to be taken. When
  a copy is made, the copy inherits the scope of the original object. The
  class tries to operate in the safest possible way when assigning objects of
  differing scope by choosing the widest possible scope.

  Example:

  jhobject somefunction(jobject androidObject)
  {
    jhobject foo(androidObject);
    // store androidObject in foo. androidObject should not be used again.
    // foo has a local ref.

    jhobject bar(foo);
    // copy foo to bar. foo and bar are both local. They can both be used
    // on this thread until returning to java. It would not be safe to return
    // them because the caller cannot be trusted to obey the local-ref rules.
    //
    // Note: This copy makes no practical sense, it's only used for
    // demonstrating scope copies. This is effectively making a copy of a
    // pointer.

    bar.setGlobal();
    // Now foo has a local ref and bar has a global ref.

    foo = bar;
    // foo's local ref is destroyed. it's value is set to bar's and the widest
    // possible scope is chosen. foo and bar now both have global refs.

    return foo;
  } // bar's global is unref'd and it is destroyed. global foo is returned.

*/

typedef void (jholder::*safe_bool_type)();
void non_null_object() {}

public:

/*! \brief constructor for jholder().
   Object is null, type is invalid
*/
  jholder()
  :m_refType(JNIInvalidRefType)
  ,m_object(0)
  {
  }

/*! \brief jholder copy constructor.
   Object is copied, scope matches the original
*/
  jholder(jholder<T> const &c)
  :m_refType(JNIInvalidRefType)
  ,m_object(c.get())
  {
    setscope(c.m_refType);
  }

/*! \brief jholder assignment operator
   Object takes on the widest scope of the two, local at a minimum
*/
  jholder &operator=(jholder const &c)
  {
    jobjectRefType newtype = JNILocalRefType;
    if(c.m_refType == JNIGlobalRefType || m_refType == JNIGlobalRefType)
      newtype = JNIGlobalRefType;

    reset(c.get());
    setscope(newtype);
    return *this;
  }

/*! \brief jholder constructor from a native jni object
   Incoming objects already hold a local ref.
*/
  explicit jholder(const T& obj)
  :m_refType(JNILocalRefType)
  ,m_object(obj)
  {
    setscope(JNILocalRefType);
  }

/*! \brief jholder static unref'ed constructor.
     Object is copied, scope is invalid
*/
  static jholder<T> fromJNI(const T& obj)
  {
    jholder<T> o;
    o.m_refType = JNIInvalidRefType;
    o.m_object = obj;
    return o;
  }

/*! \brief jholder dtor.
   Held objects are deref'd and destroyed.
*/
  ~jholder()
  {
    reset();
  }

/*! \brief cast operator for native types
   Grr, no explicit operator casts without c++11.
   This enables automatic down-casting to native types.
*/
  operator const T&() const
  {
    return get();
  }

/*! \brief get native type
   Same as the above, mainly for internal usage for readability and explicitness
*/
  const T& get() const
  {
    return m_object;
  }

/*! \brief set an object to global scope. Has no effect if it's already global
*/
  void setGlobal()
  {
    setscope(JNIGlobalRefType);
  }

/*! \brief not operator.
     queries whether the jholder contains a valid object
*/
  bool operator!() const {return !m_object;}
  operator safe_bool_type () const
  {
    return m_object ? &jholder::non_null_object : 0;
  }

  /*! \brief equal operator.
       test whether two references points to the same instance
  */
  bool operator==(const T &other) const
  {
    return m_object ? xbmc_jnienv()->IsSameObject(m_object, other) : false;
  }

/*! \brief Change the internal object
  Unref the original object. The new ref type is invalid.

  This should almost never be used outside of this class. Use it only to
  maintain static objects that should never be unref'd, such as
  nativeActivity->clazz.
  Repeat: Usage of reset() is almost definitely wrong!
*/
  void reset(const T &obj = 0)
  {
    if(m_object)
    {
      if(m_refType == JNIGlobalRefType)
        xbmc_jnienv()->DeleteGlobalRef(m_object);
      else if (m_refType == JNILocalRefType)
        xbmc_jnienv()->DeleteLocalRef(m_object);
    }
    m_refType = JNIInvalidRefType;
    m_object = obj;
  }

private:

/*! \brief Set an object to local/global scope
    New refs will be created as needed.
    If the scope is being set to invalid, its ref will be destroyed as needed.
*/
  void setscope(const jobjectRefType type)
  {
    // Don't attempt anything on a bad object. Update its status to invalid.
    if (!m_object)
    {
      m_refType = JNIInvalidRefType;
      return;
    }

    // Don't bother if the scope isn't actually changing
    if (m_refType == type)
      return;

    if(type == JNIGlobalRefType)
      reset((T)xbmc_jnienv()->NewGlobalRef(m_object));

    else if (type == JNILocalRefType)
      reset((T)xbmc_jnienv()->NewLocalRef(m_object));

    else if (type == JNIInvalidRefType)
      reset();
    m_refType = type;
  }

  jobjectRefType m_refType;
  T m_object;
};

typedef jholder<jclass> jhclass;
typedef jholder<jobject> jhobject;
typedef jholder<jstring> jhstring;
typedef jholder<jthrowable> jhthrowable;
typedef jholder<jarray> jharray;
typedef jholder<jbyteArray> jhbyteArray;
typedef jholder<jbooleanArray> jhbooleanArray;
typedef jholder<jcharArray> jhcharArray;
typedef jholder<jshortArray> jhshortArray;
typedef jholder<jintArray> jhintArray;
typedef jholder<jlongArray> jhlongArray;
typedef jholder<jfloatArray> jhfloatArray;
typedef jholder<jdoubleArray> jhdoubleArray;
typedef jholder<jobjectArray> jhobjectArray;

} // namespace jni

