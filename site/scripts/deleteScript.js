const hostname = window.location.hostname;
const port = window.location.port;
const resultMessage = document.getElementById("resultMessage"); // Get the result message element

// if (confirmation) {

document.getElementById("deleteButton").addEventListener("click", function() {
    const filePath = document.getElementById("filePath").value;
    const confirmation = confirm(`Are you sure you want to delete the file at path: ${filePath}?`);
    if (confirmation) {
        fetch(`http://${hostname}:${port}/${encodeURIComponent(filePath)}`, {
            method: "DELETE"
        })
            .then(response => {
                if (response.ok)
                    resultMessage.textContent = "File deleted successfully.";
                else {
                    resultMessage.textContent = "Error deleting file.";
                }
            })
            .catch(error => {
                console.error("An error occurred:", error);
            });
    }
});
// }