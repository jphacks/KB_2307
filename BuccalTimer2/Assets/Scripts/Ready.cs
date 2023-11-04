using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using System;
using System.Net;
using System.Net.Sockets;

public class Ready : MonoBehaviour {
    private Gyroscope m_gyro;
    private Main main;

    public void Start() {
        Input.gyro.enabled = true;
        main = GameObject.FindObjectOfType<Main>();
    }

    public void Update() {
        m_gyro = Input.gyro;
        transform.rotation = m_gyro.attitude;
        if(Mathf.Abs(m_gyro.attitude.x) > 0.8 || Mathf.Abs(m_gyro.attitude.y) > 0.8){
            Debug.Log("UpdateOK");
            StartCoroutine(DelayCoroutine());
        }
    }
/*
    public void OnGUI() {
        
        GUI.skin.label.fontSize = 30;
        GUI.Label(new Rect(40, 200, 600, 50), "attitude=" + m_gyro.attitude.x);
        GUI.Label(new Rect(40, 250, 600, 50), "attitude=" + m_gyro.attitude.y);
        GUI.Label(new Rect(40, 300, 600, 50), "attitude=" + m_gyro.attitude.z);
        GUI.Label(new Rect(40, 350, 600, 50), "rotationRate=" + m_gyro.rotationRate.x);
        GUI.Label(new Rect(40, 400, 600, 50), "rotationRate=" + m_gyro.rotationRate.y);

    }
*/
    public IEnumerator DelayCoroutine()
    {
        // 3秒間待つ
        yield return new WaitForSeconds(3);
        
        // 3秒後に0.9以下ならWorkSpaceに移動してタイマーをスタート
        if(Mathf.Abs(m_gyro.rotationRate.x) < 0.02 && Mathf.Abs(m_gyro.rotationRate.y) < 0.02){
            if(Mathf.Abs(m_gyro.attitude.x) > 0.8 || Mathf.Abs(m_gyro.attitude.y) > 0.8){
                Debug.Log("kidouOK");
                main.Ready.SetActive(false);
                main.WorkTime.SetActive(true);
                
            }
        }
    }

}