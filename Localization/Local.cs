using UnityEngine;
using System.Collections;
using UnityEditor;
using System.IO;
using System.Collections.Generic;
using System;
using System.Text.RegularExpressions;
using System.Text;

public class LocalWindow : EditorWindow
{
    [MenuItem("Mirror/Localize")]
    public static void ShowWindow()
    {
        //Show existing window instance. If one doesn't exist, make one.
        var window = EditorWindow.GetWindow<LocalWindow>();
    }

    void OnGUI()
    {
        {
            EditorGUILayout.BeginHorizontal();
            workspace = EditorGUILayout.TextField("Workspace", workspace);
            if (GUILayout.Button("Open", GUILayout.MaxWidth(100)))
            {
                workspace = EditorUtility.OpenFolderPanel("Select workspace", Application.dataPath, "");
            }
            EditorGUILayout.EndHorizontal();
        }

        {
            EditorGUILayout.BeginHorizontal();
            symbolFilePath = EditorGUILayout.TextField("Symbol file", symbolFilePath);
            if (GUILayout.Button("Open", GUILayout.MaxWidth(100)))
            {
                symbolFilePath = EditorUtility.OpenFilePanel("Select symbol file", Application.dataPath, "txt");
            }
            EditorGUILayout.EndHorizontal();
        }

        {
            EditorGUILayout.BeginHorizontal();
            outputPath = EditorGUILayout.TextField("Output Path", outputPath);
            if (GUILayout.Button("Open", GUILayout.MaxWidth(100)))
            {
                outputPath = EditorUtility.OpenFolderPanel("Select workspace", Application.dataPath, "");
            }
            EditorGUILayout.EndHorizontal();
        }

        {
            //EditorGUILayout.BeginHorizontal();
            detail = EditorGUILayout.Toggle("Detail", detail);
            ignoreInvalidSymbol = EditorGUILayout.Toggle("Ignore Invalid Symbol", ignoreInvalidSymbol);
            //EditorGUILayout.EndHorizontal();
        }

        bool parse = false;
        bool check = false;
        bool translate = false;
        {
            EditorGUILayout.BeginHorizontal();
            if (GUILayout.Button("Parse"))
            {
                parse = true;
            }

            if (GUILayout.Button("Check"))
            {
                check = true;
            }

            if (GUILayout.Button("Translate"))
            {
                translate = true;
            }

            EditorGUILayout.EndHorizontal();
        }

        if (parse) { Parse(); }
        else if (check) { Check(); }
        else if (translate) { Translate(); }

    }




    public void Parse()
    {
        if (string.IsNullOrEmpty(workspace)) return;
        if (string.IsNullOrEmpty(outputPath)) return;

        var localList = ParseCore();
        var builder = new StringBuilder();
        foreach(var info in localList)
        {
            foreach(var label in info.labels)
            {
                var text = ProcessText(label.text);
                builder.AppendLine(FormatLocalInfo(info, text));
            }
        }
        File.WriteAllText(Path.Combine(outputPath, "parse.txt"), builder.ToString());
    }

    public void Check()
    {
        if (string.IsNullOrEmpty(workspace)) return;
        if (string.IsNullOrEmpty(outputPath)) return;
        if (string.IsNullOrEmpty(symbolFilePath)) return;

        var symbolDict = ReadSymbolFile(symbolFilePath);
        var localList = ParseCore();
        var builder = new StringBuilder();
        foreach (var info in localList)
        {
            foreach (var label in info.labels)
            {
                var text = ProcessText(label.text);
                if (symbolDict.ContainsKey(text)) continue;
                builder.AppendLine(FormatLocalInfo(info, text));
            }
        }
        File.WriteAllText(Path.Combine(outputPath, "check.txt"), builder.ToString());
    }

    public void Translate()
    {
        if (string.IsNullOrEmpty(workspace)) return;
        if (string.IsNullOrEmpty(outputPath)) return;
        if (string.IsNullOrEmpty(symbolFilePath)) return;

        var symbolDict = ReadSymbolFile(symbolFilePath);
        var localList = ParseCore();
        var builder = new StringBuilder();
        foreach (var info in localList)
        {
            foreach (var label in info.labels)
            {
                var text = ProcessText(label.text);
                string localText = null;
                if(!symbolDict.TryGetValue(text, out localText))
                {
                    builder.AppendLine(FormatLocalInfo(info, text));
                    continue;
                }

                label.text = localText;
            }
            EditorUtility.SetDirty(info.go);
        }
        File.WriteAllText(Path.Combine(outputPath, "translate.txt"), builder.ToString());

        AssetDatabase.SaveAssets();
        AssetDatabase.Refresh(ImportAssetOptions.ForceSynchronousImport);
    }

    protected List<LocalPrefab> ParseCore()
    {
        Resources.UnloadUnusedAssets();
        GC.Collect();

        var localList = new List<LocalPrefab>();

        var files = Directory.GetFiles(workspace, "*.prefab", SearchOption.AllDirectories);
        foreach (var file in files)
        {
            var prefabPath = GetRelativePath(file, projectPath);
            var go = AssetDatabase.LoadAssetAtPath<GameObject>(prefabPath);
            var labels = go.GetComponentsInChildren<UILabel>();

            LocalPrefab info = null;
            foreach (var label in labels)
            {
                if (!NeedLocalize(label.text)) continue;
                if(info == null)
                {
                    info = new LocalPrefab();
                    info.filePath = prefabPath;
                    info.go = go;
                }
                info.labels.Add(label);
            }

            if(info != null)
            {
                localList.Add(info);
            }
        }
        return localList;
    }

    
    public string FormatLocalInfo(LocalPrefab info, string text)
    {
        if (!detail)
        {
            return text;
        }
        else
        {
            return string.Format("{0} - {1} : {2}", info.filePath, info.go.name, text);
        }
    }

    public Dictionary<string, string> ReadSymbolFile(string symbolFilePath)
    {
        var symbolDict = new Dictionary<string, string>();

        var lines = File.ReadAllLines(symbolFilePath);
        int curLine = 0;
        foreach (var line in lines)
        {
            ++curLine;
            var kv = line.Split('=');
            if(kv.Length != 2)
            {
                if(!ignoreInvalidSymbol)  Debug.LogErrorFormat("Error: invalid symbol line {0}, missing =", curLine);
                continue;
            }
            var k = kv[0].TrimEnd();
            var v = kv[1].TrimStart().TrimEnd('\n').Replace("\\n", "\n");
            symbolDict[k] = v;
        }

        return symbolDict;
    }

    public static string ProcessText(string text)
    {
        var str = text;
        str = str.Replace("\n\r", "\\n");
        str = str.Replace("\n", "\\n");
        str = str.Replace("\r", "\\n");
        return str;
    }

    public static bool NeedLocalize(string str)
    {
        if (string.IsNullOrEmpty(str)) return false;
        return Regex.IsMatch(str, @"[\u00ff-\uffff]");
    }


    public static string GetRelativePath(string fullPath, string rootPath)
    {
        int st = fullPath.IndexOf(rootPath);
        if (st == -1) return null;
        if (fullPath.Length == rootPath.Length) return "";
        return fullPath.Substring(st + rootPath.Length + 1);
    }


    public class LocalPrefab
    {
        public string filePath;
        public GameObject go;
        public List<UILabel> labels = new List<UILabel>();
    }

    protected string workspace
    {
        get
        {
            var path = EditorPrefs.GetString("locallize_workspace");
            if (string.IsNullOrEmpty(path)) return string.Empty;
            return path;
        }
        set
        {
            if (string.IsNullOrEmpty(value)) value = string.Empty;
            EditorPrefs.SetString("locallize_workspace", value);
        }
    }

    protected string symbolFilePath
    {
        get
        {
            var path = EditorPrefs.GetString("locallize_symbol_file");
            if (string.IsNullOrEmpty(path)) return string.Empty;
            return path;
        }
        set
        {
            if (string.IsNullOrEmpty(value)) value = string.Empty;
            EditorPrefs.SetString("locallize_symbol_file", value);
        }
    }

    protected string outputPath
    {
        get
        {
            var path = EditorPrefs.GetString("locallize_output_path");
            if (string.IsNullOrEmpty(path)) return string.Empty;
            return path;
        }
        set
        {
            if (string.IsNullOrEmpty(value)) value = string.Empty;
            EditorPrefs.SetString("locallize_output_path", value);
        }
    }


    protected  string projectPath
    {
        get
        {
            var path = Directory.GetParent(Application.dataPath).FullName;
            return path.Replace("\\", "/");
        }
    }

    protected bool detail = false;

    protected bool ignoreInvalidSymbol = false;
}
