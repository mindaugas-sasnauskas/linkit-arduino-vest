var WebSocketServer = require("ws").Server
var http = require("http")
var express = require("express")
var app = express()
var port = process.env.PORT || 5000

app.use(express.static(__dirname + "/public/"))

var server = http.createServer(app)
server.listen(port)

console.log("http server listening on %d", port)

// post
var debbb;

var bodyParser = require("body-parser");
app.use(bodyParser.json()); // to support JSON-encoded bodies
app.use(bodyParser.urlencoded({ // to support URL-encoded bodies
  extended: true
}));
app.post('/linkit-data', function(req, res) {
  var data = JSON.stringify(req.body)
    // console.log(req.body.value + " value");
  console.log("body " + data);

  write2file(data);
  res.end("received");
});
app.post('/', function(req, res) {
  var data = JSON.stringify(req.body)
  console.log("body " + data);

  write2file(data);
  res.end("received");
});
var fs = require('fs');
var fileName2read = "rrr";
var file2read_path = "public/data/";

function write2file(obj) {
  obj.replace("t2", " egfweffwefwefwefweqgf");
  obj.replace("]", " ");

//  console.log("JSON.parse(obj)33 " + obj.length);
  var fileName = fileNameF(fileName2read);
  var path = file2read_path + fileName + ".json";
  var dataFile = fs.readFileSync(path);
  var data = JSON.parse(dataFile);
  var obj2push = JSON.parse(obj);
  // console.log(obj2push.length);
  for (var i = 0; i < obj2push.length; i++) {
    data.items.push(obj2push[i]);
  }

  var dataJSON = JSON.stringify(data);
  fs.writeFileSync(path, dataJSON);
}

function fileNameF(fileName2read) {
  var d = new Date();
  d = d.toDateString();
  if (fileName2read !== d) {
    fileName2read = d;
    var ini = "{\"items\": []}";
    // fs.access(path[, mode],
    // fs.writeFileSync(file2read_path + fileName2read + ".json", ini);
    // fs.open(file2read_path + fileName2read + ".json", "wx", function (err, fd) {
    //     // handle error
    //     fs.close(fd, function (err) {
    //       // handle error
    //     });
    //   })
    fs.stat(file2read_path + fileName2read + ".json", function(err, stat) {
      console.log(err ? 'no access!' : 'can read/write');
      if (err) {
        fs.writeFileSync(file2read_path + fileName2read + ".json", ini);
      }
    });
  }
  return fileName2read;
}

function readFile(file) {
  var path = "public/data/" + file + ".json";
  var dataFile = fs.readFileSync(path);
  var data = JSON.parse(dataFile);
  // console.log(data);
  var dataJSON = JSON.stringify(data);
  return data;
  // return dataJSON;
}


//ws

var wss = new WebSocketServer({
  server: server
})
console.log("websocket server created")

var file2read = "data01"
wss.on('connection', function connection(ws) {
  ws.on('message', function incoming(message) {
    console.log('received: %s', message);
    message = JSON.parse(message);
    console.log('received: %s', message.type);
    var dataJSON;
    if (message.type == "requestData") {
      dataJSON = readFile(file2read);
      ws.send(JSON.stringify(dataJSON));
    } else if (message.type == "add") {

      dataJSON = readFile(file2read);
      dataJSON.type = "add";
      var dataJSON_add = {
        "type": "add",
        "items": []
      };
      var data_leng = dataJSON.items.length;
      for (var i = 0; i < message.range; i++) {
        dataJSON_add.items.push(dataJSON.items[data_leng - message.range_now - i])
      }
      ws.send(JSON.stringify(dataJSON_add));

    } else if (message.type == "add_left") {
      dataJSON = readFile(file2read);
      var dataJSON_add = {
        "items": []
      };
      var data_leng = dataJSON.items.length;
      for (var i = 0; i < message.range; i++) {
        dataJSON_add.items.push(dataJSON.items[data_leng - message.pos_left + i])
      }
      ws.send(JSON.stringify(dataJSON_add));
    }
  });
  // var dataJSON = readFile("data01");
  var dataJSON = readFile(fileNameF("fileName2read"));
  ws.send(JSON.stringify(dataJSON));

});



//// interval thing

// var minutes = 5;
// the_interval = minutes * 1 * 1000;
// setInterval(function () {
//   console.log("I am doing my 5 minutes check");
//   var objDate = new Date();
//   var hours = objDate.getHours();
//   var minutes = objDate.getMinutes();
//   ws.send(dataJSON);
//   console.log("time: " + hours + " " + minutes);
// }, the_interval);
