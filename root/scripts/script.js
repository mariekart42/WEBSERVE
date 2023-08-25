const fileItemsContainer = document.getElementById("fileItems");

filePaths.forEach(filePath => {
    const fileItem = document.createElement("div");
    fileItem.className = "file-item"; // Add the "file-item" class

    const fileLink = document.createElement("a");
    fileLink.href = filePath;
    fileLink.textContent = filePath;
    fileLink.className = "file-link";

    const spacer = document.createElement("span");
    spacer.className = "spacer"; // Apply spacer class

    const deleteButton = document.createElement("button");
    deleteButton.className = "delete-button";
    deleteButton.textContent = "Delete";

    deleteButton.addEventListener("click", function() {
        confirmDelete(filePath);
    });

    const resultMessage = document.createElement("div");
    resultMessage.className = "index-result-message";

    fileItem.appendChild(fileLink);
    fileItem.appendChild(spacer); // Add spacer
    fileItem.appendChild(deleteButton);
    fileItem.appendChild(resultMessage);
    fileItemsContainer.appendChild(fileItem);
});

function confirmDelete(filePath) {
    const confirmation = confirm(`Are you sure you want to delete the file at path: ${filePath}?`);
    if (confirmation) {
        fetch(`http://${hostname}:${port}/${customEncodeURIComponent(filePath)}`, {
            method: "DELETE"
        })
            .then(response => {
                const resultMessage = document.querySelector(`[href="${filePath}"]`).nextSibling;
                if (response.ok){
                    resultMessage.textContent = " File deleted successfully";
                    spacer.classList.add("green-spacer"); // Apply the green spacer class
                }
                else {
                    resultMessage.textContent = " Error, file does not exist";
                    spacer.classList.add("green-spacer"); // Apply the green spacer class
                }
            })
            .catch(error => {
                console.error("An error occurred:", error);
            });
    }
}

function customEncodeURIComponent(uri) {
    return encodeURIComponent(uri).replace(/%2F/g, "/").replace(/^upload\//, "");
}