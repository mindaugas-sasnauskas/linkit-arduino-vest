// debug
$(document).ready(function() {
  var postTest;
  $("#submit").click(function() {
    postTest = $("#postTest").val();
    $.post("/", {
      postTest: postTest
    }, function(data) {
      if (data === 'received') {
        console.log(data + ", post success");
      }
    });
  });

  $(".deb").click(function() {
    console.log("data_chart deb ");
    console.log(data_chart);
  })
});
// END debug


var domain = document.domain;

data_chart = {
  labels: [],
  series: [
    [],
    [],
    [],
    [],
    []
  ]
};


var host = location.origin.replace(/^http/, 'ws')
var ws = new WebSocket(host);
ws.open = function(event) {
  // ws.send('something');
};


ws.onmessage = function(event) {
  message = JSON.parse(event.data);
  console.log(event.data);
  if (message.items[0].ti) {
    if (message.type == "add") {
      for (var i = 0; i < message.items.length; i++) {
        data_chart.labels.unshift(message.items[i].ti);
        data_chart.series[0].unshift(message.items[i].t1);
        data_chart.series[1].unshift(message.items[i].t2);
        data_chart.series[2].unshift(message.items[i].t3);
        data_chart.series[3].unshift(message.items[i].t4);
        data_chart.series[4].unshift(message.items[i].t5);
      }
    } else {
      for (var i = 0; i < message.items.length; i++) {
        data_chart.labels.push(message.items[i].ti);
        data_chart.series[0].push(message.items[i].t1);
        data_chart.series[1].push(message.items[i].t2);
        data_chart.series[2].push(message.items[i].t3);
        data_chart.series[3].push(message.items[i].t4);
        data_chart.series[4].push(message.items[i].t5);
      }
    }
  }


  draw(data_chart);
};
ws.onclose = function(event) {
  console.log('err');
  setTimeout(function() {
    location.reload();
  }, 100);
};
//ws end
var pos_left = 0;
var range = 1;

function zoomChart(type) {

  if (type == "in") {
    for (var i = 0; i < range; i++) {
      for (var i = 0; i < data_chart.series.length; i++) {
        data_chart.series[i].shift();
      }
      data_chart.labels.shift();
    }
  } else if (type == "out") {
    var lab_len_ini = data_chart.labels.length;
    var lab_len = data_chart.labels.length;
    var pos = lab_len_ini - lab_len;
    ws.send(JSON.stringify({
      "type": "add",
      "range": range,
      "range_now": data_chart.series[0].length
    }))
  } else if (type == "left") {
    console.log("left");
    for (var i = 0; i < range; i++) {

      for (var i = 0; i < data_chart.series.length; i++) {
        data_chart.series[i].pop();
      }
      data_chart.labels.pop();
      pos_left++;
    }
  } else if (type == "right") {
    console.log("right");
    if (pos_left > 0) {
      ws.send(JSON.stringify({
        "type": "add_left",
        "range": range,
        "range_now": data_chart.series[0].length,
        "pos_left": pos_left
      }))

      for (var i = 0; i < range; i++) {
        pos_left--;
      }
    }
  }
  console.log(pos_left);
  draw(data_chart);
}

function requestData(type) {
  ws.send(JSON.stringify({
    "type": "requestData"
  }))
}

function draw(data_dr) {
  // var options = {
  //   fullWidth: true,
  //   chartPadding: {
  //     right: 5,
  //     left: -5
  //   }
  // };
  // new Chartist.Line('.ct-chart', data_dr, options);
}



////////// Gcharts



google.load('visualization', '1', {
  packages: ['corechart', 'line']
});
google.setOnLoadCallback(drawBasic);

function drawBasic() {

  var data = new google.visualization.DataTable();
  data.addColumn('timeofday', 'Time of Day');
  data.addColumn('number', 't1');
  data.addColumn('number', 't2');

  data.addRows([
    [[8, 30, 45], 5,5],
        [[9, 0, 0], 10,5],
        [[10, 0, 0, 0], 12,5],
        [[10, 45, 0, 0], 13,55],
        [[11, 0, 0, 0], 15,5],
        [[12, 15, 45, 0], 20,55],
        [[13, 0, 0, 0], 22,5],
        [[14, 30, 0, 0], 25,55],
        [[15, 12, 0, 0], 30,5],
        [[16, 45, 0], 32,25],
        [[16, 59, 0], 42,5]
  ]);

  var options = {
    hAxis: {
      title: 'Time'
    },
    vAxis: {
      title: 'Temperature'
    },
    legend: {
      position: 'none'
    },
    explorer: {
      keepInBounds: true,
      axis: 'horizontal'
    }
  };

  var chart = new google.visualization.LineChart(document.getElementById('chart_div'));

  chart.draw(data, options);
}
