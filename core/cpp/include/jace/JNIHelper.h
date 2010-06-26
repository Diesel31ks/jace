
#ifndef JACE_JNI_HELPER_H
#define JACE_JNI_HELPER_H

#ifndef JACE_OS_DEP_H
#include "jace/os_dep.h"
#endif

#ifndef JACE_NAMESPACE_H
#include "jace/namespace.h"
#endif

#ifndef JACE_JNI_EXCEPTION_H
#include "jace/JNIException.h"
#endif

#ifndef JACE_VIRTUAL_MACHINE_RUNNING_ERROR_H
#include "jace/VirtualMachineRunningError.h"
#endif

#ifndef JACE_VIRTUAL_MACHINE_SHUTDOWN_ERROR_H
#include "jace/VirtualMachineShutdownError.h"
#endif

#ifndef JACE_JFACTORY_H
#include "jace/JFactory.h"
#endif

#ifndef JACE_VM_LOADER
#include "jace/VmLoader.h"
#endif

#ifndef JACE_OPTION_LIST_H
#include "jace/OptionList.h"
#endif

#ifdef SUPPORTS_SSTREAM
  #include <sstream>
  using std::stringstream;
	using std::wstringstream;
#else
  #include <strstream>
  using std::strstream;
#endif

BEGIN_NAMESPACE(jace)
class Peer;
END_NAMESPACE(jace)

#include <jni.h>


BEGIN_NAMESPACE_2(jace, helper)


/**
 * A namespace for helper functions which make it easier to 
 * use the JNI API.
 *
 * @author Toby Reyelts
 *
 */


/**
 * Creates a new Java Virtual Machine using the specified loader
 * with the specified options.
 *
 * To link with a virtual machine you may specify any dynamic loading
 * VmLoader (for example, UnixVmLoader or Win32VmLoader).
 *
 * This call results in a call to setVmLoader internally.
 * @throws VirtualMachineRunningError if the virtual machine is already running
 * @throws JNIException if the virtual machine cannot be created
 */
JACE_API void createVm(const VmLoader& loader, 
											 const OptionList& options, 
											 bool ignoreUnrecognized = true)
											 throw (VirtualMachineRunningError, JNIException);

/**
 * Destroys the current Java Virtual Machine and tells Jace that it
 * shouldn't try to re-attach any more threads.
 * After calling this function, most other functions will fail.
 *
 * If the current thread is attached, the VM waits until the current
 * thread is the only non-daemon user-level Java thread. If the
 * current thread is not attached, the VM attaches the current thread
 * and then waits until the current thread is the only non-daemon
 * user-level thread. The JDK/JRE still does not support VM unloading, however.
 *
 * @see http://java.sun.com/javase/6/docs/technotes/guides/jni/spec/invocation.html#destroy_java_vm
 * @throws JNIException if the virtual machine fails to shut down
 */
JACE_API void destroyVm() throw (JNIException);

/**
 * Sets the current running java virtual machine. This method can be used to implement a custom vm
 * loading policy outside of createVm.
 *
 * @param jvm a running java virtual machine
 * @throws VirtualMachineRunningError if a JVM is already running
 * @throws JNIException if an error occurs while registering the shutdown hook
 */
JACE_API void setJavaVm(JavaVM* jvm) throw(VirtualMachineRunningError, JNIException);

/**
 * Returns the current java virtual machine.
 *
 * @return null if no virtual machine is running
 */
JACE_API JavaVM* getJavaVm();


/**
 * Attaches the current thread to the virtual machine and returns the appropriate 
 * JNIEnv for the thread. If the thread is already attached, this method method 
 * does nothing.
 *
 * This method is equivilent to attach(0, 0, false).
 *
 * @throws JNIException if an error occurs while trying to attach the current thread.
 * @see AttachCurrentThread
 * @see attach(const jobject, const char*, const bool)
 * @throws JNIException if an error occurs while attaching the current thread
 * @throws VirtualMachineShutdownError if the virtual machine is not running
 */
JACE_API JNIEnv* attach() throw (JNIException, VirtualMachineShutdownError);


/**
 * Attaches the current thread to the virtual machine
 * and returns the appropriate JNIEnv for the thread.
 * If the thread is already attached, this method method does nothing.
 *
 * @param threadGroup the ThreadGroup associated with the thread, or null
 * @param name the thread name as a modified UTF-8 string, or null
 * @param daemon true if the thread should be attached as a daemon thread
 * @see AttachCurrentThread
 * @see AttachCurrentThreadAsDaemon
 * @see http://en.wikipedia.org/wiki/UTF-8#Modified_UTF-8
 * @throws JNIException if an error occurs while trying to attach the current thread.
 * @throws VirtualMachineShutdownError if the virtual machine is not running
 */
JACE_API JNIEnv* attach(const jobject threadGroup, const char* name, const bool daemon)
	throw (JNIException, VirtualMachineShutdownError);


/**
 * Detaches the current thread from the virtual machine.
 *
 * @see DetachCurrentThread
 */
JACE_API void detach() throw ();

/**
 * A central point for allocating new local references.
 * These references must be deallocated by a call to deleteLocalRef.
 *
 * @throws JNIException if the local reference can not be allocated.
 */
JACE_API jobject newLocalRef(JNIEnv* env, jobject ref);


/**
 * A central point for deleting local references.
 *
 * @throws VirtualMachineShutdownError if the virtual machine is not running
 */
JACE_API void deleteLocalRef(JNIEnv* env, jobject localRef) throw (VirtualMachineShutdownError);


/**
 * A central point for allocating new global references.
 * These references must be deallocated by a call to deleteGlobalRef.
 *
 * @throws VirtualMachineShutdownError if the virtual machine is not running
 * @throws JNIException if the global reference can not be allocated.
 */
JACE_API jobject newGlobalRef(JNIEnv* env, jobject ref) throw (VirtualMachineShutdownError, JNIException);


/**
 * A central point for deleting global references.
 */
JACE_API void deleteGlobalRef(JNIEnv* env, jobject globalRef);


/**
 * Enlists a new factory for a java class with the JNIHelper.
 *
 * All java classes should enlist with the JNIHelper on start-up.
 * They can do this by adding a static member variable
 * of type JEnlister to their class definition.
 *
 * For example, java::lang::Object has a static member variable,
 *
 *   static JEnlister<Object> enlister;
 *
 * which is all that is required to register a new factory
 * for itself.
 */
JACE_API void enlist(JFactory* factory);


/**
 * Checks to see if a java exception has been thrown.
 * 
 * If an exception has been thrown, a corresponding C++ proxy
 * exception is constructed and thrown.
 */
JACE_API void catchAndThrow();

/**
 * Returns the Peer for a given java Peer.
 */
JACE_API Peer* getPeer(jobject jPeer);

/**
 * Returns the ClassLoader being used by the current thread.
 *
 */
JACE_API jobject getClassLoader();

/**
 * Sets the ClassLoader to be used by the current thread.
 *
 * By default, Jace uses the JNIEnv->FindClass() to load classes,
 * but if a thread ClassLoader is defined then it is used to load
 * classes instead. A thread ClassLoader must be defined under
 * Java Webstart, Applets or any other framework that makes use
 * of custom ClassLoaders to load classes.
 *
 * NOTE: You must setClassLoader(0) to release the ClassLoader
 *       reference or detach() will do it for you on thread shutdown.
 */
JACE_API void setClassLoader(jobject classLoader);

/**
 * Returns the string representation of any type.
 */
template <class T> std::string toString(T value)
{
#ifdef SUPPORTS_SSTREAM
    std::stringstream stream;
#else
    std::strstream stream;
#endif
    stream << value;
    return stream.str();
}

#ifdef SUPPORTS_SSTREAM
/**
 * Returns the wstring representation of any type.
 */
template <class T> std::wstring toWString(T value)
{
  std::wstringstream stream;
  stream << value;
  return stream.str();
}
#endif

/**
 * Returns the result of calling Object.toString() on obj.
 * Useful for low level debugging.
 */
JACE_API std::string toString(jobject obj);

/**
 * Prints toString(obj) to cout.
 */
JACE_API void print(jobject obj);

/**
 * Prints the result of calling Object.getClass().toString() on obj to cout.
 */
JACE_API void printClass(jobject obj);

/**
 * Indicates if Java Virtual Machine is running.
 */
JACE_API bool isRunning();

END_NAMESPACE_2(jace, helper)

#endif