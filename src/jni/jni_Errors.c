/* JNativeHook: Global keyboard and mouse hooking for Java.
 * Copyright (C) 2006-2014 Alexander Barker.  All Rights Received.
 * https://github.com/kwhat/jnativehook/
 *
 * JNativeHook is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published
 * by the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * JNativeHook is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <jni.h>
#include <stdlib.h>

#include "jni_Errors.h"
#include "jni_Globals.h"
#include "jni_Logger.h"

void jni_ThrowFatalError(JNIEnv *env, const char *message) {
	#ifdef USE_DEBUG
	fprintf(stderr, "Fatal Error: %s\n", message);
	#endif

	(*env)->FatalError(env, message);

	exit(EXIT_FAILURE);
}

void jni_ThrowException(JNIEnv *env, const char *classname, const char *message) {
	// Locate our exception class.
	jclass Exception_class = (*env)->FindClass(env, classname);

	if (Exception_class != NULL) {
		(*env)->ThrowNew(env, Exception_class, message);
		#ifdef USE_DEBUG
		fprintf(stderr, "%s: %s\n", classname, message);
		#endif
		(*env)->DeleteLocalRef(env, Exception_class);
	}
	else {
		Exception_class = (*env)->FindClass(env, java_lang_NoClassDefFoundError);

		if (Exception_class != NULL) {
			#ifdef USE_DEBUG
			fprintf(stderr, "%s: %s\n", java_lang_NoClassDefFoundError, classname);
			#endif

			(*env)->ThrowNew(env, Exception_class, classname);
			(*env)->DeleteLocalRef(env, Exception_class);
		}
		else {
			// Unable to find exception class, Terminate with error.
			jni_ThrowFatalError(env, "Unable to locate java classes.");
		}
	}
}

void jni_ThrowNativeHookException(JNIEnv *env, short code, const char *message) {
	// Locate our exception class.
	jclass Exception_class = (*env)->FindClass(env, "org/jnativehook/NativeHookException");

	if (Exception_class != NULL) {
		jmethodID init = (*env)->GetMethodID(
				env,
				Exception_class,
				"<init>",
				"(SLjava/lang/String;)V");

		if (init != NULL) {
			jobject Exception_object = (*env)->NewObject(
					env,
					Exception_class,
					init,
					(jshort) code,
					(*env)->NewStringUTF(env, message));

			(*env)->Throw(env, (jthrowable) Exception_object);
			(*env)->DeleteLocalRef(env, Exception_object);
		}
		else {
			jni_Logger(LOG_LEVEL_ERROR, "%s [%u]: Failed to acquire the method ID for NativeHookException.<init>(SLjava/lang/String;)V!\n",
					__FUNCTION__, __LINE__);
		}
	}
	else {
		jni_Logger(LOG_LEVEL_ERROR, "%s [%u]: Failed to locate the NativeHookException class!\n",
    			__FUNCTION__, __LINE__);
	}
}
