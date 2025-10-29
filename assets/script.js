fetch('http://127.0.0.1:3490/api_call', {
  method: 'GET'
})
  .then(function (response) { return response.json(); })
  .then(function (json) {
    console.log(json);
  });

let file = "fetch_info.txt"
fetch (file)
.then(x => x.text())
.then(y => document.getElementById("demo").innerHTML = y);