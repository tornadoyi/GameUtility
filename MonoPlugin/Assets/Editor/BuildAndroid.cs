using UnityEngine;
using System.Collections;
using UnityEditor;
using UnityEditor.Callbacks;
using System.IO;
using System;
using System.Collections.Generic;

public class BuildAndroid
{
    [MenuItem("Mono/Build-Android")]
    public static void BuildAndroidPlatform()
    {
        var outputPath = EditorUtility.SaveFolderPanel("Select path", Application.dataPath, string.Empty);
        if (string.IsNullOrEmpty(outputPath)) return;
        var scenes = GetActiveScenes();
        BuildPipeline.BuildPlayer(scenes, outputPath, BuildTarget.Android, BuildOptions.AcceptExternalModificationsToPlayer);
    }


    public static string[] GetActiveScenes()
    {
        var scenes = new List<string>();
        foreach (var scene in EditorBuildSettings.scenes)
        {
            if (!scene.enabled)
            {
                continue;
            }
            scenes.Add(scene.path);
        }
        return scenes.ToArray();
    }

    [PostProcessBuildAttribute]
    public static void OnPostprocessBuild(BuildTarget buildTarget, string path)
    {
        if (buildTarget != BuildTarget.Android) return;
        if (path.Contains(".apk")) return;

        var projectPath = Path.Combine(path, PlayerSettings.productName);

        // Copy build python
        File.Copy(Path.Combine(Application.dataPath, "Plugins/Android/build.py"),
                  Path.Combine(projectPath, "build.py"), true);

        // Copy keystore
        File.Copy(Path.Combine(Application.dataPath, "Plugins/Android/mono.keystore"),
                  Path.Combine(projectPath, "mono.keystore"), true);

        // Copy ant.properties
        File.Copy(Path.Combine(Application.dataPath, "Plugins/Android/ant.properties"),
                  Path.Combine(projectPath, "ant.properties"), true);

        // Encrypt dll
        EncryptFile(Path.Combine(projectPath, "assets/bin/Data/Managed/Assembly-CSharp.dll"));

        // Replace armeabi-v7a mono.so
        File.Copy(Path.Combine(Application.dataPath, "Plugins/Editor/Android/armeabi-v7a/libmono.so"),
                  Path.Combine(projectPath, "libs/armeabi-v7a/libmono.so"), true);

        // Replace x86 mono.so
        File.Copy(Path.Combine(Application.dataPath, "Plugins/Editor/Android/x86/libmono.so"),
                  Path.Combine(projectPath, "libs/x86/libmono.so"), true);
    }

    public static void EncryptFile(string path)
    {
        if(!File.Exists(path))
        {
            Debug.LogErrorFormat("Can not find file {0}", path);
            return;
        }
        byte[] bytes = File.ReadAllBytes(path);
        bytes = Encrypt(bytes);
        File.WriteAllBytes(path, bytes);
    }

    public static byte[] Encrypt(byte[] bytes)
    {
        // Encrypt
        byte enumber = Convert.ToByte(13);
        int ecount = 16;
        int elength = bytes.Length < ecount ? bytes.Length : ecount;
        for (int i = 0; i < elength; ++i)
        {
            bytes[i] ^= enumber;
        }

        return bytes;
    }

}
