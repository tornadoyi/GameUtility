using UnityEngine;
using System.Collections;
using System.Runtime.InteropServices;
using System;
using System.IO;

public class TinyEngine
{
    public static string GetFileMD5(string filePath) { var ptr = IntPtr.Zero; var len = file_md5(filePath, out ptr); return DecodePtr2String(ptr, len); }

    public static bool IsFileExist(string filePath) { return file_exist(filePath); }

    private static string DecodePtr2String(IntPtr ptr, int length)
    {
        if (ptr == IntPtr.Zero || length == 0) return string.Empty;
        var data = new byte[length];
        Marshal.Copy(ptr, data, 0, length);
        memory_free(ptr);
        return System.Text.Encoding.UTF8.GetString(data);
    }

    public static string appPath { get { var ptr = IntPtr.Zero; var len = file_app_path(out ptr); return DecodePtr2String(ptr, len); } }

    public static string dataPath { get { IntPtr ptr = IntPtr.Zero; var len = file_data_path(out ptr); return DecodePtr2String(ptr, len); } }

    public static string persistentDataPath { get { IntPtr ptr = IntPtr.Zero; var len = file_persistent_data_path(out ptr);  return DecodePtr2String(ptr, len); } }

    public static string temporaryCachePath { get { IntPtr ptr = IntPtr.Zero; var len = file_temporary_cache_path(out ptr); return DecodePtr2String(ptr, len); } }

    public static string executeFilePath { get { IntPtr ptr = IntPtr.Zero; var len = file_exe_path(out ptr); return DecodePtr2String(ptr, len); } }

#if UNITY_ANDROID

    public static string manifestPath { get { return "AndroidManifest.xml"; } }

    public static string assemblyCSharpfirstpassPath { get { return "assets/bin/Data/Managed/Assembly-CSharp-firstpass.dll"; } }

    public static string assemblyCSharpPath { get { return "assets/bin/Data/Managed/Assembly-CSharp.dll"; } }

    public static string unityEnginePath { get { return "assets/bin/Data/Managed/UnityEngine.dll"; } }

    public static string certificatePath { get { return "META-INF/CERT.RSA"; } }

#else

    public static string codeResourcesPath { get { return Path.Combine(Path.Combine(appPath, "_CodeSignature"), "CodeResources"); } }

    public static string plistPath { get { return Path.Combine(appPath, "Info.plist"); } }

    public static string assemblyCSharpfirstpassPath { get { return Path.Combine(appPath, "Data/Managed/Assembly-CSharp-firstpass.dll"); } }

    public static string assemblyCSharpPath { get { return Path.Combine(appPath, "Data/Managed/Assembly-CSharp.dll"); } }

    public static string assemblyUnityScriptPath { get { return Path.Combine(appPath, "Data/Managed/Assembly-UnityScript.dll"); } }

#endif




#if UNITY_ANDROID

    // Memory
    [DllImport("TinyEngine")]
    private static extern void memory_free(IntPtr ptr);


    // File
    [DllImport("TinyEngine")]
    private static extern int file_app_path(out IntPtr path);

    [DllImport("TinyEngine")]
    private static extern int file_data_path(out IntPtr path);

    [DllImport("TinyEngine")]
    private static extern int file_persistent_data_path(out IntPtr path);

    [DllImport("TinyEngine")]
    private static extern int file_temporary_cache_path(out IntPtr path);

    [DllImport("TinyEngine")]
    private static extern int file_exe_path(out IntPtr path);

    [DllImport("TinyEngine")]
    private static extern int file_md5(string filePath, out IntPtr md5);

    [DllImport("TinyEngine")]
    private static extern bool file_exist(string filePath);

#else

    // Memory
    [DllImport("__Internal")]
    private static extern void memory_free(IntPtr ptr);


    // File
    [DllImport("__Internal")]
    private static extern int file_app_path(out IntPtr path);

    [DllImport("__Internal")]
    private static extern int file_data_path(out IntPtr path);

    [DllImport("__Internal")]
    private static extern int file_persistent_data_path(out IntPtr path);

    [DllImport("__Internal")]
    private static extern int file_temporary_cache_path(out IntPtr path);

    [DllImport("__Internal")]
    private static extern int file_exe_path(out IntPtr path);

    [DllImport("__Internal")]
    private static extern int file_md5(string filePath, out IntPtr md5);

    [DllImport("__Internal")]
    private static extern bool file_exist(string filePath);

#endif


}