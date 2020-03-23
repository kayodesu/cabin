/*
 * Author: kayo
 */

#include "../../../../runtime/frame.h"
#include "../../../../objects/array_object.h"

/*
 * Gather the VM and command line properties and return as a String[].
 * The array indices are alternating key/value pairs
 * supplied by the VM including those defined on the command line
 * using -Dkey=value that may override the platform defined value.
 *
 * Note: The platform encoding must have been set.
 *
 * Class:     jdk_internal_util_SystemProps_Raw
 * Method:    vmProperties
 * Signature: ()[Ljava/lang/String;
 */
// private static native String[] vmProperties();
static void vmProperties(Frame *frame)
{
    jarrref prop_array = newArray(loadBootClass(S(array_java_lang_String)), g_properties.size()*2);
    int i = 0;
    for (auto p: g_properties) {
        prop_array->set(i++, newString(p.first));
        prop_array->set(i++, newString(p.second));
    }
    frame->pushr(prop_array);
}

/*
 * Gather the system properties and return as a String[].
 * The first FIXED_LENGTH entries are the platform defined property values, no names.
 * The remaining array indices are alternating key/value pairs
 * supplied by the VM including those defined on the command line
 * using -Dkey=value that may override the platform defined value.
 * The caller is responsible for replacing platform provided values as needed.
 *
 * Class:     jdk_internal_util_SystemProps_Raw
 * Method:    platformProperties
 * Signature: ()[Ljava/lang/String;
 */
// private static native String[] platformProperties();
static void platformProperties(Frame *frame)
{
    // from class jdk/internal/util/SystemProps$Raw
    static const int _display_country_NDX = 0;
    static const int _display_language_NDX = 1 + _display_country_NDX;
    static const int _display_script_NDX = 1 + _display_language_NDX;
    static const int _display_variant_NDX = 1 + _display_script_NDX;
    static const int _file_encoding_NDX = 1 + _display_variant_NDX;
    static const int _file_separator_NDX = 1 + _file_encoding_NDX;
    static const int _format_country_NDX = 1 + _file_separator_NDX;
    static const int _format_language_NDX = 1 + _format_country_NDX;
    static const int _format_script_NDX = 1 + _format_language_NDX;
    static const int _format_variant_NDX = 1 + _format_script_NDX;
    static const int _ftp_nonProxyHosts_NDX = 1 + _format_variant_NDX;
    static const int _ftp_proxyHost_NDX = 1 + _ftp_nonProxyHosts_NDX;
    static const int _ftp_proxyPort_NDX = 1 + _ftp_proxyHost_NDX;
    static const int _http_nonProxyHosts_NDX = 1 + _ftp_proxyPort_NDX;
    static const int _http_proxyHost_NDX = 1 + _http_nonProxyHosts_NDX;
    static const int _http_proxyPort_NDX = 1 + _http_proxyHost_NDX;
    static const int _https_proxyHost_NDX = 1 + _http_proxyPort_NDX;
    static const int _https_proxyPort_NDX = 1 + _https_proxyHost_NDX;
    static const int _java_io_tmpdir_NDX = 1 + _https_proxyPort_NDX;
    static const int _line_separator_NDX = 1 + _java_io_tmpdir_NDX;
    static const int _os_arch_NDX = 1 + _line_separator_NDX;
    static const int _os_name_NDX = 1 + _os_arch_NDX;
    static const int _os_version_NDX = 1 + _os_name_NDX;
    static const int _path_separator_NDX = 1 + _os_version_NDX;
    static const int _socksNonProxyHosts_NDX = 1 + _path_separator_NDX;
    static const int _socksProxyHost_NDX = 1 + _socksNonProxyHosts_NDX;
    static const int _socksProxyPort_NDX = 1 + _socksProxyHost_NDX;
    static const int _sun_arch_abi_NDX = 1 + _socksProxyPort_NDX;
    static const int _sun_arch_data_model_NDX = 1 + _sun_arch_abi_NDX;
    static const int _sun_cpu_endian_NDX = 1 + _sun_arch_data_model_NDX;
    static const int _sun_cpu_isalist_NDX = 1 + _sun_cpu_endian_NDX;
    static const int _sun_io_unicode_encoding_NDX = 1 + _sun_cpu_isalist_NDX;
    static const int _sun_jnu_encoding_NDX = 1 + _sun_io_unicode_encoding_NDX;
    static const int _sun_os_patch_level_NDX = 1 + _sun_jnu_encoding_NDX;
    static const int _sun_stderr_encoding_NDX = 1 + _sun_os_patch_level_NDX;
    static const int _sun_stdout_encoding_NDX = 1 + _sun_stderr_encoding_NDX;
    static const int _user_dir_NDX = 1 + _sun_stdout_encoding_NDX;
    static const int _user_home_NDX = 1 + _user_dir_NDX;
    static const int _user_name_NDX = 1 + _user_home_NDX;
    static const int FIXED_LENGTH = 1 + _user_name_NDX;

    jarrref prop_array = newArray(loadBootClass(S(array_java_lang_String)), FIXED_LENGTH);
    // todo 具体赋值

    for (int i = 0; i < FIXED_LENGTH; i++) {
        prop_array->set(i, jnull);
    }
    frame->pushr(prop_array);
}

void jdk_internal_util_SystemProps$Raw_registerNatives()
{
    registerNative("jdk/internal/util/SystemProps$Raw", "vmProperties", "()[Ljava/lang/String;", vmProperties);
    registerNative("jdk/internal/util/SystemProps$Raw", "platformProperties", "()[Ljava/lang/String;", platformProperties);
}

