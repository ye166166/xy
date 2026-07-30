using UnityEngine;

public class PlayerController : MonoBehaviour
{
    public float walkSpeed = 5f;
    public float mouseSensitivity = 2f;
    public float maxLookAngle = 80f;

    private CharacterController characterController;
    private Camera playerCamera;
    private Vector3 velocity;
    private float verticalRotation = 0f;

    void Start()
    {
        characterController = GetComponent<CharacterController>();
        playerCamera = GetComponentInChildren<Camera>();
        Cursor.lockState = CursorLockMode.Locked;
        Cursor.visible = false;
    }

    void Update()
    {
        HandleMouseLook();
        HandleMovement();
    }

    void HandleMouseLook()
    {
        float mouseX = Input.GetAxis("Mouse X") * mouseSensitivity;
        float mouseY = Input.GetAxis("Mouse Y") * mouseSensitivity;

        verticalRotation -= mouseY;
        verticalRotation = Mathf.Clamp(verticalRotation, -maxLookAngle, maxLookAngle);
        playerCamera.transform.localRotation = Quaternion.Euler(verticalRotation, 0, 0);
        transform.Rotate(Vector3.up * mouseX);
    }

    void HandleMovement()
    {
        if (characterController.isGrounded && velocity.y < 0)
            velocity.y = -2f;

        float x = Input.GetAxis("Horizontal");
        float z = Input.GetAxis("Vertical");
        Vector3 move = transform.right * x + transform.forward * z;
        characterController.Move(move * walkSpeed * Time.deltaTime);

        velocity.y += -9.81f * Time.deltaTime;
        characterController.Move(velocity * Time.deltaTime);
    }
}
