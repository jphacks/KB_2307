using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using System;
using System.Net;
using System.Net.Sockets;
using TMPro;


public class Main : MonoBehaviour
{
    //各シーンの取得と有効無効化
    public GameObject Home;
    public GameObject Ready;
    public GameObject WorkTime;
    public GameObject BuccalCone;
    public TMP_InputField WorkMinute;
    public TMP_InputField RestMinute;
    public string WorkMinuteText;
    public string RestMinuteText;
    public bool HomeScene_SetActive = true;

    //-----------ジャイロの取得------------
    private Gyroscope m_gyro;

    public void Start() {
        Input.gyro.enabled = true;
    }

    public void Update() {
        m_gyro = Input.gyro;
        transform.rotation = m_gyro.attitude;
    }

    //-----------文字取得---------------
    public void OnEndEdit()
    {
        WorkMinuteText = WorkMinute.text;
        RestMinuteText = RestMinute.text;
        Debug.Log(WorkMinuteText);
        Debug.Log(RestMinuteText);
    }

    //--------------シーン変更---------------

    public void Load_Ready()
    {
        //Image、TextUIを出したり消したりする
        if (HomeScene_SetActive)
        {
            Home.SetActive(false);
            Ready.SetActive(true);
            WorkTime.SetActive(false);
            BuccalCone.SetActive(false);
        }
    }  
}
