using UnityEngine;

public class PlayerInteraction : MonoBehaviour
{
    public float interactRange = 3f;
    public Transform carryPoint;
    public LayerMask interactLayer = ~0;

    private Camera playerCamera;
    private GameObject carriedItem;
    private Rigidbody carriedRigidbody;
    private UIManager uiManager;

    void Start()
    {
        playerCamera = GetComponentInChildren<Camera>();
        uiManager = FindObjectOfType<UIManager>();
        if (carryPoint == null)
        {
            carryPoint = new GameObject("CarryPoint").transform;
            carryPoint.SetParent(playerCamera.transform);
            carryPoint.localPosition = new Vector3(0, -0.25f, 1.2f);
        }
    }

    void Update()
    {
        HandleInteraction();
        UpdateCarriedItem();
        if (Input.GetMouseButtonDown(1) && carriedItem != null)
        {
            DropItem();
            ShowMessage("Dropped item.");
        }
    }

    void HandleInteraction()
    {
        if (!Input.GetMouseButtonDown(0)) return;
        Ray ray = playerCamera.ViewportPointToRay(new Vector3(0.5f, 0.5f, 0));
        if (Physics.Raycast(ray, out RaycastHit hit, interactRange, interactLayer))
        {
            IInteractable interactable = hit.collider.GetComponent<IInteractable>();
            if (interactable != null)
                interactable.Interact(this);
        }
    }

    void UpdateCarriedItem()
    {
        if (carriedItem != null && carryPoint != null)
        {
            carriedItem.transform.position = carryPoint.position;
            carriedItem.transform.rotation = carryPoint.rotation;
        }
    }

    public bool PickUpItem(GameObject item)
    {
        if (carriedItem != null) return false;
        carriedItem = item;
        carriedRigidbody = item.GetComponent<Rigidbody>();
        if (carriedRigidbody != null)
        {
            carriedRigidbody.isKinematic = true;
            carriedRigidbody.useGravity = false;
        }
        Collider col = item.GetComponent<Collider>();
        if (col != null) col.enabled = false;
        return true;
    }

    public GameObject DropItem()
    {
        GameObject item = carriedItem;
        if (item != null)
        {
            if (carriedRigidbody != null)
            {
                carriedRigidbody.isKinematic = false;
                carriedRigidbody.useGravity = true;
                carriedRigidbody.velocity = playerCamera.transform.forward * 2f + Vector3.up * 1f;
            }
            Collider col = item.GetComponent<Collider>();
            if (col != null) col.enabled = true;
            carriedItem = null;
            carriedRigidbody = null;
        }
        return item;
    }

    public bool HasItem() => carriedItem != null;
    public GameObject GetCarriedItem() => carriedItem;

    public void ShowMessage(string message, float duration = 2f)
    {
        uiManager?.ShowMessage(message, duration);
    }
}
