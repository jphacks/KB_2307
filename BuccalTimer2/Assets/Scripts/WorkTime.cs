using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;
using System;


public class WorkTime : MonoBehaviour {
    private Gyroscope m_gyro;
    private Main main;

    public void Start() {
        Input.gyro.enabled = true;
        main = GameObject.FindObjectOfType<Main>();
    }

    public void Update() {
        m_gyro = Input.gyro;
        transform.rotation = m_gyro.attitude;
        WorkTimeAction();
    }

    public void WorkTimeAction() {
        Debug.Log("Work");
        
        if(Mathf.Abs(m_gyro.rotationRate.x) > 0.02 || Mathf.Abs(m_gyro.rotationRate.y) > 0.02){
            main.WorkTime.SetActive(false);
            main.BuccalCone.SetActive(true);
        }
    }
    
}