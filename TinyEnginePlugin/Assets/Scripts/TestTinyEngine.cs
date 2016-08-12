using UnityEngine;
using System.Collections;
using UnityEngine.UI;
using System.Text;

public class TestTinyEngine : MonoBehaviour
{

    public void OnFileExist()
    {

    }

    public void OnMD5()
    {
        string md5 = FileChecker.GetAllFilesMD5();
        text.text = md5;
        Debug.Log(md5);
    }



    public Text text;
}
