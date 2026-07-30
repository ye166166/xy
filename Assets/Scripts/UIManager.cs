using UnityEngine;
using UnityEngine.UI;
using System.Collections;

public class UIManager : MonoBehaviour
{
    private Text messageText;

    void Awake()
    {
        messageText = transform.Find("MessageText")?.GetComponent<Text>();
        if (messageText != null) messageText.text = "";
    }

    public void ShowMessage(string message, float duration = 2f)
    {
        if (messageText == null) return;
        StopAllCoroutines();
        StartCoroutine(DisplayMessage(message, duration));
    }

    IEnumerator DisplayMessage(string message, float duration)
    {
        messageText.text = message;
        messageText.enabled = true;
        yield return new WaitForSeconds(duration);
        messageText.enabled = false;
    }

    public void ClearMessage()
    {
        if (messageText == null) return;
        StopAllCoroutines();
        messageText.enabled = false;
    }
}
