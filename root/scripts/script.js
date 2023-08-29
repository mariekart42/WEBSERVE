const fileItemsContainer = document.getElementById("fileItems");
const hostname = window.location.hostname;
const port = window.location.port;
filePaths.forEach(filePath => {
    const fileItem = document.createElement("div");
    fileItem.className = "file-item"; // Add the "file-item" class

    const fileLink = document.createElement("a");
    fileLink.href = filePath;
    fileLink.textContent = filePath;
    fileLink.className = "file-link";


    // Here you can set the href attribute to a different variable
    // Let's assume your different variable is named 'downloadPath'
    // const downloadPath = filePath;
    // fileLink.href = downloadPath;


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


// document.addEventListener("DOMContentLoaded", function() {
//     const fileItemsContainer = document.getElementById("fileItems");
//     const hostname = window.location.hostname;
//     const port = window.location.port;
//
//     // Parse the downloadPathMappingString into a JavaScript object
//     var downloadPathMapping = JSON.parse(downloadPathMappingString);
//
//     // Get the first key-value pair from the mapping
//     var firstFile = Object.keys(downloadPathMapping)[0];
//     var firstPath = downloadPathMapping[firstFile];
//
//     // Remove any extra slashes from the root folder
//     const rootFolder = "/path/to/your/root/folder/".replace(/\/+$/, "");
//
//     // Create the anchor element for the first file
//     const firstFileItem = document.createElement("div");
//     firstFileItem.className = "file-item";
//
//     const firstFileLink = document.createElement("a");
//     firstFileLink.href = firstPath; // Use the full path as href
//     firstFileLink.textContent = firstFile;
//     firstFileLink.className = "file-link";
//
//     const spacer = document.createElement("span");
//     spacer.className = "spacer";
//
//     const deleteButton = document.createElement("button");
//     deleteButton.className = "delete-button";
//     deleteButton.textContent = "Delete";
//
//     deleteButton.addEventListener("click", function() {
//         confirmDelete(firstPath);
//     });
//
//     const resultMessage = document.createElement("div");
//     resultMessage.className = "index-result-message";
//
//     firstFileItem.appendChild(firstFileLink);
//     firstFileItem.appendChild(spacer);
//     firstFileItem.appendChild(deleteButton);
//     firstFileItem.appendChild(resultMessage);
//     fileItemsContainer.appendChild(firstFileItem);
//
//     // Iterate through the rest of the files
//     for (const filePath in downloadPathMapping) {
//         if (filePath !== firstFile) {
//             const fileItem = document.createElement("div");
//             fileItem.className = "file-item";
//
//             const fileLink = document.createElement("a");
//             const downloadPath = downloadPathMapping[filePath];
//
//             // Remove any extra slashes from the download path
//             const formattedDownloadPath = downloadPath.replace(/^\/+/, "");
//
//             fileLink.href = formattedDownloadPath;
//             fileLink.textContent = filePath;
//             fileLink.className = "file-link";
//
//             const spacer = document.createElement("span");
//             spacer.className = "spacer";
//
//             const deleteButton = document.createElement("button");
//             deleteButton.className = "delete-button";
//             deleteButton.textContent = "Delete";
//
//             deleteButton.addEventListener("click", function() {
//                 confirmDelete(formattedDownloadPath);
//             });
//
//             const resultMessage = document.createElement("div");
//             resultMessage.className = "index-result-message";
//
//             fileItem.appendChild(fileLink);
//             fileItem.appendChild(spacer);
//             fileItem.appendChild(deleteButton);
//             fileItem.appendChild(resultMessage);
//             fileItemsContainer.appendChild(fileItem);
//         }
//     }
//
//
// function confirmDelete(filePath) {
//     const confirmation = confirm(`Are you sure you want to delete the file at path: ${filePath}?`);
//     if (confirmation)
//     {
//         fetch(`http://${hostname}:${port}/${customEncodeURIComponent(filePath)}`,
//             {
//             method: "DELETE"
//         })
//             .then(response =>
//             {
//                 const resultMessage = document.querySelector(`[href="${filePath}"]`).nextSibling;
//                 if (response.ok) {
//                     resultMessage.textContent = "File deleted successfully";
//                     resultMessage.classList.add("success"); // Apply the success class
//                 } else if (response.status === 403) {
//                     resultMessage.textContent = "Sowwy, mom said no";
//                     resultMessage.classList.add("permission-denied");
//                 } else {
//                     resultMessage.textContent = "Error, file does not exist";
//                     resultMessage.classList.add("error"); // Apply the error class
//                 }
//             })
//             .catch(error =>
//             {
//                 console.error("An error occurred:", error);
//             });
//
//     }
// }
//
// function customEncodeURIComponent(uri) {
//     return encodeURIComponent(uri).replace(/%2F/g, "/").replace(/^upload\//, "");
// }}