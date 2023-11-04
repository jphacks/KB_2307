using UnityEngine;
using UnityEngine.UI;

public class UIController : MonoBehaviour
{
    public GameObject panel;
    bool UI_SetActive = false;

    public void Start()
    {
      
    }

    public void ToggleUI()
    {
        //Image、TextUIを出したり消したりする
        if (UI_SetActive)
        {
            panel.SetActive(false);
            UI_SetActive = false;
        }
        else
        {
            panel.SetActive(true);
            UI_SetActive = true;
        }
        
    }
    
}