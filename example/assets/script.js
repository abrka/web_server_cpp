fetch("fetch_info.txt")
  .then(x => x.text())
  .then(y => document.getElementById("demo").innerHTML = y);


fetch("/files", {
  method: 'GET'
})
  .then(function (response) {
    return response.json();
  })
  .then(function (json) {
    console.log(json);
    
    const myList = document.getElementById('filename-list');
    console.log("p1");
    for (let index = 0; index < json["filenames"].length; index++) {
      console.log("p2");
      const element = json["filenames"][index];
      const filename = "filename is " + element["filename"];
      const newListItem = document.createElement('li');
      newListItem.textContent = filename;
      myList.appendChild(newListItem);
    }
  });


