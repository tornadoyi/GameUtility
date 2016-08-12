using UnityEngine;
using System.Collections;
using UnityEngine.UI;

public class Hello : MonoBehaviour {

    public void OnPress()
    {
        text.text = "hello " + i;
        ++i;
    }

    public Text text;

    int i = 0;
}
