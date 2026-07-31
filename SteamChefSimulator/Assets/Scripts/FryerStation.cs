using UnityEngine;

public class FryerStation : MonoBehaviour, IInteractable
{
    public float cookTime = 6f;
    public float burnTime = 11f;

    private bool isCooking = false;
    private float cookingTimer = 0f;
    private GameObject currentFries;
    private FoodItem currentFoodItem;
    private PlayerInteraction currentPlayer;
    private bool isReady = false;

    private Material rawMat;
    private Material cookedMat;
    private Material burntMat;

    void Start()
    {
        rawMat = new Material(Shader.Find("Standard")) { color = new Color(0.95f, 0.85f, 0.4f) };
        cookedMat = new Material(Shader.Find("Standard")) { color = new Color(0.85f, 0.55f, 0.08f) };
        burntMat = new Material(Shader.Find("Standard")) { color = Color.black };
        CreateVisual();
    }

    void CreateVisual()
    {
        GameObject visual = new GameObject("FryerVisual");
        visual.transform.SetParent(transform);
        visual.transform.localPosition = Vector3.zero;

        GameObject counter = GameObject.CreatePrimitive(PrimitiveType.Cube);
        counter.name = "Counter";
        counter.transform.SetParent(visual.transform);
        counter.transform.localPosition = new Vector3(0, -0.25f, 0);
        counter.transform.localScale = new Vector3(1.5f, 0.5f, 1.5f);
        counter.GetComponent<Renderer>().material = new Material(Shader.Find("Standard")) { color = new Color(0.55f, 0.55f, 0.55f) };

        GameObject basket = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
        basket.name = "Basket";
        basket.transform.SetParent(visual.transform);
        basket.transform.localPosition = new Vector3(0, 0.15f, 0);
        basket.transform.localScale = new Vector3(0.85f, 0.15f, 0.85f);
        basket.GetComponent<Renderer>().material = new Material(Shader.Find("Standard")) { color = new Color(0.3f, 0.3f, 0.3f) };

        GameObject oil = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
        oil.name = "Oil";
        oil.transform.SetParent(basket.transform);
        oil.transform.localPosition = new Vector3(0, 0.12f, 0);
        oil.transform.localScale = new Vector3(0.75f, 0.06f, 0.75f);
        oil.GetComponent<Renderer>().material = new Material(Shader.Find("Standard")) { color = new Color(0.8f, 0.6f, 0.05f) };
    }

    void Update()
    {
        if (!isCooking || currentFries == null) return;
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
                currentPlayer?.ShowMessage("Fries are ready! Click to take them.");
            }
        }
        else
        {
            currentPlayer?.ShowMessage("Fries burnt! Discarded.");
            DestroyFries();
        }
    }

    void UpdateVisual(Material mat)
    {
        if (currentFries != null)
        {
            Renderer r = currentFries.GetComponent<Renderer>();
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
            if (food != null && food.itemType == FoodType.Fries && food.currentState == FoodState.Raw)
            {
                player.DropItem();
                PlaceFries(held);
            }
            else
            {
                player.ShowMessage("Can't place that in the fryer.");
            }
        }
        else if (isReady && currentFries != null)
        {
            currentFries.transform.SetParent(null);
            if (player.PickUpItem(currentFries))
                ResetFryer();
        }
        else if (!isCooking)
        {
            SpawnFries();
            isCooking = true;
            cookingTimer = 0f;
            isReady = false;
            player.ShowMessage("Raw fries in fryer... cooking.");
        }
        else
        {
            player.ShowMessage("Fries are cooking... wait.");
        }
    }

    void SpawnFries()
    {
        currentFries = GameObject.CreatePrimitive(PrimitiveType.Cylinder);
        currentFries.name = "Fries";
        currentFries.transform.localScale = new Vector3(0.14f, 0.22f, 0.14f);
        currentFries.transform.position = transform.position + new Vector3(0, 0.3f, 0);
        currentFries.transform.rotation = Quaternion.identity;
        currentFries.GetComponent<Renderer>().material = rawMat;

        Rigidbody rb = currentFries.AddComponent<Rigidbody>();
        rb.mass = 0.2f;
        rb.isKinematic = true;
        rb.useGravity = false;

        currentFoodItem = currentFries.AddComponent<FoodItem>();
        currentFoodItem.itemType = FoodType.Fries;
        currentFoodItem.SetState(FoodState.Raw);

        Collider col = currentFries.GetComponent<Collider>();
        if (col != null) col.enabled = false;
    }

    void PlaceFries(GameObject fries)
    {
        currentFries = fries;
        currentFoodItem = fries.GetComponent<FoodItem>();
        fries.transform.position = transform.position + new Vector3(0, 0.3f, 0);
        fries.transform.rotation = Quaternion.identity;
        Rigidbody rb = fries.GetComponent<Rigidbody>();
        if (rb != null) { rb.isKinematic = true; rb.useGravity = false; }
        Collider col = fries.GetComponent<Collider>();
        if (col != null) col.enabled = false;
        isCooking = true;
        cookingTimer = 0f;
        isReady = false;
    }

    void DestroyFries()
    {
        if (currentFries != null) Destroy(currentFries);
        ResetFryer();
    }

    void ResetFryer()
    {
        currentFries = null;
        currentFoodItem = null;
        isCooking = false;
        cookingTimer = 0f;
        isReady = false;
    }
}
