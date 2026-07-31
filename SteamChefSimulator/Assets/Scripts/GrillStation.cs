using UnityEngine;

public class GrillStation : MonoBehaviour, IInteractable
{
    public float cookTime = 8f;
    public float burnTime = 14f;

    private bool isCooking = false;
    private float cookingTimer = 0f;
    private GameObject currentSteak;
    private FoodItem currentFoodItem;
    private PlayerInteraction currentPlayer;
    private bool isReady = false;

    private Material rawMat;
    private Material cookedMat;
    private Material burntMat;

    void Start()
    {
        rawMat = new Material(Shader.Find("Standard")) { color = Color.red };
        cookedMat = new Material(Shader.Find("Standard")) { color = new Color(0.45f, 0.25f, 0.08f) };
        burntMat = new Material(Shader.Find("Standard")) { color = Color.black };
        CreateVisual();
    }

    void CreateVisual()
    {
        GameObject visual = new GameObject("GrillVisual");
        visual.transform.SetParent(transform);
        visual.transform.localPosition = Vector3.zero;

        GameObject counter = GameObject.CreatePrimitive(PrimitiveType.Cube);
        counter.name = "Counter";
        counter.transform.SetParent(visual.transform);
        counter.transform.localPosition = new Vector3(0, -0.25f, 0);
        counter.transform.localScale = new Vector3(1.5f, 0.5f, 1.5f);
        counter.GetComponent<Renderer>().material = new Material(Shader.Find("Standard")) { color = new Color(0.55f, 0.55f, 0.55f) };

        GameObject pan = GameObject.CreatePrimitive(PrimitiveType.Cube);
        pan.name = "Pan";
        pan.transform.SetParent(visual.transform);
        pan.transform.localPosition = new Vector3(0, 0.25f, 0);
        pan.transform.localScale = new Vector3(0.9f, 0.06f, 0.9f);
        pan.GetComponent<Renderer>().material = new Material(Shader.Find("Standard")) { color = new Color(0.25f, 0.25f, 0.25f) };
    }

    void Update()
    {
        if (!isCooking || currentSteak == null) return;
        cookingTimer += Time.deltaTime;

        if (cookingTimer < cookTime)
        {
            UpdateVisual(rawMat);
        }
        else if (cookingTimer < burnTime)
        {
            if (currentFoodItem != null) currentFoodItem.SetState(FoodState.Cooked);
            UpdateVisual(cookedMat);
            if (!isReady)
            {
                isReady = true;
                currentPlayer?.ShowMessage("Steak is cooked! Click to take it.");
            }
        }
        else
        {
            currentPlayer?.ShowMessage("Steak burnt! Discarded.");
            DestroySteak();
        }
    }

    void UpdateVisual(Material mat)
    {
        if (currentSteak != null)
        {
            Renderer r = currentSteak.GetComponent<Renderer>();
            if (r != null) r.material = mat;
        }
    }

    public void Interact(PlayerInteraction player)
    {
        currentPlayer = player;

        if (player.HasItem())
        {
            GameObject held = player.GetCarriedItem();
            FoodItem food = held.GetComponent<FoodItem>();
            if (food != null && food.itemType == FoodType.Steak && food.currentState == FoodState.Raw)
            {
                player.DropItem();
                PlaceSteak(held);
            }
            else
            {
                player.ShowMessage("Can't place that on the grill.");
            }
        }
        else if (isReady && currentSteak != null)
        {
            currentSteak.transform.SetParent(null);
            if (player.PickUpItem(currentSteak))
                ResetGrill();
        }
        else if (!isCooking)
        {
            SpawnSteak();
            isCooking = true;
            cookingTimer = 0f;
            isReady = false;
            player.ShowMessage("Raw steak on grill... cooking.");
        }
        else
        {
            player.ShowMessage("Steak is cooking... wait.");
        }
    }

    void SpawnSteak()
    {
        currentSteak = GameObject.CreatePrimitive(PrimitiveType.Cube);
        currentSteak.name = "Steak";
        currentSteak.transform.localScale = new Vector3(0.35f, 0.08f, 0.45f);
        currentSteak.transform.position = transform.position + new Vector3(0, 0.3f, 0);
        currentSteak.transform.rotation = Quaternion.identity;
        currentSteak.GetComponent<Renderer>().material = rawMat;

        Rigidbody rb = currentSteak.AddComponent<Rigidbody>();
        rb.mass = 0.5f;
        rb.isKinematic = true;
        rb.useGravity = false;

        currentFoodItem = currentSteak.AddComponent<FoodItem>();
        currentFoodItem.itemType = FoodType.Steak;
        currentFoodItem.SetState(FoodState.Raw);

        Collider col = currentSteak.GetComponent<Collider>();
        if (col != null) col.enabled = false;
    }

    void PlaceSteak(GameObject steak)
    {
        currentSteak = steak;
        currentFoodItem = steak.GetComponent<FoodItem>();
        steak.transform.position = transform.position + new Vector3(0, 0.3f, 0);
        steak.transform.rotation = Quaternion.identity;
        Rigidbody rb = steak.GetComponent<Rigidbody>();
        if (rb != null) { rb.isKinematic = true; rb.useGravity = false; }
        Collider col = steak.GetComponent<Collider>();
        if (col != null) col.enabled = false;
        isCooking = true;
        cookingTimer = 0f;
        isReady = false;
    }

    void DestroySteak()
    {
        if (currentSteak != null) Destroy(currentSteak);
        ResetGrill();
    }

    void ResetGrill()
    {
        currentSteak = null;
        currentFoodItem = null;
        isCooking = false;
        cookingTimer = 0f;
        isReady = false;
    }
}
