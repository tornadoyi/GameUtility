using UnityEngine;
using System.Collections;
using System.IO;
using System.Collections.Generic;
using System.Text;

public class FileChecker
{
    public static string GetAllFilesMD5()
    {
        var list = fileCheckList;

        var builder = new StringBuilder();
        for(int i=0; i<list.Count; ++i)
        {
            var filePath = list[i];
            var fileName = Path.GetFileName(filePath);
            var md5 = TinyEngine.GetFileMD5(filePath);
            builder.AppendFormat("{0}: {1}\n", fileName, md5);
        }
        return builder.ToString();
    }

#if UNITY_ANDROID
    private static List<string> fileCheckList
    {
        get
        {
            var list = new List<string>();
            list.Add(TinyEngine.manifestPath);
            list.Add(TinyEngine.assemblyCSharpfirstpassPath);
            list.Add(TinyEngine.assemblyCSharpPath);
            list.Add(TinyEngine.unityEnginePath);
            list.Add(TinyEngine.certificatePath);
            return list;
        }
    }
#else

    private static List<string> fileCheckList
    {
        get
        {
            var list = new List<string>();
            list.Add(TinyEngine.codeResourcesPath);
            list.Add(TinyEngine.plistPath);
            list.Add(TinyEngine.executeFilePath);
            return list;
        }
    }
    
#endif
}