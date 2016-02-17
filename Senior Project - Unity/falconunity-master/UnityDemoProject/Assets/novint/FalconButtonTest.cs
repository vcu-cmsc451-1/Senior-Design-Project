using UnityEngine;
using System.Collections;

public class FalconButtonTest : MonoBehaviour {
    bool[] button; 
	// Use this for initialization
	void Start () {
	
	}
	
	// Update is called once per frame
	void FixedUpdate () {
        Debug.Log(FalconUnity.getFalconButtonStates(FalconUnity.getNumFalcons(), out button));

	}
}
