const fileItemsContainer = document.getElementById("fileItems");
const hostname = window.location.hostname;
const port = window.location.port;
filePaths.forEach(filePath => {
    const fileItem = document.createElement("div");
    fileItem.className = "file-item";

    const fileLink = document.createElement("a");
    fileLink.href = filePath;
    fileLink.textContent = filePath;
    fileLink.className = "file-link";

    const spacer = document.createElement("span");
    spacer.className = "spacer";

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

// let currentPath = ""; // Initialize the current path
// filePaths.forEach(filePath => {
//     const fileItem = document.createElement("div");
//     fileItem.className = "file-item";
//
//     const isFolder = filePath.endsWith('/'); // Check if it's a folder
//
//     const fileLink = document.createElement("a");
//
//     if (isFolder) {
//         // For folders, display the full path as the link text
//         fileLink.href = filePath;
//         fileLink.textContent = filePath;
//         currentPath = filePath; // Update the current path
//     } else {
//         // For files, display only the filename and set the link to the current path
//         fileLink.href = currentPath + filePath;
//         fileLink.textContent = filePath.split('/').pop();
//     }
//
//     fileLink.className = "file-link";
//
//     const spacer = document.createElement("span");
//     spacer.className = "spacer";
//
//     const deleteButton = document.createElement("button");
//     deleteButton.className = "delete-button";
//     deleteButton.textContent = "Delete";
//
//     deleteButton.addEventListener("click", function() {
//         confirmDelete(filePath);
//     });
//
//     const resultMessage = document.createElement("div");
//     resultMessage.className = "index-result-message";
//
//     fileItem.appendChild(fileLink);
//     fileItem.appendChild(spacer); // Add spacer
//     fileItem.appendChild(deleteButton);
//     fileItem.appendChild(resultMessage);
//     fileItemsContainer.appendChild(fileItem);
// });


function confirmDelete(filePath) {
    const confirmation = confirm(`Are you sure you want to delete the file at path: ${filePath}?`);
    if (confirmation) {
        fetch(`http://${hostname}:${port}/${customEncodeURIComponent(filePath)}`, {
            method: "DELETE"
        })
        .then(response => {
            const resultMessage = document.querySelector(`[href="${filePath}"]`).nextSibling;
            if (response.ok) {
                resultMessage.textContent = "File deleted successfully";
                resultMessage.classList.add("success"); // Apply the success class
            } else if (response.status === 403) {
                resultMessage.textContent = "Sowwy, mom said no";
                resultMessage.classList.add("permission-denied");
            } else if (response.status === 405){
                resultMessage.textContent = "Delete Method not allowed!";
                resultMessage.classList.add("error");
            } else{
                resultMessage.textContent = "Error, file does not exist";
                resultMessage.classList.add("error"); // Apply the error class
            }
        })
        .catch(error => {
            console.error("An error occurred:", error);
        });
    }
}

function customEncodeURIComponent(uri) {
    return encodeURIComponent(uri).replace(/%2F/g, "/");//.replace(/^upload\//, "");
}