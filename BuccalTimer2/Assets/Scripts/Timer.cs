using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public enum LightState{
    Orange,
    Red,
    White
}

public class Timer : MonoBehaviour
{
    private GameObject kurione;
    private int _WorkMinute;
    private int _RestMinute;
    private int loop_count = 0;
    private Main main;
    private float timer;
    public int DefaultWorKMinute;
    public int DefaultRestMinute;
    public int Mode = 0;
    LightState lightColor = LightState.White;
    
    // Start is called before the first frame update
    void Start()
    {
        _WorkMinute = int.Parse("main.WorkMinuteText");
        _RestMinute = int.Parse("main.RestMinuteText");
        DefaultWorKMinute = _WorkMinute;
        DefaultRestMinute = _RestMinute;
        timer = 0f;
        Debug.Log(_WorkMinute + "minutes left.");
        main = GameObject.FindObjectOfType<Main>();
    }

    // Update is called once per frame
    void Update()
    {
        if(Mode == 0){
            timer += Time.deltaTime; //経過時間を計測
        
            if(timer >= 60.0f){
                _WorkMinute = _WorkMinute - 1;  
                timer = 0f;

                if(_WorkMinute  > 0){
                    Debug.Log(_WorkMinute + "minutes left.");
                }
                else if(_WorkMinute  == 0){
                    lightColor = LightState.Orange;
                    main.WorkTime.SetActive(false);
                    Debug.Log("End Work Time.");
                    timer = 0f;
                    Mode = 1;
                    _WorkMinute = DefaultWorKMinute;
                }
            }
        }
        else if(Mode == 1){
            timer += Time.deltaTime; 

            if(timer >= 60.0f){
                _RestMinute = _RestMinute - 1;
                timer = 0f;

                if(_RestMinute > 0){
                    Debug.Log(_RestMinute + "minutes left.");
                }
                else if(_RestMinute == 0){
                    lightColor = LightState.White;
                    Debug.Log("End Rest Time.");
                    timer = 0f;
                    main.WorkTime.SetActive(true);
                    _RestMinute = DefaultWorKMinute;
                    loop_count = loop_count + 1;
                    Mode = 0;
                    Instantiate(kurione, new Vector3(Random.Range(0f, 100f), Random.Range(0f, 100f), 20f), Quaternion.identity);
                }
            }
        }
    }
    
}