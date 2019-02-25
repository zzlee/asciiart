using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class moveScript : MonoBehaviour {

    public Vector3 ampVect;
    public float duration;
    Vector3 orgPos;
	void Start () {
        orgPos = transform.localPosition;

    }
	
	// Update is called once per frame
	void Update () {
        transform.localPosition = orgPos + ampVect * Mathf.Sin(Time.time / duration);

    }
}
