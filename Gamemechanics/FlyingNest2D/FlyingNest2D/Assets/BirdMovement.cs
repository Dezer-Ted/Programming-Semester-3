using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class BirdMovement : MonoBehaviour
{
    // Start is called before the first frame update
    [SerializeField]
    private float moveSpeed;
    [SerializeField]
    private float idleRadius;
    private enum States { moving, idling,stopped}
    [SerializeField]
    private States state = States.moving;

    private float turnRange = 2f;
    private Vector3 idleTarget = new Vector3();
    private Vector3 lastMousePos;

 
    

    void Start()
    {
        
    }

    // Update is called once per frame
    void Update()
    {
        Vector3 mousePos = Camera.main.ScreenToWorldPoint(Input.mousePosition);
        mousePos.z = 0;
        if (DidMove(mousePos))
        {
            state = States.moving;
        }
        if (state == States.idling)
        {

            if (!(Vector3.Distance(transform.position, idleTarget) <= 0.1f))
            {
                mousePos = idleTarget;
            }
            else
            {
                state = States.moving;

            }
        }
        else if (CheckIfTooClose(mousePos) && state != States.idling && !DidMove(mousePos))
        {
            mousePos = Idling(mousePos);
        }
        FollowMouse(mousePos);
        RotateToMouse(mousePos);
        lastMousePos = mousePos;
    }

    private Vector3 Idling(Vector3 mousePos)
    {
        state = States.idling;
        Vector2 direction = mousePos - transform.position;
        float angle = Vector2.SignedAngle(Vector2.right, direction);

        Vector3 newPos = new Vector2(turnRange * Mathf.Cos(Mathf.Deg2Rad * angle), turnRange * Mathf.Sin(Mathf.Deg2Rad * angle));
        mousePos += newPos;
        idleTarget = mousePos;
        return mousePos;
    }

    private void RotateToMouse(Vector3 mousePos)
    {
        Vector2 direction = mousePos - transform.position;
        float angle = Vector2.SignedAngle(Vector2.right, direction);
        transform.eulerAngles = new Vector3(0, 0, angle-90);
    }

    private Vector3 FollowMouse(Vector3 mousePos)
    {
        transform.position = Vector2.MoveTowards(transform.position, mousePos, moveSpeed * Time.deltaTime);
        return mousePos;
    }
    private bool CheckIfTooClose(Vector3 mousePos)
    {
        if (Vector3.Distance(mousePos, transform.position) <= idleRadius)
            return true;
        return false;
    }
    private bool DidMove(Vector3 mousePos)
    {
        if((mousePos.x - lastMousePos.x >=5|| mousePos.x - lastMousePos.x <= -5)|| (mousePos.y - lastMousePos.y >= 5 || mousePos.y - lastMousePos.y <= -5))
            return true;
        return false;

    }
}
